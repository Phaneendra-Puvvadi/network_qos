#ifndef CONFIG_H
#define CONFIG_H

#define INPUT_INTERFACE "veth-router1"
#define OUTPUT_INTERFACE "veth-router2"
#define MAX_QUEUE_SIZE 1500
#define CONGESTION_THRESHOLD 0.6
#define PREDICTION_WINDOW_MS 100
#define EWMA_ALPHA 0.3
#define WEIGHT_HIGH 5
#define WEIGHT_MED 3
#define WEIGHT_LOW 2
#define FLOW_TIMEOUT_SEC 60
#define LOG_FILE "logs/router.log"
#define STATS_FILE "web/stats.json"
#define LINK_CAPACITY_BPS 8000000

#endif
