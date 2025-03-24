# start_backends.py - Start and auto-restart backend servers
import subprocess
import time
import threading
import os
import signal

# Define backend servers and configurations
backends = [
    {"port": 8081, "name": "B1", "message": "Hello from B1"},
    {"port": 8082, "name": "B2", "message": "Hello from B2"},
    {"port": 8083, "name": "B3", "message": "Hello from B3"},
]

# Store backend processes
processes = {}

# Backend server template
backend_template = """
from http.server import BaseHTTPRequestHandler, HTTPServer

class BackendHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/health':
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(b'OK')
        else:
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.send_header('Content-Length', '{}')
            self.end_headers()
            self.wfile.write(b'{}')

if __name__ == "__main__":
    server_address = ('127.0.0.1', {})
    httpd = HTTPServer(server_address, BackendHandler)
    print("Backend {} running on port {}...")
    httpd.serve_forever()
"""

# Start a backend and return the process
def start_backend(backend):
    backend_code = backend_template.format(
        len(backend["message"]),
        backend["message"],
        backend["port"],
        backend["name"],
        backend["port"],
    )
    process = subprocess.Popen(["python3", "-c", backend_code])
    print(f"Started Backend {backend['name']} on port {backend['port']} (PID: {process.pid})")
    return process

# Monitor and auto-restart crashed backends
def monitor_backends():
    while True:
        for backend in backends:
            process = processes[backend["port"]]
            if process.poll() is not None:  # Process has terminated
                print(f"⚠️ Backend {backend['name']} on port {backend['port']} crashed. Restarting...")
                processes[backend["port"]] = start_backend(backend)
        time.sleep(5)  # Check every 5 seconds

# Start all backends and store processes
def start_all_backends():
    for backend in backends:
        processes[backend["port"]] = start_backend(backend)

    # Start monitoring thread for auto-restart
    monitor_thread = threading.Thread(target=monitor_backends, daemon=True)
    monitor_thread.start()


# Stop all running backend processes
def stop_all_backends():
    for port, process in processes.items():
        if process.poll() is None:
            print(f"Stopping backend on port {port} (PID: {process.pid})...")
            os.kill(process.pid, signal.SIGTERM)

    print("✅ All backends have been stopped!")


if __name__ == "__main__":
    try:
        start_all_backends()
        print("✅ All backends are running. Monitoring for crashes...")
        while True:
            time.sleep(1)  # Keep the main thread alive
    except KeyboardInterrupt:
        print("\n⏹️  Stopping all backends...")
        stop_all_backends()
