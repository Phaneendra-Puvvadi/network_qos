#!/bin/bash

echo "=== Setting up virtual network environment ==="

# Cleanup any existing namespaces
sudo ip netns del client 2>/dev/null
sudo ip netns del router 2>/dev/null
sudo ip netns del server 2>/dev/null

# Create namespaces
sudo ip netns add client
sudo ip netns add router
sudo ip netns add server

# Create veth pairs
sudo ip link add veth-client type veth peer name veth-router1
sudo ip link add veth-router2 type veth peer name veth-server

# Move to namespaces
sudo ip link set veth-client netns client
sudo ip link set veth-router1 netns router
sudo ip link set veth-router2 netns router
sudo ip link set veth-server netns server

# Configure IPs
sudo ip netns exec client ip addr add 10.0.1.2/24 dev veth-client
sudo ip netns exec router ip addr add 10.0.1.1/24 dev veth-router1
sudo ip netns exec router ip addr add 10.0.2.1/24 dev veth-router2
sudo ip netns exec server ip addr add 10.0.2.2/24 dev veth-server

# Bring up interfaces
sudo ip netns exec client ip link set veth-client up
sudo ip netns exec client ip link set lo up
sudo ip netns exec router ip link set veth-router1 up
sudo ip netns exec router ip link set veth-router2 up
sudo ip netns exec router ip link set lo up
sudo ip netns exec server ip link set veth-server up
sudo ip netns exec server ip link set lo up

# Add routes
sudo ip netns exec client ip route add default via 10.0.1.1
sudo ip netns exec server ip route add default via 10.0.2.1
sudo ip netns exec router sysctl -w net.ipv4.ip_forward=1 > /dev/null

echo "=== Network setup complete ==="
echo "Client: 10.0.1.2"
echo "Router: 10.0.1.1 / 10.0.2.1"
echo "Server: 10.0.2.2"
