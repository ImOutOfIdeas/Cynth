CC     = gcc
CFLAGS = -std=c11 -g -Wall -Wextra -I. -Imodules
LIBS   = -lpthread -ldl -lm -lncursesw

# macOS: LIBS = -lpthread -lm -lncursesw -framework CoreAudio -framework AudioToolbox -framework AudioUnit

TARGET = rack

all: $(TARGET)

# unity build: main.c includes everything
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(TARGET)
