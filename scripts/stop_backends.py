# stop_backends.py - Stop all running backend servers
import os
import signal
import subprocess

# Define backend ports to stop
backend_ports = [8081, 8082, 8083]

# Kill processes listening on the backend ports
def stop_backends():
    for port in backend_ports:
        try:
            # Use lsof to get the process ID (PID) of the port
            result = subprocess.run(
                ["lsof", "-ti", f":{port}"], capture_output=True, text=True
            )
            pids = result.stdout.strip().split("\n")

            if pids and pids[0] != "":
                for pid in pids:
                    print(f"Stopping backend running on port {port} (PID: {pid})...")
                    os.kill(int(pid), signal.SIGTERM)
            else:
                print(f"No backend running on port {port}.")
        except Exception as e:
            print(f"Error stopping backend on port {port}: {e}")


if __name__ == "__main__":
    stop_backends()
    print("✅ All backends have been stopped!")
