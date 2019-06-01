#include <iostream>			// for cout and cerr
#include <unistd.h>			// for usleep
#include <linux/i2c-dev.h> 	// for i2c structures
#include <linux/i2c.h>		// for i2c structures
#include <sys/ioctl.h>		// for ioctl calls
#include <fcntl.h>			// for file open
#include <math.h>			// for floor function

// Local headers
#include "pwm.hpp"

using namespace std;


PWM::PWM(int pwmType)
{
	int meanvalue;
	switch(pwmType)
	{
		case I2C_THROTTLE:
			openI2CDevice(config.Address, config.BusNo, config.SpeedChannel, config.PulseFrequency);
			infos->type = I2C_THROTTLE;
			infos->minValue = config.SpeedMinValue;
			infos->maxValue = config.SpeedMaxValue;
			// Set PWM to medium
			meanvalue = (int)((double)infos->minValue + ((double)infos->maxValue - (double)infos->minValue) / (double)2);
			setPWM(infos, 0, meanvalue);
			break;
		
		case I2C_STEERING:
			openI2CDevice(config.Address, config.BusNo, config.DirectionChannel, config.PulseFrequency);
			infos->type = I2C_STEERING;
			infos->minValue = config.DirectionMinValue;
			infos->maxValue = config.DirectionMaxValue;
			// Set PWM to min, max and medium
			meanvalue = (int)((double)infos->minValue + ((double)infos->maxValue - (double)infos->minValue) / (double)2);
			setPWM(infos, 0, infos->minValue);
			setPWM(infos, 0, infos->maxValue);
			setPWM(infos, 0, meanvalue);
			break;
		
		default:
			cerr << "pwmType not recognized: " << pwmType << endl;
			exit(-1);
	}
}

PWM::~PWM()
{
	closeI2CDevice(infos);
}

void PWM::work()
{
	if(infos->type == I2C_STEERING)
	{
		workSteering();
	}
	else
	{
		workThrottle();
	}
}

void PWM::workSteering()
{
	int value;
	if(state.isSteeringAuto())
	{
		value = state.getAutoPilotSteeringValue();
	}
	else
	{
		value = state.getJoystickSteeringValue();
	}
	
	if(value != lastValue)
	{
		int outputValue = (int)state.translate(MIN_STATE_FIELDS_VALUE, MAX_STATE_FIELDS_VALUE, infos->minValue, infos->maxValue, value, false);
		if(outputValue < infos->minValue) outputValue = infos->minValue;
		if(outputValue > infos->maxValue) outputValue = infos->maxValue;
		
		int ret = setPWM(infos, 0, outputValue);
		if(ret < 0)
		{
			cerr << "ERROR Steering could not be set to " << outputValue << endl;
		}
		else
		{
			lastValue = value;
			int backValue = (int)state.translate(infos->minValue, infos->maxValue, MIN_STATE_FIELDS_VALUE, MAX_STATE_FIELDS_VALUE, value, false);
			state.setSteeringOutput(backValue);
		}
	}
}

void PWM::workThrottle()
{
	int value;
	if(state.isConstantThrottleActive())
	{
		value = state.getConstantThrottleValue();
	}
	else
	{
		if(state.isThrottleAuto())
		{
			value = state.getAutoPilotThrottleValue();
		}
		else
		{
			value = state.getJoystickThrottleValue();
		}
	}
	
	if(value != lastValue)
	{
		// First check configured max Throttle
		if((unsigned int)value > state.getMaxThrottleLimit()) value = state.getMaxThrottleLimit();
		
		int outputValue = (int)state.translate(MIN_STATE_FIELDS_VALUE, MAX_STATE_FIELDS_VALUE, infos->minValue, infos->maxValue, value, false);
		if(outputValue < infos->minValue) outputValue = infos->minValue;
		if(outputValue > infos->maxValue) outputValue = infos->maxValue;
		
		int ret = setPWM(infos, 0, outputValue);
		if(ret < 0)
		{
			cerr << "ERROR Throttle could not be set to " << outputValue << endl;
		}
		else
		{
			lastValue = value;
			int backValue = (int)state.translate(infos->minValue, infos->maxValue, MIN_STATE_FIELDS_VALUE, MAX_STATE_FIELDS_VALUE, value, false);
			state.setThrottleOutput(backValue);
		}
	}
}

