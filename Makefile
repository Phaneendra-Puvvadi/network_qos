CC = gcc
CFLAGS = -Wall -O2 -I./include
LDFLAGS = -lpcap -lpthread

SRCS = src/main.c src/capture.c src/classify.c src/predict.c src/queue.c src/forward.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = network_qos

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f logs/*.log
	rm -f web/stats.json

.PHONY: all clean
