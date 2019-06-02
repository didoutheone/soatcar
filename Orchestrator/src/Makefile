TOOLSDIR= ../tools/cpp
CFLAGS  = -Wall -g -O2 -I../tools/cpp
LDFLAGS = -lstdc++

SRC_C =		main.cpp Config.cpp Orchestrator.cpp PartManager.cpp $(TOOLSDIR)/Utils.cpp $(TOOLSDIR)/SoatcarState.cpp

TARGET =	orchestrator

$(TARGET):
		$(CC) $(LDFLAGS) $(SRC_C) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(TARGET) ../$(TARGET)

install:
		cp $(TARGET) ..
