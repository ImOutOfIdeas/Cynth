CC     = gcc
CFLAGS = -std=c11 -g -Wall -Wextra -I. -Irack -Iui
LIBS   = -lpthread -ldl -lm -lncursesw

# macOS: LIBS = -lpthread -lm -lncursesw -framework CoreAudio -framework AudioToolbox -framework AudioUnit

TARGET = rack_synth

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(TARGET)
