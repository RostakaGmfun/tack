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

sockbuf::sockbuf(sockbuf &&other)
{
    delete [] buffer_;
    buffer_ = other.buffer_;
    size_ = other.size_;
    header_len_ = other.header_len_;
    head_ = other.head_;
    headers_ = std::move(other.headers_);
    payload_ = other.payload_;
    end_ = other.end_;

    other.buffer_ = nullptr;
    other.size_ = 0;
    other.header_len_ = 0;
    other.head_ = nullptr;
    other.headers_.clear();
    other.payload_ = nullptr;
    other.end_ = nullptr;
}

sockbuf &sockbuf::operator=(sockbuf &&other)
{
    delete [] buffer_;
    buffer_ = other.buffer_;
    size_ = other.size_;
    header_len_ = other.header_len_;
    head_ = other.head_;
    headers_ = std::move(other.headers_);
    payload_ = other.payload_;
    end_ = other.end_;

    other.buffer_ = nullptr;
    other.size_ = 0;
    other.header_len_ = 0;
    other.head_ = nullptr;
    other.headers_.clear();
    other.payload_ = nullptr;
    other.end_ = nullptr;

    return *this;
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
