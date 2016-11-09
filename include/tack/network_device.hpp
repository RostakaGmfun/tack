#ifndef NETWORK_DEVICE_HPP
#define NETWORK_DEVICE_HPP

#include <string>
#include <vector>

#include "tack/addresses.hpp"

namespace tack
{

class network_device {
public:
    network_device(std::string name,
            size_t num_multiqueue_devices = 1,
            size_t mtu = 1500);

    ~network_device();

    int get_device_fd(int index = 0) const { return device_fds_[index]; }
    size_t get_num_devices() const { return device_fds_.size(); }

    size_t get_mtu() const { return mtu_; }
    bool set_mtu(size_t mtu);

    const std::string &get_name() const { return name_; }

    const hw_address &get_hwaddr() const { return hw_addr_; }

private:
    bool init_tap(size_t num_devices);
    bool get_if_addr();

    void cleanup();

private:
    std::vector<int> device_fds_;
    int sock_fd_;
    size_t mtu_;
    std::string name_;

    hw_address hw_addr_;
};

} // namespace tack

#endif // NETWORK_DEVICE_HPP