void PWM::openI2CDevice(uint8_t address, uint8_t busNo, uint8_t channel, float pulseFreq)
{
	// Open the I2C device
	infos = i2cOpen(address, busNo, channel);
	if(infos == NULL)
	{
		cerr << "Failed to open the I2C device" << endl;
		exit(-2);
	}
	
	// We are all set, now init the PWM
	if(init(infos) < 0)
	{
		cerr << "Failed to init the PWM" << endl;
		exit(-3);
	}
	
	// And set the pulse frequency
	if(setPWMFreq(infos, pulseFreq) < 0)
	{
		cerr << "Failed to set the pulse frequency for the PWM" << endl;
		exit(-4);
	}
}

i2cInfos_t* PWM::i2cOpen(uint8_t address, uint8_t busNo, uint8_t channel)
{
	cout << "Attempting to open I2C device" << endl;
	
	// Open I2C busNo
	char buf[16];
	sprintf(buf, "/dev/i2c-%d", busNo);
	int fd;
	if ((fd = open(buf, O_RDWR)) < 0)
	{
		// Open port for reading and writing
		cerr << "Failed to open i2c bus " << buf << endl;
		return NULL;
	}
	
	// Select device by its address
	if (ioctl(fd, I2C_SLAVE, address) < 0)
	{
		cerr << "Failed to acquire bus access and/or talk to slave on address " << address << endl;
		return NULL;
	}
	
	i2cInfos_t* i2c = (i2cInfos_t*)malloc(sizeof(i2cInfos_t));
	i2c->fd = fd;
	i2c->address = address;
	i2c->busNo = busNo;
	i2c->channel = channel;
	
	return i2c;
}

int PWM::init(i2cInfos_t* i2c)
{
	cout << "Attempting to init PWM device" << endl;

	// return values
	int retVal = -1;
	
	// Reset all pwm
	retVal = write8(i2c, ALL_LED_ON_L, 0);
	if(retVal < 0) return retVal;
	
	retVal = write8(i2c, ALL_LED_ON_H, 0);
	if(retVal < 0) return retVal;
	
	retVal = write8(i2c, ALL_LED_OFF_L, 0);
	if(retVal < 0) return retVal;
	
	retVal = write8(i2c, ALL_LED_OFF_H, 0);
	if(retVal < 0) return retVal;
	
	// Dont know ! :p
	retVal = write8(i2c, PCA9685_MODE2, OUTDRV);
	if(retVal < 0) return retVal;
	
	retVal = write8(i2c, PCA9685_MODE1, ALLCALL);
	if(retVal < 0) return retVal;
	usleep(5000); // wait 5ms for oscillator
	
	// Get current mode
	uint8_t mode;
	retVal = readReg(i2c, PCA9685_MODE1, &mode);
	if(retVal < 0) return retVal;
	
	// Wake up time
	mode = mode & ~SLEEP;
	retVal = write8(i2c, PCA9685_MODE1, mode);
	if(retVal < 0) return retVal;
	usleep(5000); // wait 5ms for wake up
	
	return 0;
}

int PWM::setPWMFreq(i2cInfos_t* i2c, float freq)
{
	cout << "Attempting to set pulse frequency" << endl;

	// return values
	int retVal = -1;
	
	//freq *= 0.9;  // Correct for overshoot in the frequency setting
	float prescaleval = 25000000;
	prescaleval /= 4096;
	prescaleval /= freq;
	prescaleval -= 1;
	cout << "Estimated pre-scale: " << prescaleval << endl;

	uint8_t prescale = floor(prescaleval + 0.5);
	cout << "Final pre-scale: " << prescale << endl;

	// Get current mode
	uint8_t oldmode;
	retVal = readReg(i2c, PCA9685_MODE1, &oldmode);
	if(retVal < 0) return retVal;
	
	// Go to sleep
	uint8_t newmode = (oldmode&0x7F) | SLEEP;
	retVal = write8(i2c, PCA9685_MODE1, newmode);
	if(retVal < 0) return retVal;
	
	// Set the prescaler
	retVal = write8(i2c, PCA9685_PRESCALE, prescale);
	if(retVal < 0) return retVal;
	
	// Return to old mode
	retVal = write8(i2c, PCA9685_MODE1, oldmode);
	if(retVal < 0) return retVal;
	usleep(5000); // Wait 5ms
	
	// This sets the MODE1 register to turn on auto increment (0xa0). En python : 0x80
	retVal = write8(i2c, PCA9685_MODE1, oldmode | 0x80);  
	if(retVal < 0) return retVal;

	// Get current mode for info
	uint8_t nowmode;
	retVal = readReg(i2c, PCA9685_MODE1, &nowmode);
	if(retVal < 0) return retVal;
	
	cout << "Mode now " << nowmode << endl;
	return 0;
}

