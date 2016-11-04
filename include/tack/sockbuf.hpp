#ifndef SOCKBUF_HPP
#define SOCKBUF_HPP

#include <cstdint>
#include <cstddef>

namespace tack {

/**
 * Socket buffer implementation.
 *
 * @tparam E Enumeration type used to identify protocol headers present in the buffer
 * @tparam N Total size of buffer.
 */
template <class E, size_t N>
class sockbuf
{
public:
    /**
     * @param mtu Used to deal with fragmentation and read size of Ethernet frames.
     */
    sockbuf(size_t mtu);

    // TODO: consider implementing these
    sockbuf(const sockbuf &other) = delete;
    sockbuf(sockbuf &&rval) = delete;
    sockbuf &operator=(const sockbuf &other) = delete;

    ~sockbuf() = default;

    /**
     * Adds protocol header to the buffer.
     *
     * @param header Header data to put into the buffer.
     * @param size   Size of header data.
     * @param type   Unique type identifier of header.
     *
     * @return Success/failure.
     *
     * @note This might fail because of insufficient space in buffer.
     */
    bool wrap(const uint8_t *header, size_t size, E type);

    /**
     * Returns current number of protocol headers present in buffer.
     */
    size_t num_headers() const;

    /**
     * Returns const pointer to header of given type.
     *
     * @param type Type of header.
     *
     * @retval Pointer to start of header.
     * @retval 0 if header of given type is not present in buffer.
     */
    const uint8_t *get_header(E type) const;

    /**
     * Non-const version of get_header().
     * @see get_header().
     */
    uint8_t *get_header(E type);

    /**
     * Returns size of header of given type.
     *
     * @param type Type of header.
     *
     * @retval Size of header.
     * @retval 0 if header of given type is not present in buffer.
     */
    size_t get_header_size(E type) const;

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
     * Returns size of entire buffer.
     *
     * @note Equals to N template parameter.
     */
    constexpr size_t raw_size() const;

    /**
     * Clears payload section of buffer.
     */
    void clear_payload();

    /**
     * Clears entire buffer.
     */
    void clear_all();

    /**
     * Writes (Ethernet) frame to given decriptor.
     *
     * @param fd Descriptor to write to.
     *
     * @return Number of bytes writen.
     *
     * @note Payload is fragmented into MTU-sized parts.
     */
    int64_t write(int fd);

    /**
     * Reads frame from given descriptor.
     *
     * @param fd Descriptor to read from.
     *
     * @return Number of bytes read.
     *
     * @note Completely discards previous contents of buffer.
     */
    int64_t read(int fd);
};

} // namespace tack

#endif // SOCKBUF_HPP
