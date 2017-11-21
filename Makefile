CFLAGS = -std=c99 -Wpedantic -Wall -Wextra -Wconversion -Wshadow -O2
TARGET = uenv
OBJECTS = uenv.o

.PHONY: test clean

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

test: $(TARGET)
	@PATH="$${PWD}:$${PATH}" test/run

clean:
	rm -f $(TARGET) $(OBJECTS)
