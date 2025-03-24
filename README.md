# 🦀 CrabbyLB — A High-Performance HTTP Server & Load Balancer in C++

CrabbyLB is a high-performance HTTP server and load balancer implemented in **C++**. It leverages raw socket programming (`netinet`) to handle HTTP requests and distributes them across multiple backend servers using a round-robin load balancing algorithm. 

Designed for performance and concurrency, CrabbyLB efficiently handles high loads, making it an excellent tool for learning about HTTP servers, load balancing, and low-level networking concepts.

---

## 🚀 **Features**

✅ Basic HTTP Server to handle incoming client requests.  
✅ Multi-threaded Server for handling requests concurrently.  
✅ Thread Pool Server for efficient request handling.  
✅ Round Robin Load Balancing for backend server distribution.  
✅ Backend Health Monitoring with automatic failover and recovery.  
✅ High Concurrency with per-request threading.  
✅ Graceful Handling of Backend Failures and Recovery.  

---

## 🛠️ **Usage**

To run CrabbyLB, first make the script executable:
```sh
chmod +x run_crabbyLB.sh
```

Then, use the script to run CrabbyLB:
```sh
./run_crabbyLB.sh -m <mode> [-b <backend_addresses>]
```

### Modes:
- `basic`: Run the basic single-threaded HTTP server.
- `multi_thread`: Run the multi-threaded server (one thread per request).
- `thread_pool`: Run the server with a thread pool.
- `load_balancer`: Run the load balancer with health checks.

### Examples:
- Run Basic Mode:
    ```sh
    ./run_crabbyLB.sh -m basic
    ```

- Run Load Balancer Mode with backend addresses:
    ```sh
    ./run_crabbyLB.sh -m load_balancer -b 127.0.0.1:8081,127.0.0.1:8082
    ```

---

## 🔄 **Stress Test**

To perform a stress test on CrabbyLB, you can use the `wrk` tool:
```sh
wrk -t4 -c100 -d5s http://localhost:8080/
```

This command runs a stress test with 4 threads, 100 connections, for a duration of 5 seconds against the server running on `localhost` at port `8080`.

---
