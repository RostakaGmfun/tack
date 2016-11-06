#include "tack/sockbuf.hpp"

#include <stdexcept>

namespace tack {

sockbuf::sockbuf(size_t size, size_t header_len):
    size_(size)
{
    if (header_len >= size) {
        throw std::runtime_error("header_len >= size");
    }
    buffer_ = new uint8_t[size_];
    clear_all(header_len);
}

sockbuf::~sockbuf()
{
    if (buffer_) {
        delete [] buffer_;
    }
}

size_t sockbuf::num_headers() const
{
    return headers_.size();
}

bool sockbuf::add_payload(const uint8_t *payload, size_t size)
{
    if (payload == nullptr || size == 0) {
        return false;
    }

    if (head_ + size_ - end_ < static_cast<ptrdiff_t>(size)) {
        return false; // not enough space
    }

    std::copy(payload, payload + size, end_);
    end_ += size;
    return true;
}

uint8_t *sockbuf::payload()
{
    return payload_;
}

const uint8_t *sockbuf::payload() const
{
    return const_cast<sockbuf *>(this)->payload();
}

size_t sockbuf::payload_size() const
{
    return end_ - payload_;
}

uint8_t *sockbuf::raw()
{
    return head_;
}

const uint8_t *sockbuf::raw() const
{
    return const_cast<sockbuf *>(this)->payload();
}

size_t sockbuf::raw_size() const
{
    return end_ - head_;
}

void sockbuf::clear_payload()
{
    end_ = payload_;
}

bool sockbuf::clear_all(size_t header_len)
{
    if (header_len > size_) {
        return false;
    }

    header_len_ = header_len;
    payload_ = buffer_ + header_len_;
    end_ = payload_;
    head_ = payload_;
    headers_.clear();

    return true;
}

}
