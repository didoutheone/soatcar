CFLAGS  = -Wall -g -O2
LDFLAGS = -lstdc++

SRC_C =		main.cpp Config.cpp Orchestrator.cpp PartManager.cpp Utils.cpp ../tools/cpp/SoatcarState.cpp

LIBS =

TARGET =	orchestrator

$(TARGET):
		$(CC) $(LIBS) $(LDFLAGS) $(SRC_C) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(OBJS) $(TARGET)

install:
		cp $(TARGET) ..
