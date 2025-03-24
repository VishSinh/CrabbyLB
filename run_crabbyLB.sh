#!/bin/bash

# 🚀 CrabbyLB Runner Script
# Allows quick switching between server modes with auto-rebuild and auto-start of backends.

# Default values
PORT=8080
MODE=""
BACKENDS=()

# Paths
BUILD_DIR="build"
BIN="./bin/crabbyLB"
START_BACKENDS="scripts/start_backends.py"
STOP_BACKENDS="scripts/stop_backends.py"

# Print usage
usage() {
    echo "Usage: $0 -m <mode> [-b <backend_addresses>]"
    echo "Modes: basic, multi_thread, thread_pool, load_balancer"
    echo "Examples:"
    echo "  Run Basic Mode:          $0 -m basic"
    echo "  Run Load Balancer Mode:  $0 -m load_balancer -b 127.0.0.1:8081,127.0.0.1:8082"
    exit 1
}

# 🚧 Always rebuild CrabbyLB from scratch
rebuild_crabbyLB() {
    echo "🔨 Rebuilding CrabbyLB from scratch..."
    rm -rf $BUILD_DIR
    mkdir -p $BUILD_DIR
    cmake -S . -B $BUILD_DIR
    make -C $BUILD_DIR
}

# Rebuild CrabbyLB every time
rebuild_crabbyLB

# 🎉 Auto-start Python backend servers if using load_balancer mode
start_backends() {
    echo "🚀 Starting backend servers..."
    if [ -f "$START_BACKENDS" ]; then
        # Run in the background and redirect output to logs
        python3 $START_BACKENDS > /dev/null 2>&1 &  # ✅ Run in background
        BACKEND_PID=$!  # Capture the process ID of start_backends.py
        echo "✅ Backends started successfully (PID: $BACKEND_PID)"
    else
        echo "❗️ start_backends.py not found in scripts/ directory!"
        exit 1
    fi
}

# 🛑 Stop all backend servers gracefully
stop_backends() {
    echo "🛑 Stopping all backend servers..."
    if [ -f "$STOP_BACKENDS" ]; then
        python3 $STOP_BACKENDS
    else
        echo "❗️ stop_backends.py not found in scripts/ directory!"
    fi
}

# Trap SIGINT and stop backends when CrabbyLB is stopped
trap stop_backends SIGINT

# Parse command-line arguments
while getopts ":m:b:" opt; do
    case ${opt} in
        m )  # Mode
            MODE=$OPTARG
            ;;
        b )  # Backend addresses (comma-separated)
            IFS=',' read -r -a BACKENDS <<< "$OPTARG"
            ;;
        \? )  # Invalid option
            echo "❌ Invalid option: -$OPTARG"
            usage
            ;;
        : )  # Missing argument for an option
            echo "❌ Option -$OPTARG requires an argument."
            usage
            ;;
    esac
done

# Validate mode
if [ -z "$MODE" ]; then
    echo "❌ Mode is required. Use -m <mode>."
    usage
fi

# Validate backend addresses if using load_balancer mode
if [ "$MODE" == "load_balancer" ] && [ "${#BACKENDS[@]}" -eq 0 ]; then
    echo "❌ Load balancer mode requires at least one backend address. Use -b <addresses>."
    usage
fi

# Build the command
CMD="$BIN $MODE"
if [ "$MODE" == "load_balancer" ]; then
    CMD="$CMD ${BACKENDS[@]}"
    start_backends  # ✅ Start backends before running CrabbyLB
fi

# 🚀 Run CrabbyLB
echo "🔄 Running: $CMD"
$CMD
