#include "core/thread_pool.h"
#include <iostream>

// Constructor to create a thread pool with a specified number of threads
ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    for(size_t i = 0; i < num_threads; ++i){
        // Create worker threads and bind them to the worker_thread function
        workers.emplace_back([this]{
            this->worker_thread();
        });
    }
}

// Destructor to clean up and stop all threads
ThreadPool::~ThreadPool(){
    {
        // Lock the mutex to ensure no tasks are added while stopping
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    // Notify all threads that it's time to stop
    condition.notify_all();

    // Join all threads to wait for them to finish
    for(std::thread &worker: workers){
        worker.join();
    }
}

// Add a task to the task queue
void ThreadPool::enqueue_task(std::function<void()> task){
    {
        // Lock the mutex to ensure no other thread modifies the task queue
        std::unique_lock<std::mutex> lock(queue_mutex);

        // Add the task to the queue
        tasks_queue.push(std::move(task));
    }

    // Notify one thread that a new task is available
    condition.notify_one();
}

// Worker thread that processes tasks from the task queue
void ThreadPool::worker_thread() {
    while(true) {
        std::function<void()> task;

        {
            // Lock the mutex to access the task queue
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Wait until a task is available or shutdown signal is received
            condition.wait(lock, [this]{
                return !tasks_queue.empty() || stop;
            });

            // If the thread was woken up to stop, exit the loop
            if(stop && tasks_queue.empty()){
                return;
            }

            // Get the next task from the queue
            task = std::move(tasks_queue.front());
            tasks_queue.pop();
        }

        // Execute the task
        task();
    }
}
