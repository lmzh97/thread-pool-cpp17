// Based on progschj/ThreadPool (zlib License, (c) 2012 Jakob Progsch, Václav Zeman).
// Modified for C++17 by lmzh97, 2026. See COPYING.

#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
   public:
    explicit ThreadPool(size_t);
    ~ThreadPool();

    template <class F, class... Args>
    auto Enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

   private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::condition_variable cv_;
    std::mutex mtx_;
    bool stop_{false};
};

inline ThreadPool::ThreadPool(size_t threads_num) {
    for (size_t i = 0; i < threads_num; ++i) {
        workers_.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lck(this->mtx_);
                    this->cv_.wait(lck, [this] { return this->stop_ || !this->tasks_.empty(); });
                    if (this->stop_ && this->tasks_.empty()) return;
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }
                task();
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lck(mtx_);
        stop_ = true;
    }

    cv_.notify_all();
    for (auto& worker : workers_)
        if (worker.joinable()) worker.join();
}

template <class F, class... Args>
auto ThreadPool::Enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        [f = std::forward<F>(f), args = std::tuple(std::forward<Args>(args)...)]() mutable {
            return std::apply([&f](auto&&... a) { return std::invoke(f, std::move(a)...); }, args);
        });
    {
        std::unique_lock<std::mutex> lck(mtx_);
        if (stop_) return std::future<return_type>{};
        tasks_.emplace([task] { (*task)(); });
    }
    cv_.notify_one();
    return task->get_future();
}

#endif  // THREAD_POOL_H_
