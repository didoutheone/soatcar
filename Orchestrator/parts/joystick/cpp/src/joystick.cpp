#include <unistd.h>			// for read, close
#include <iostream>			// for cout, cerr
#include <thread>       	// for this_thread
#include <fcntl.h>			// For open
#include <string.h>			// For strcpy
#include <errno.h>			// For errno
#include <sys/ioctl.h> 		// For ioctl
#include <linux/joystick.h> // For joystick structures

#include "joystick.hpp"

using namespace std;

JoystickPart::JoystickPart()
{
	nbInput = 0;
	
	cout << "Initializing Joystick..." << endl;

	infos = openJoystickDevice(config.DeviceNo);
	if(infos == NULL)
	{
		cerr << "Error while initializing Joystick " << config.DeviceNo << endl;
		exit(-1);
	}
	
    cout << "Joystick initialized successfully" << endl;
	
	state.setJoystickReady(true);
}

void JoystickPart::run()
{
	while(!state.getStopFlag())
	{
		js_event_t event = pollJoystick(infos);
		
		if(event.type != 0x0 && event.type != 0x40)
		{
			nbInput++;
			
			// Gestion des boutons
			if (event.type == 0x1)
            {
                cout << "Time " << event.time << ": button number " << event.number << " was " << ((event.value == 0) ? "Released" : "Pressed") << endl;
                if (event.number == config.Button1)
                {
                    // Button1 is the cross: Stop
					state.setStopFlag(true);
					cout << "Button1 pressed: STOP!" << endl;
                }
                else
                {
                    cout << "Button " << event.number << " pressed: nothing configured" << endl;
                }
            }
			// Gestion des axes
			else if (event.type == 0x2)
			{
				cout << "Time " << event.time << ": axis number " << event.number << " was pushed to " << event.value << endl;
				if (event.number == config.AxisDirection)
                {
					int finalValue = state.translate(MIN_JOYSTICK_INPUT_VALUE, MAX_JOYSTICK_INPUT_VALUE, MIN_STATE_FIELDS_VALUE, MAX_STATE_FIELDS_VALUE, event.value, config.AxisDirectionInverted);
					// Always respect min and max
					if(finalValue < MIN_STATE_FIELDS_VALUE) finalValue = MIN_STATE_FIELDS_VALUE;
					if(finalValue > MAX_STATE_FIELDS_VALUE) finalValue = MAX_STATE_FIELDS_VALUE;
					state.setJoystickSteeringValue(finalValue);
					cout << "Direction was set in State to " << finalValue << endl;
                }
                else if (event.number == config.AxisSpeed)
                {
					int finalValue = state.translate(MIN_JOYSTICK_INPUT_VALUE, MAX_JOYSTICK_INPUT_VALUE, MIN_STATE_FIELDS_VALUE, MAX_STATE_FIELDS_VALUE, event.value, config.AxisSpeedInverted);
					// Always respect min and max
					if(finalValue < MIN_STATE_FIELDS_VALUE) finalValue = MIN_STATE_FIELDS_VALUE;
					if(finalValue > MAX_STATE_FIELDS_VALUE) finalValue = MAX_STATE_FIELDS_VALUE;
					// Also respect max speed value
					if((unsigned int)finalValue > state.getMaxThrottleLimit()) finalValue = state.getMaxThrottleLimit();
					state.setJoystickThrottleValue(finalValue);
					cout << "Throttle was set in State to " << finalValue << endl;
                }
			}
		}
		
		this_thread::sleep_for(chrono::milliseconds(5));
	}
	
	closeJoystickDevice(infos);
	state.setJoystickReady(false);
}

joyInfos_t* JoystickPart::openJoystickDevice(uint8_t joyNo)
{
	int joy_fd, num_of_axis=0, num_of_buttons=0;
	char name_of_joystick[80];
	char buf[16];
	
	sprintf(buf, "/dev/input/js%d", joyNo);
	
	// open joystick device
	joy_fd = open(buf, O_RDONLY|O_NONBLOCK);
	if(joy_fd == -1)
	{
		cerr << "Couldn’t open joystick device " << buf << endl;
		return NULL;
	}
	
	// get joystick infos
	ioctl(joy_fd, JSIOCGAXES, &num_of_axis);
	ioctl(joy_fd, JSIOCGBUTTONS, &num_of_buttons);
	ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);
	
	joyInfos_t *joy = (joyInfos_t*)malloc(sizeof(joyInfos_t));
	joy->fd = joy_fd;
	strcpy(joy->name, name_of_joystick);
	joy->buttons = num_of_buttons;
	joy->axis = num_of_axis;
	
	return joy;
}

js_event_t JoystickPart::pollJoystick(joyInfos_t *joy)
{
	js_event_t js;
	int nbRead = read(joy->fd, &js, sizeof(js));
	
	// EAGAIN is returned when the queue is empty
	if (nbRead == -1 && errno == EAGAIN)
	{
		js.type = 0x0; // no event available
		return js;
	}
	
	if (nbRead == -1)
	{
		perror("PollJoystick");
		js.type = 0x40; // Set erreur
		return js;
	}
	
	return js;
}

void JoystickPart::closeJoystickDevice(joyInfos_t *joy)
{
	close(joy->fd);
}
