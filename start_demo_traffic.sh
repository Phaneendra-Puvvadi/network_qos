#!/bin/bash

echo "=== Starting Demo Traffic Generation ==="
echo ""
echo "This will start 6 iperf3 servers and 6 clients"
echo "Press Ctrl+C in THIS terminal to stop all traffic"
echo ""

# Start servers in background
echo "Starting servers..."
sudo ip netns exec server iperf3 -s -p 5000 > /dev/null 2>&1 &
SERVER1=$!
sudo ip netns exec server iperf3 -s -p 20000 > /dev/null 2>&1 &
SERVER2=$!
sudo ip netns exec server iperf3 -s -p 443 > /dev/null 2>&1 &
SERVER3=$!
sudo ip netns exec server iperf3 -s -p 80 > /dev/null 2>&1 &
SERVER4=$!
sudo ip netns exec server iperf3 -s -p 21 > /dev/null 2>&1 &
SERVER5=$!
sudo ip netns exec server iperf3 -s -p 5201 > /dev/null 2>&1 &
SERVER6=$!

sleep 2

echo "Servers started."
echo ""
echo "Starting traffic generators..."
echo "  - High Priority (Real-time): Ports 5000, 20000"
echo "  - Medium Priority (Interactive): Ports 443, 80"
echo "  - Low Priority (Bulk): Ports 21, 5201"
echo ""

# Start clients in background
sudo ip netns exec client iperf3 -c 10.0.2.2 -u -b 3M -p 5000 -t 300 -l 200 > /dev/null 2>&1 &
CLIENT1=$!

sudo ip netns exec client iperf3 -c 10.0.2.2 -u -b 3M -p 20000 -t 300 -l 300 > /dev/null 2>&1 &
CLIENT2=$!

sudo ip netns exec client iperf3 -c 10.0.2.2 -u -b 2M -p 443 -t 300 -l 800 > /dev/null 2>&1 &
CLIENT3=$!

sudo ip netns exec client iperf3 -c 10.0.2.2 -u -b 2M -p 80 -t 300 -l 600 > /dev/null 2>&1 &
CLIENT4=$!

sudo ip netns exec client iperf3 -c 10.0.2.2 -b 8M -p 21 -t 300 > /dev/null 2>&1 &
CLIENT5=$!

sudo ip netns exec client iperf3 -c 10.0.2.2 -b 8M -p 5201 -t 300 -l 1450 > /dev/null 2>&1 &
CLIENT6=$!

echo "Traffic generation started!"
echo ""
echo "Open http://localhost:5000 to see the dashboard"
echo ""
echo "You should see:"
echo "  ✓ All 3 queues filling up (Red, Yellow, Green)"
echo "  ✓ Congestion score rising above 0.6"
echo "  ✓ Status changing to CONGESTED (red)"
echo ""
echo "Traffic will run for 5 minutes (300 seconds)"
echo "Press Ctrl+C to stop early"
echo ""

# Trap Ctrl+C to kill all processes
trap "echo ''; echo 'Stopping all traffic...'; kill $CLIENT1 $CLIENT2 $CLIENT3 $CLIENT4 $CLIENT5 $CLIENT6 $SERVER1 $SERVER2 $SERVER3 $SERVER4 $SERVER5 $SERVER6 2>/dev/null; echo 'All traffic stopped.'; exit" INT TERM

# Wait for clients to finish
wait $CLIENT1 $CLIENT2 $CLIENT3 $CLIENT4 $CLIENT5 $CLIENT6

echo ""
echo "Traffic generation completed."
echo "Stopping servers..."
kill $SERVER1 $SERVER2 $SERVER3 $SERVER4 $SERVER5 $SERVER6 2>/dev/null

echo "All done. Queues should drain to 0 in dashboard."
