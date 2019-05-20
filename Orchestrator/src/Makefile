CFLAGS  = -g -O2
LDFLAGS = -lstdc++

SRC_C =		Orchestrator.cpp Config.cpp ../tools/cpp/SoatcarState.cpp

LIBS =

TARGET =	orchestrator

$(TARGET):
		$(CC) $(LIBS) $(LDFLAGS) $(SRC_C) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(OBJS) $(TARGET)

install:
		cp $(TARGET) ..
