TOOLSDIR = ../../../../tools/cpp
CFLAGS   = -Wall -g -O2 -I$(TOOLSDIR)
LDFLAGS  = -lstdc++ -lm

SRC_C    = main.cpp config.cpp pwm.cpp $(TOOLSDIR)/Utils.cpp $(TOOLSDIR)/SoatcarState.cpp

TARGET   = pwm

$(TARGET):
		$(CC) $(LDFLAGS) $(SRC_C) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(TARGET) ../$(TARGET)

install:
		cp $(TARGET) ..
