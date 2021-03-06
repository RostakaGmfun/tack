#include "tack/network_device.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netinet/in.h>

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <mutex>

namespace tack {

network_device::network_device(std::string name,
        size_t num_multiqueue_devices, size_t mtu):
        name_(name)
{

    if (!num_multiqueue_devices) {
        throw std::invalid_argument("Number of devices can't be 0");
    }

    if (!init_tap(num_multiqueue_devices)) {
        cleanup();
        throw std::runtime_error("Failed to init TAP device");
    }

    sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd_ < 0) {
        cleanup();
        throw std::runtime_error("Failed to open socket: " +
                std::string(strerror(errno)));
    }

    if (!set_mtu(mtu)) {
        cleanup();
        throw std::runtime_error("Failed to set device MTU");
    }

    if (!get_if_addr()) {
        cleanup();
        throw std::runtime_error("Failed to retrieve hardware address");
    }

}

network_device::~network_device()
{
    cleanup();
}

void network_device::cleanup()
{
    for (auto fd : device_fds_) {
        if (fd) {
            close(fd);
        }
    }

    if (sock_fd_) {
        close(sock_fd_);
    }
}

bool network_device::set_mtu(size_t mtu)
{
    if (!mtu) {
        return false;
    }

    ifreq ifr;

    strncpy(ifr.ifr_name, name_.c_str(), sizeof(ifr.ifr_name));
    ifr.ifr_addr.sa_family = AF_INET;
    ifr.ifr_mtu = mtu;

    if (ioctl(sock_fd_, SIOCSIFMTU, (void *)&ifr) < 0) {
        std::cerr << strerror(errno) << std::endl;
        return false;
    }

    mtu_ = mtu;

    std::cout << "MTU set to " << mtu_ << std::endl;
    return true;
}

bool network_device::get_if_addr()
{
    ifreq ifr;

    strncpy(ifr.ifr_name, name_.c_str(), sizeof(ifr.ifr_name));

    if (ioctl(sock_fd_, SIOCGIFHWADDR, (void *)&ifr) < 0) {
        std::cerr << strerror(errno) << std::endl;
        return false;
    }

    std::copy(ifr.ifr_hwaddr.sa_data, ifr.ifr_hwaddr.sa_data+6, &hw_addr_[0]);
    std::cout << "Retrieved hardware address: " << hw_addr_ << '\n';
    return true;
}

ipv4_address network_device::get_ipaddr() const
{
    static std::mutex mx;
    std::lock_guard<std::mutex> lock(mx);
    static ipv4_address addr;
    static bool retrieved = false;
    if (!retrieved) {
        ifreq ifr;

        strncpy(ifr.ifr_name, name_.c_str(), sizeof(ifr.ifr_name));

        if (ioctl(sock_fd_, SIOCGIFADDR, (void *)&ifr) < 0) {
            std::cerr << strerror(errno) << std::endl;
            return ipv4_address{0, 0, 0, 0};
        }

        sockaddr_in *sin = reinterpret_cast<sockaddr_in*>(&ifr.ifr_addr);
        uint8_t *saddr = reinterpret_cast<uint8_t*>(&sin->sin_addr);
        std::copy(saddr, saddr+4, &addr[0]);
        std::cout << "Retrieved IPv4 address: " << addr << '\n';
        retrieved = true;
    }

    return addr;
}

bool network_device::init_tap(size_t num_devices)
{
    ifreq ifr;

    strncpy(ifr.ifr_name, name_.c_str(), sizeof(ifr.ifr_name));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI | IFF_MULTI_QUEUE;

    for (size_t i = 0;i<num_devices; i++) {
        int fd = open("/dev/net/tun", O_RDWR);
        if (fd < 0) {
            std::cerr << strerror(errno) << std::endl;
            return false;
        }

        if (ioctl(fd, TUNSETIFF, (void *)&ifr) < 0) {
            if (errno == E2BIG) {
                // The limit of device queue is reached.
                // Let's report about this but don't fail
                std::cerr << "Maximum number of devices reached: " << i << std::endl;
                break;
            } else {
                std::cerr << strerror(errno) << std::endl;
                return false;
            }
        }
        device_fds_.push_back(fd);
    }

    return true;
}

}