int PWM::setPWM(i2cInfos_t* i2c, int on, int off)
{
	
	if(i2c == NULL)
	{
		cerr << "I2C device not valid (NULL)" << endl;
		return -1;
	}
	
	int retVal = write8(i2c, LED0_ON_L+4*i2c->channel, on & 0xFF);
	if(retVal < 0) return retVal;
	
	retVal = write8(i2c, LED0_ON_H+4*i2c->channel, on >> 8);
	if(retVal < 0) return retVal;
	
	retVal = write8(i2c, LED0_OFF_L+4*i2c->channel, off & 0xFF);
	if(retVal < 0) return retVal;
	
	retVal = write8(i2c, LED0_OFF_H+4*i2c->channel, off >> 8);
	if(retVal < 0) return retVal;
	
	return 0;
}

void PWM::closeI2CDevice(i2cInfos_t* i2c)
{
	if(i2c == NULL)
	{
		cerr << "I2C device not valid (NULL)" << endl;
		return;
	}
	
	close(i2c->fd);
	free(i2c);
}

/***************************************** HELPERS **************************************************/


/********************************************************************
 * This function reads a byte of data "data" from a specific register
 * "reg_addr" in the I2C device. This involves sending the register address
 * byte "reg_Addr" with "write" asserted and then instructing the
 * I2C device to read a byte of data from that address ("read asserted").
 * This necessitates the use of two i2c_msg structs. One for the register
 * address write and another for the read from the I2C device i.e.
 * I2C_M_RD flag is set. The read data is then saved into the reference
 * variable "data".
 ********************************************************************/
int PWM::readReg(i2cInfos_t* i2c, uint8_t reg_addr, uint8_t *data) {

    uint8_t *inbuff, outbuff;
    int retVal = -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    outbuff = reg_addr;
    messages[0].addr = i2c->address;
    messages[0].flags= 0;
    messages[0].len = sizeof(outbuff);
    messages[0].buf = &outbuff;

    inbuff = data;
    messages[1].addr = i2c->address;
    messages[1].flags = I2C_M_RD;
    messages[1].len = sizeof(*inbuff);
    messages[1].buf = inbuff;

    packets.msgs = messages;
    packets.nmsgs = 2;

    retVal = ioctl(i2c->fd, I2C_RDWR, &packets);
    if(retVal < 0)
	{
        cerr << "Read register " << reg_addr << " from I2C device failed with code " << retVal << endl;
	}

    return retVal;
}


/********************************************************************
 * This function writes a byte of data "data" to a specific register
 * "reg_addr" in the I2C device. This involves sending these two bytes
 * in order to the i2C device by means of the ioctl() command. Since
 * both bytes are written (no read/write switch), both pieces
 * of information can be sent in a single message (i2c_msg structure)
 ********************************************************************/
int PWM::write8(i2cInfos_t* i2c, uint8_t reg_addr, uint8_t data) {

    uint8_t buff[2];
    int retVal = -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    buff[0] = reg_addr;
    buff[1] = data;

    messages[0].addr = i2c->address;
    messages[0].flags = 0;
    messages[0].len = sizeof(buff);
    messages[0].buf = buff;

    packets.msgs = messages;
    packets.nmsgs = 1;

    retVal = ioctl(i2c->fd, I2C_RDWR, &packets);
    if(retVal < 0) 
	{
        cerr << "Write data " << data << " in register " << reg_addr << " to I2C Device failed with code " << retVal << endl;
	}

    return retVal;
}