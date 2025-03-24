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

## 📦 **Installation**

To build and install CrabbyLB, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/crabbyLB.git
    cd crabbyLB
    ```

2. Create a build directory and navigate into it:
    ```sh
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

4. Build the project:
    ```sh
    make
    ```

5. The `crabbyLB` executable will be located in the `bin` directory.

---

## 🛠️ **Usage**

To run CrabbyLB, use the following command:
```sh
./bin/crabbyLB
```

You can configure the server and load balancer settings by editing the configuration file located at `config/crabbyLB.conf`.

