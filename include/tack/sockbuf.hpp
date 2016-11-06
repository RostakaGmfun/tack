#ifndef SOCKBUF_HPP
#define SOCKBUF_HPP

#include <cstdint>
#include <cstddef>
#include <tuple>
#include <list>
#include <typeindex>

namespace tack {

/**
 * Socket buffer implementation.
 *
 * Internal structure:
 *
 * buffer_          +-------------------+
 * head_            |____free space_____|
 *                  |______header_0_____|
 *                  |________...________|
 * payload_         |_____data start____|
 *                  |________...________|
 * end_             |______data end_____|
 * buffer_+size_    |_____free space____|
 */
class sockbuf
{
public:
    /**
     * @param size Total size of buffer
     * @param header_len Maximum total size of protocol headers in bytes.
     */
    sockbuf(size_t size, size_t header_len);

    // TODO: consider implementing these
    sockbuf(const sockbuf &other) = delete;
    sockbuf(sockbuf &&rval) = delete;
    sockbuf &operator=(const sockbuf &other) = delete;

    ~sockbuf();

    /**
     * Wraps buffer contents (payload + protocol headers) with another header.
     *
     * @param header Header data to put into the buffer.
     *
     * @return Success/failure.
     *
     * @note This might fail because of insufficient space in buffer
     * or in case the header with given type is already present in buffer.
     */
    template <class HeaderType>
    bool wrap(const HeaderType *header);

    /**
     * Places header identifier at the end of header section.
     * This is used during packet parsing.
     *
     * @retval true Header successfully placed.
     * @retval false Insufficient space in buffer or
     * header with given type already exists.
     */
    template <class HeaderType>
    bool push_header();

    /**
     * Returns current number of protocol headers present in buffer.
     */
    size_t num_headers() const;

    /**
     * Returns const pointer to header of given type.
     *
     * @tparam HeaderType Type of header.
     *
     * @retval Pointer to start of header.
     * @retval nullptr if header of given type is not present in buffer.
     */
    template <class HeaderType>
    const HeaderType *get_header() const;

    /**
     * Non-const version of get_header().
     */
    template <class HeaderType>
    HeaderType *get_header();

    /**
     * Appends given payload to the end of payload section.
     *
     * @param payload Pointer to payload to copy from.
     * @param size    Size of payload.
     *
     * @return Success/failure.
     *
     * @note This might fail vecause of insufficient space in buffer.
     */
    bool add_payload(const uint8_t *payload, size_t size);

    /**
     * Returns pointer to payload, excluding protocol headers.
     */
    const uint8_t *payload() const;

    /**
     * Non-const version of payload().
     */
    uint8_t *payload();

    /**
     * Returns size of payload in bytes, excluding size of protocol headers.
     */
    size_t payload_size() const;

    /**
     * Returns read-only pointer to entire buffer.
     */
    const uint8_t *raw() const;

    /**
     * Non-const version of raw().
     * @see raw().
     */
    uint8_t *raw();

    /**
     * Returns size of data in buffer.
     */
    size_t raw_size() const;

    /**
     * Clears payload section of buffer.
     */
    void clear_payload();

    /**
     * Clears contents and eserves headers_len bytes
     * for protocol headers in buffer.
     *
     * @param headers_len Number of bytes to reserve.
     *
     * @retval true If successfully cleared and reserved.
     * @retval false If headers_len is greater than total size of buffer.
     *
     */
    bool clear_all(size_t headers_len);

    /**
     * Writes data using Writer interface.
     *
     * @tparam Writer Writer object type.
     * @param writer Writer object.
     *
     * @return Number of bytes writen.
     */
    template <class Writer>
    int64_t write(Writer &writer);

    /**
     * Reads data using Reader interface.
     *
     * @tparam Reader Reader object type.
     * @param reader Reader object.
     *
     * @return Number of bytes read.
     *
     * @note Completely discards previous contents of buffer,
     * regardless of success of opeartion.
     */
    template <class Reader>
    int64_t read(Reader &reader);

private:
    uint8_t *buffer_;

    size_t size_;
    size_t header_len_;

    using header_id = std::tuple<std::type_index, uint8_t *, size_t>;
    std::list<header_id> headers_;

    uint8_t *head_;
    uint8_t *payload_;
    uint8_t *end_;
};

template <class HeaderType>
bool sockbuf::wrap(const HeaderType *header)
{
    if (header == nullptr) {
        return false;
    }

    for (const auto &i : headers_) {
        if (std::get<0>(i) == std::type_index(typeid(HeaderType))) {
            return false; // header with given type already exists
        }
    }

    if (static_cast<ptrdiff_t>(sizeof(HeaderType)) > head_ - buffer_) {
        return false; // not enough space in buffer
    }

    head_ -= sizeof(HeaderType);
    headers_.emplace_front(std::make_tuple(std::type_index(typeid(HeaderType)),
                head_, sizeof(HeaderType)));
    std::copy(header, header + 1, reinterpret_cast<HeaderType*>(head_));

    return true;
}

template <class HeaderType>
bool sockbuf::push_header()
{
    for (const auto &i : headers_) {
        if (std::get<0>(i) == std::type_index(typeid(HeaderType))) {
            return false; // header with given type already exists
        }
    }

    if (static_cast<ptrdiff_t>(sizeof(HeaderType)) > buffer_ + size_ - payload_) {
        return false; // not enough space in "head room"
    }

    headers_.emplace_back(std::make_tuple(std::type_index(typeid(HeaderType)), payload_,
            sizeof(HeaderType)));

    payload_ += sizeof(HeaderType);
    return true;
}

template <class HeaderType>
const HeaderType *sockbuf::get_header() const
{
    return const_cast<sockbuf *>(this)->get_header<HeaderType>();
}


template <class HeaderType>
HeaderType *sockbuf::get_header()
{
    for (auto &i : headers_) {
        if (std::type_index(typeid(HeaderType)) == std::get<0>(i)) {
            return reinterpret_cast<HeaderType *>(std::get<1>(i));
        }
    }
    return nullptr;
}

template <class Writer>
int64_t sockbuf::write(Writer &writer)
{
    return writer.write(head_, end_ - head_);
}

template <class Reader>
int64_t sockbuf::read(Reader &reader)
{
    clear_all(0);
    int64_t ret = reader.read(buffer_, size_);
    if (ret <= 0) {
        return ret;
    }

    end_ += ret;

    return ret;
}

} // namespace tack

#endif // SOCKBUF_HPP
