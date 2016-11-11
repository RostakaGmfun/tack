#ifndef NDEV_POOL_HPP
#define NDEV_POOL_HPP

#include <cstddef>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <queue>

#include "tack/network_device.hpp"
#include "tack/sockbuf.hpp"

namespace tack
{

class arp_cache;
using arp_cache_ptr = std::shared_ptr<arp_cache>;

class ndev_pool
{
public:
    ndev_pool(const network_device &ndev);
    ~ndev_pool();

    bool is_stop() const;

    arp_cache_ptr arp_cache() const
    {
        return arp_cache_;
    }

    using write_callback_t = std::function<void(int64_t)>;
    using write_op_t = std::pair<tack::sockbuf, write_callback_t>;

    /**
     * Asynchronous write operation
     *
     * @param skb       Socket buffer to write.
     * @param callback  Called after the write succeeds or fails.
     *
     * @note @c skb is moved out so data becomes invalid after this call.
     * @note @c callback might be called from another thread.
     */
    void write(tack::sockbuf &&skb, write_callback_t callback);

    /**
     * Thread safe method to pick a task from write queue.
     * Intended to be called by ndev_worker.
     *
     * @param skb Reference to socket buffer to which the
     *      buffer to be written should be moved.
     * @param cb Callback to call after write operation.
     *
     * @return Operation validness.
     */
    bool pick_task(sockbuf &skb, write_callback_t &cb);

private:
    std::vector<std::thread> threads_;
    bool stop_;
    arp_cache_ptr arp_cache_;
    std::queue<write_op_t> write_queue_;
    std::mutex write_queue_mutex_;
};

} // namespace tack

#endif // NDEV_POOL_HPP
