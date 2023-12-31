CC = gcc
CFLAGS = -pthread

SRCS = main.c
OBJS = $(SRCS:.c=.o)
TARGET = santa

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
