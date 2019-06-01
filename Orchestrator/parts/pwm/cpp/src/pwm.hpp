#ifndef SOATPWMPART_H
#define SOATPWMPART_H

#include "config.hpp"
#include "SoatcarState.hpp"


typedef struct {
	// pwm type (steering or direction)
	int type;
	// i2c device file descriptor
	int fd;
	// I2C device address (0x40)
	uint8_t address;
	// I2C bus number (Revision 2 Pi uses I2C bus 1)
	uint8_t busNo;
	// Channel number (0 throttle, 1 steering for example)
	uint8_t channel;
	// min and max values
	int minValue;
	int maxValue;
} i2cInfos_t;


#define PCA9685_MODE1 		0x0
#define PCA9685_MODE2 		0x1
#define PCA9685_PRESCALE 	0xFE

#define SLEEP 				0x10
#define OUTDRV 				0x04
#define ALLCALL 			0x01

#define LED0_ON_L			0x06
#define LED0_ON_H			0x07
#define LED0_OFF_L			0x08
#define LED0_OFF_H			0x09

#define ALL_LED_ON_L 		0xFA
#define ALL_LED_ON_H 		0xFB
#define ALL_LED_OFF_L 		0xFC
#define ALL_LED_OFF_H 		0xFD

#define I2C_M_RD 			0x0001  // read data, from slave to master


#define I2C_THROTTLE	1
#define I2C_STEERING	2

class PWM
{
	public:
		PWM(int i2CType);
		~PWM();
		void work();
		
	private:
		PWMConfig config{"pwm.conf"};
		SoatcarState state{"/var/tmp/soatcarmmf.tmp"};
		i2cInfos_t* infos;
		int lastValue;
		
		void workSteering();
		void workThrottle();
		
		void openI2CDevice(uint8_t address, uint8_t busNo, uint8_t channel, float pulseFreq);
		int setPWM(i2cInfos_t *i2c, int on, int off);
		void closeI2CDevice(i2cInfos_t *i2c);

		i2cInfos_t* i2cOpen(uint8_t address, uint8_t busNo, uint8_t channel);
		int init(i2cInfos_t *i2c);
		int setPWMFreq(i2cInfos_t *i2c, float freq);
		int readReg(i2cInfos_t *i2c, uint8_t reg_addr, uint8_t *data);
		int write8(i2cInfos_t *i2c, uint8_t reg_addr, uint8_t data);
};

#endif