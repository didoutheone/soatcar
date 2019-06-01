TOOLSDIR = ../../../../tools/cpp
CFLAGS   = -Wall -g -O2 -I$(TOOLSDIR)
LDFLAGS  = -lstdc++

SRC_C    = main.cpp config.cpp pwm.cpp $(TOOLSDIR)/Utils.cpp $(TOOLSDIR)/SoatcarState.cpp

LIBS     =

TARGET   = pwm

$(TARGET):
		$(CC) $(LIBS) $(LDFLAGS) $(SRC_C) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(OBJS) $(TARGET) ../$(TARGET)

install:
		cp $(TARGET) ..
