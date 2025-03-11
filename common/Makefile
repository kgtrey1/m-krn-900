CC = gcc
CFLAGS = -Wall -g -I$(PWD)/include

LIBRARY = libotp.a

SRCS = src/method_list.c src/device.c

OBJS = $(SRCS:.c=.o)

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	ar rcs $(LIBRARY) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIBRARY)

.PHONY: all clean
