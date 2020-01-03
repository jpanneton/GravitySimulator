#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int n)
{
    // Create the requested number of worker threads
    m_workers.reserve(n);
    for (unsigned int i = 0; i < n; ++i)
    {
        m_workers.emplace_back(std::bind(&ThreadPool::threadProc, this));
    }
}

ThreadPool::~ThreadPool()
{
    // Unblock any threads and tell them to stop
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_shutdown = true;
        m_taskCondVar.notify_all();
    }

    // Wait for all threads to stop
    for (auto& worker : m_workers)
    {
        worker.join();
    }
}

void ThreadPool::waitFinished()
{
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_finishedCondVar.wait(lock, [this]() { return m_tasks.empty() && m_busy == 0; });
}

void ThreadPool::threadProc()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_taskCondVar.wait(lock, [this]() { return m_shutdown || !m_tasks.empty(); });
        if (!m_tasks.empty())
        {
            // Got work, set busy
            ++m_busy;

            // Pop task from queue
            const auto task = std::move(m_tasks.front());
            m_tasks.pop();

            // Run async task without holding any lock
            lock.unlock();
            task();

            lock.lock();
            --m_busy;
            m_finishedCondVar.notify_one();
        }
        else if (m_shutdown)
        {
            break;
        }
    }
}