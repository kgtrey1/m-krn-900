CC = gcc
CFLAGS = -Wall -g -I$(PWD)/include -I../common/include

LDFLAGS = -L../common -lotp

TARGET = otp_config

SRCS = src/main.c src/help.c src/dict.c src/time.c src/utils.c src/exec.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean