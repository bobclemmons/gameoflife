CC = clang
CFLAGS  = -std=c99 -g -Wall

TARGET = gol

all: $(TARGET)
$(TARGET): $(TARGET).c ; $(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)
