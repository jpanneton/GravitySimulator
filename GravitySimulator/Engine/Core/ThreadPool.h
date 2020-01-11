#pragma once

#include <mutex>
#include <queue>
#include <thread>

class ThreadPool
{
public:
    ThreadPool(unsigned int n = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<class F>
    void enqueue(F&& f)
    {
        // Enqueue job and unblock a thread
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_tasks.emplace(std::forward<F>(f));
        m_taskCondVar.notify_one();
    }

    void waitFinished();

    // Utility function to get an index range on a dataset according to a certain batch size and index
    template<typename IndexType>
    static constexpr auto getRangeFromBatch(size_t totalSize, size_t batchCount, IndexType batchIndex)
    {
        auto batchSize = totalSize / batchCount;
        if (batchIndex < totalSize % batchCount)
            ++batchSize;
        return std::make_pair(
            static_cast<IndexType>(batchIndex * batchSize),
            static_cast<IndexType>((batchIndex + 1) * batchSize)
        );
    }

private:
    void threadProc();

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_taskCondVar;
    std::condition_variable m_finishedCondVar;
    unsigned int m_busy = {};
    bool m_shutdown = false;
};