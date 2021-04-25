CC = g++
CFLAGS  = -g -Wall
TARGET = tsp
RM = rm

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)
