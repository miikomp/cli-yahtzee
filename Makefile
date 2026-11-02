CC ?= cc
CPPFLAGS ?= -Iinc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic

TARGET := yahtzee
SOURCES := $(wildcard src/*.c)

$(TARGET): $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
