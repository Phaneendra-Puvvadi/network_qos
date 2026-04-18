#!/bin/bash

echo "=== Network QoS Demo ==="
echo ""
echo "This will:"
echo "1. Setup network namespaces"
echo "2. Compile the router program"
echo "3. Start the router"
echo "4. Start the web dashboard"
echo "5. Start traffic generators"
echo ""
read -p "Press Enter to continue..."

# Setup network
./setup_network.sh

# Compile
echo ""
echo "Compiling router..."
make clean
make

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# Start router in background
echo ""
echo "Starting router..."
sudo ip netns exec router ./network_qos &
ROUTER_PID=$!
sleep 2

# Start web server
echo ""
echo "Starting web dashboard..."
cd web
python3 server.py &
WEB_PID=$!
cd ..
sleep 2

# Start server
echo ""
echo "Starting iperf3 server..."
sudo ip netns exec server iperf3 -s &
SERVER_PID=$!
sleep 1

echo ""
echo "=== System Running ==="
echo "Router PID: $ROUTER_PID"
echo "Web Dashboard: http://localhost:5000"
echo ""
echo "Open Wireshark: sudo ip netns exec router wireshark &"
echo ""
echo "Generate traffic with:"
echo "  sudo ip netns exec client iperf3 -c 10.0.2.2 -u -b 2M -t 60"
echo ""
echo "Press Ctrl+C to stop everything"

trap "sudo kill $ROUTER_PID $WEB_PID $SERVER_PID 2>/dev/null; ./cleanup_network.sh; exit" INT TERM

wait
