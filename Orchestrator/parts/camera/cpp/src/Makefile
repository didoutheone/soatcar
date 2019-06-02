TOOLSDIR = ../../../../tools/cpp
CFLAGS   = -Wall -g -O2 -I$(TOOLSDIR)
LDFLAGS  = -lstdc++ -lraspicam

SRC_C    = main.cpp config.cpp camera.cpp $(TOOLSDIR)/Utils.cpp $(TOOLSDIR)/SoatcarState.cpp

TARGET   = camera

$(TARGET):
		$(CC) $(LDFLAGS) $(SRC_C) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(TARGET) ../$(TARGET)

install:
		cp $(TARGET) ..
