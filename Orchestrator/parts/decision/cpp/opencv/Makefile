TOOLSDIR = ../../../../tools/cpp
CFLAGS   = -Wall -g -O2 -I$(TOOLSDIR) -I/usr/local/include/opencv4
LDFLAGS  = -lstdc++ -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

#/usr/local/lib/libopencv_dnn.so.4.0.1 /usr/local/lib/libopencv_gapi.so.4.0.1 /usr/local/lib/libopencv_ml.so.4.0.1 /usr/local/lib/libopencv_objdetect.so.4.0.1 /usr/local/lib/libopencv_photo.so.4.0.1 /usr/local/lib/libopencv_stitching.so.4.0.1 /usr/local/lib/libopencv_video.so.4.0.1 /usr/local/lib/libopencv_calib3d.so.4.0.1 /usr/local/lib/libopencv_features2d.so.4.0.1 /usr/local/lib/libopencv_flann.so.4.0.1 /usr/local/lib/libopencv_highgui.so.4.0.1 /usr/local/lib/libopencv_videoio.so.4.0.1 /usr/local/lib/libopencv_imgcodecs.so.4.0.1 /usr/local/lib/libopencv_imgproc.so.4.0.1 /usr/local/lib/libopencv_core.so.4.0.1

SRC_C    = main.cpp config.cpp autopilot.cpp $(TOOLSDIR)/Utils.cpp $(TOOLSDIR)/SoatcarState.cpp

TARGET   = autopilot

$(TARGET):
		$(CC) $(LDFLAGS) $(SRC_C) $(CFLAGS) -o $(TARGET)

all:	$(TARGET)

clean:
		rm -f $(TARGET) ../$(TARGET)

install:
		cp $(TARGET) ..
