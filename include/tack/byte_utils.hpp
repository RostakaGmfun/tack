#ifndef BYTE_UTILS_HPP
#define BYTE_UTILS_HPP

#include <type_traits>
#include <arpa/inet.h>

namespace tack {

template <typename E>
inline typename std::enable_if<(sizeof(E) == 2), E>::type ntohs(const E &e)
{
    return static_cast<E>(::ntohs(static_cast<uint16_t>(e)));
}

template <typename E>
inline typename std::enable_if<(sizeof(E) == 4), E>::type ntohl(const E &e)
{
    return static_cast<E>(::ntohl(static_cast<uint16_t>(e)));
}

template <typename E>
inline typename std::enable_if<(sizeof(E) == 2), E>::type htons(const E &e)
{
    return static_cast<E>(::htons(static_cast<uint16_t>(e)));
}

template <typename E>
inline typename std::enable_if<(sizeof(E) == 4), E>::type htonl(const E &e)
{
    return static_cast<E>(::htonl(static_cast<uint16_t>(e)));
}

}

#endif // BYTE_UTILS_HPP
