#ifndef NETWORK_DEVICE_HPP
#define NETWORK_DEVICE_HPP

#include <string>
#include <vector>

namespace tack
{

enum class network_device_type {
    tun,
    tap
};

class network_device {
public:
    network_device(std::string name,
            network_device_type type, size_t mtu = 1500);

    ~network_device();

    const uint8_t *get_buffer() const { return packet_buffer_; }
    int get_device_fd() const { return device_fd_; }
    network_device_type get_type() const { return type_; }
    size_t get_device_mtu() const { return device_mtu_; }
    bool set_device_mtu(size_t mtu);

private:
    bool init_tun();
    bool init_tap();
    bool init_pkb();

private:
    int device_fd_;
    int sock_fd_;
    size_t device_mtu_;
    std::string device_name_;

    uint8_t *packet_buffer_;

    network_device_type type_;
};

} // namespace tack

#endif // NETWORK_DEVICE_HPP
