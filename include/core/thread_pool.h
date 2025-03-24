#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>


class ThreadPool {
public:
    // Constructor to create a thread pool with a specified number of threads
    ThreadPool(size_t num_threads);

    // Destructor to clean up and stop all threads
    ~ThreadPool();

    void enqueue_task(std::function<void()> task);

private:
    // Worker threads that will execute tasks
    std::vector<std::thread> workers;

    // Task queue to hold incoming tasks
    std::queue<std::function<void()>> tasks_queue;

    // Mutex and condition variable to synchronize access to the task queue
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

    // Function that worker threads will execute
    void worker_thread();
};

#endif