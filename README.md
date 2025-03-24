# 🦀 CrabbyLB — A High-Performance HTTP Server & Load Balancer in C++

CrabbyLB is a custom, bare-metal HTTP server and load balancer implemented in **C++**. It uses raw socket programming (`netinet`) to handle HTTP requests and distributes them across multiple backend servers using a round-robin load balancing algorithm. 

With a focus on performance and concurrency, CrabbyLB is capable of handling high loads efficiently, making it a great starting point for learning about HTTP servers, load balancing, and low-level networking concepts.

---

## 🚀 **Features**

✅ Basic HTTP Server to handle incoming client requests.  
✅ Round Robin Load Balancing for backend server distribution.  
✅ Backend Health Monitoring (Failover & Marking Down Unhealthy Backends).  
✅ High Concurrency with Per-Request Threading.  
✅ Graceful Handling of Backend Failures and Recovery.  
✅ Extensible Architecture to add more algorithms (e.g., Least Connections, Weighted Round Robin).  


---