CC = gcc
CFLAGS = -Wall -Wextra -g -msse -mavx -msse2 -msse3 -msse4

# Default optimization flag
OPTIMIZATION = -O1

SOURCES = default_image_performance.c \
          performance_test.c \
          simd_image_performance.c \
          optimized_sisd_performance_image.c

OBJECTS = $(addsuffix .o,$(basename $(SOURCES)))
TARGET = performance_test
.PHONY: all clean

all: $(TARGET)

performance_test: $(OBJECTS)
	$(CC) $(CFLAGS) $(OPTIMIZATION) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(OPTIMIZATION) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
