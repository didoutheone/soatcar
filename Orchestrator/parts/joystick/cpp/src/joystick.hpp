#ifndef SOATJOYPART_H
#define SOATJOYPART_H

#include <linux/joystick.h>
#include "SoatcarState.hpp"
#include "config.hpp"


#define MIN_JOYSTICK_INPUT_VALUE -32768
#define MAX_JOYSTICK_INPUT_VALUE 32768



// Structs declaration
typedef struct {
	// joystick device file descriptor
	int fd;
	// joystick name
	char name[80];
	// number of buttons
	int buttons;
	// number of axis
	int axis;
} joyInfos_t;

typedef struct js_event js_event_t;


class JoystickPart
{
	public:
		JoystickPart();
		void Run();
		
	private:
		SoatcarState state{"/var/tmp/soatcarmmf.tmp"};
		JoystickConfig config{"joystick.conf"};
		unsigned long long nbInput;
		joyInfos_t* infos;
		
		joyInfos_t* OpenJoystickDevice(uint8_t joyNo);
		js_event_t PollJoystick(joyInfos_t *joy);
		void CloseJoystickDevice(joyInfos_t *joy);
};

#endif