TOOLSDIR = ../../../../tools/cpp
CFLAGS   = -Wall -g -O2 -I$(TOOLSDIR)
LDFLAGS  = -lstdc++

SRC_C    = main.cpp config.cpp joystick.cpp $(TOOLSDIR)/Utils.cpp $(TOOLSDIR)/SoatcarState.cpp

TARGET   = joystick

$(TARGET):
		$(CC) $(LDFLAGS) $(SRC_C) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(TARGET) ../$(TARGET)

install:
		cp $(TARGET) ..
