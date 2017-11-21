CFLAGS = -std=c99 -Wpedantic -Wall -Wextra -Wconversion -Wshadow -O2
TARGET = uenv
OBJECTS = uenv.o

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS)
