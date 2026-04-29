# Network QoS Router System

## Overview

This project simulates a router that prioritizes network traffic (QoS) using multiple queues and congestion prediction.

---

## Requirements

* Linux (Ubuntu/Kali)
* gcc, make
* Python3 + Flask
* iperf3, tcpdump

---

## Installation

```bash
sudo apt-get update
sudo apt-get install -y build-essential libpcap-dev iperf3 net-tools iproute2 tcpdump wireshark python3 python3-pip libjson-c-dev
pip3 install flask reportlab --break-system-packages
```

---

## Setup Network

```bash
cd network_qos
sudo ./setup_network.sh
```

---

## Build

```bash
make clean
make
```

---

## Run System

### Terminal 1 (Router)

```bash
sudo ip netns exec router ./network_qos
```

### Terminal 2 (Dashboard)

```bash
cd web
python3 server.py
```

Open: http://localhost:5000

---

### LIVE DEPLOY LINK
https://network-qos.vercel.app

## Quick Demo (Recommended)

Instead of manual setup:

```bash
sudo ./start_demo_traffic.sh
```

---

## Cleanup

```bash
sudo ./cleanup_network.sh
```


