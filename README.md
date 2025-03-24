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

To run CrabbyLB, use the provided `run_crabbyLB.sh` script:
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

# 📊 CrabbyLB Current Performance Analysis

This report summarizes the current performance of CrabbyLB across different modes.



## 🚀 **1. Basic Mode**
- **Requests/sec:** ~1486.75  
- **Avg Latency:** ~149.99 µs  
- **Transfer/sec:** 105.99 KB  
- **Errors:** High read errors  
✅ Basic Mode processes requests sequentially. It's simple but lacks concurrency, making it slower under high load.


## 🧵 **2. Multi-thread Mode**
- **Requests/sec:** ~16.64 (extremely low)  
- **Avg Latency:** ~792.90 µs  
- **Transfer/sec:** 1.19 KB  
- **Errors:** High read errors  
⚠️ Multi-thread Mode should perform better but seems to be bottlenecked, likely due to thread contention or improper socket handling. Needs fixing.



## 🧠 **3. Thread Pool Mode**
- **Requests/sec:** ~1495.79  
- **Avg Latency:** ~146.60 µs  
- **Transfer/sec:** 106.63 KB  
- **Errors:** High read errors  
✅ Thread Pool Mode handles concurrent requests better by reusing threads. Performance is close to Basic Mode but needs better error handling to reduce read errors.



## 🌐 **4. Load Balancer Mode**
- **Requests/sec:** ~2739.11 (very high throughput)  
- **Avg Latency:** ~524.96 µs  
- **Transfer/sec:** 167.80 KB  
- **Errors:** ⚠️ **12744 Non-2xx responses**  
🔥 Load Balancer Mode efficiently distributes requests to multiple backends, resulting in the highest throughput. However, many responses are invalid (non-2xx), suggesting backend issues or misconfigured routes.




