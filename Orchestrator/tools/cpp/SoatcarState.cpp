#include <sys/mman.h>	// For mmap(), PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED: memory map file
#include <sys/stat.h>	// For fstat(): size of file
#include <fcntl.h>		// For O_RDWR: open file flag
#include <stdio.h>		// For perror, printf
#include <stdlib.h>		// For malloc, exit, EXIT_FAILURE
#include <unistd.h>		// For close: close file
#include <string.h>		// For memcpy: copy memory
#include "SoatcarState.hpp"

// -------------------------
//         PRIVATE
// -------------------------
bool SoatcarState::_getFlag(int offset)
{
	if(_mmap[offset] == 0) return false;
	else return true;
}

void SoatcarState::_setFlag(int offset, bool flag)
{
	if(flag) _mmap[offset] = 1;
	else _mmap[offset] = 0;
}


u32 SoatcarState::_getInt(int offset)
{
	return ((u8)_mmap[offset + 3] << 24) | ((u8)_mmap[offset + 2] << 16) | ((u8)_mmap[offset + 1] << 8) | (u8)_mmap[offset + 0];
}

void SoatcarState::_setInt(int offset, u32 val)
{
	_mmap[offset + 3] = val >> 24 & 0xFF;
	_mmap[offset + 2] = val >> 16 & 0xFF;
	_mmap[offset + 1] = val >> 8 & 0xFF;
	_mmap[offset + 0] = val >> 0 & 0xFF;
}


u64 SoatcarState::_getLong(int offset)
{
	return ((u64)_mmap[offset + 7] << 56) | ((u64)_mmap[offset + 6] << 48) | ((u64)_mmap[offset + 5] << 40) | ((u64)_mmap[offset + 4] << 32)
		| ((u64)_mmap[offset + 3] << 24) | ((u64)_mmap[offset + 2] << 16) | ((u64)_mmap[offset + 1] << 8) | ((u64)_mmap[offset + 0] << 0);
}

void SoatcarState::_setLong(int offset, u64 val)
{
	_mmap[offset + 7] = val >> 56 & 0xFF;
	_mmap[offset + 6] = val >> 48 & 0xFF;
	_mmap[offset + 5] = val >> 40 & 0xFF;
	_mmap[offset + 4] = val >> 32 & 0xFF;
	_mmap[offset + 3] = val >> 24 & 0xFF;
	_mmap[offset + 2] = val >> 16 & 0xFF;
	_mmap[offset + 1] = val >> 8 & 0xFF;
	_mmap[offset + 0] = val >> 0 & 0xFF;
}


char* SoatcarState::_getString(int offset)
{
	u32 length = _getInt(offset);
	char* data = (char*)malloc(length + 1);
	memcpy(data, _mmap + offset + 4, length);
	data[length] = '\0';
	return data;
}

void SoatcarState::_setString(int offset, char* str)
{
	u32 length = strlen(str);
	_setInt(offset, length);
	memcpy(_mmap + offset + 4, str, length);
}


DataBuf SoatcarState::_getData(int offset)
{
	u32 length = _getInt(offset);
	u8* data = (u8*)malloc(length);
	memcpy(data, _mmap + offset + 4, length);
	DataBuf ret;
	ret.length = length;
	ret.buffer = data;
	return ret;
}

void SoatcarState::_setData(int offset, DataBuf data)
{
	_setInt(offset, data.length);
	memcpy(_mmap + offset + 4, data.buffer, data.length);
}



// -------------------------
//         PUBLIC
// -------------------------
SoatcarState::SoatcarState(const char *fileName)
{
	int fd;
	struct stat sb;
	size_t length;

	fd = open(fileName, O_RDWR);
	if (fd == -1)
		handle_error("ERROR: SOATCARSTATE - open");

	if (fstat(fd, &sb) == -1)      // Pour obtenir la taille du fichier
		handle_error("ERROR: SOATCARSTATE - fstat");
	length = sb.st_size;

	_mmap = (u8*)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_mmap == MAP_FAILED)
		handle_error("ERROR: SOATCARSTATE - mmap");

	close(fd);
}


double SoatcarState::translate(double fromMin, double fromMax, double toMin, double toMax, double value, bool inverted)
{
	double val = toMin + ((value - fromMin) * (toMax - toMin) / (fromMax - fromMin));
			
	if (inverted) val = toMax - val;
	
	return val;
}

//
// Common fields
//
bool SoatcarState::getStopFlag() { return _getFlag(CONST_STOP_FLAG); }
void SoatcarState::setStopFlag(bool flag) { _setFlag(CONST_STOP_FLAG, flag); }

bool SoatcarState::isThrottleAuto() { return _getFlag(CONST_IS_THROTTLE_AUTO); }
void SoatcarState::setThrottleAuto(bool flag) { _setFlag(CONST_IS_THROTTLE_AUTO, flag); }

bool SoatcarState::isSteeringAuto() { return _getFlag(CONST_IS_STEERING_AUTO); }
void SoatcarState::setSteeringAuto(bool flag) { _setFlag(CONST_IS_STEERING_AUTO, flag); }

u32 SoatcarState::getMaxThrottleLimit() { return _getInt(CONST_MAX_THROTTLE_LIMIT); }
void SoatcarState::setMaxThrottleLimit(u32 value) { _setInt(CONST_MAX_THROTTLE_LIMIT, value); }

bool SoatcarState::isConstantThrottleActive() { return _getFlag(CONST_IS_CONSTANT_THROTTLE_ACTIVE); }
void SoatcarState::setConstantThrottleActive(bool flag) { _setFlag(CONST_IS_CONSTANT_THROTTLE_ACTIVE, flag); }

u32 SoatcarState::getConstantThrottleValue() { return _getInt(CONST_CONSTANT_THROTTLE_VALUE); }
void SoatcarState::setConstantThrottleValue(u32 value) { _setInt(CONST_CONSTANT_THROTTLE_VALUE, value); }

char* SoatcarState::getHostName() { return _getString(CONST_HOSTNAME); }
void SoatcarState::setHostName(char* value)  { _setString(CONST_HOSTNAME, value); }


//
// Joystick fields
//
bool SoatcarState::isJoystickReady() { return _getFlag(CONST_IS_JOYSTICK_READY); }
void SoatcarState::setJoystickReady(bool flag) { _setFlag(CONST_IS_JOYSTICK_READY, flag); }

u32 SoatcarState::getJoystickLoopRate() { return _getInt(CONST_JOYSTICK_LOOP_RATE); }
void SoatcarState::setJoystickLoopRate(u32 value) { _setInt(CONST_JOYSTICK_LOOP_RATE, value); }

u32 SoatcarState::getJoystickSteeringValue() { return _getInt(CONST_JOYSTICK_STEERING_VALUE); }
void SoatcarState::setJoystickSteeringValue(u32 value) { _setInt(CONST_JOYSTICK_STEERING_VALUE, value); }

u32 SoatcarState::getJoystickThrottleValue() { return _getInt(CONST_JOYSTICK_THROTTLE_VALUE); }
void SoatcarState::setJoystickThrottleValue(u32 value) { _setInt(CONST_JOYSTICK_THROTTLE_VALUE, value); }


//
// PWM part
//
bool SoatcarState::isActuatorReady() { return _getFlag(CONST_IS_ACTUATOR_READY); }
void SoatcarState::setActuatorReady(bool flag) { _setFlag(CONST_IS_ACTUATOR_READY, flag); }

u32 SoatcarState::getActuatorLoopRate() { return _getInt(CONST_ACTUATOR_LOOP_RATE); }
void SoatcarState::setActuatorLoopRate(u32 value) { _setInt(CONST_ACTUATOR_LOOP_RATE, value); }

u32 SoatcarState::getSteeringOutput() { return _getInt(CONST_STEERING_OUTPUT); }
void SoatcarState::setSteeringOutput(u32 value) { _setInt(CONST_STEERING_OUTPUT, value); }

u32 SoatcarState::getThrottleOutput() { return _getInt(CONST_THROTTLE_OUTPUT); }
void SoatcarState::setThrottleOutput(u32 value) { _setInt(CONST_THROTTLE_OUTPUT, value); }


//
// System sensors
//
bool SoatcarState::isSystemSensorReady() { return _getFlag(CONST_IS_SYSTEM_SENSOR_READY); }
void SoatcarState::setSystemSensorReady(bool flag) { _setFlag(CONST_IS_SYSTEM_SENSOR_READY, flag); }

u32 SoatcarState::getSystemSensorLoopRate() { return _getInt(CONST_SYSTEM_SENSOR_LOOP_RATE); }
void SoatcarState::setSystemSensorLoopRate(u32 value) { _setInt(CONST_SYSTEM_SENSOR_LOOP_RATE, value); }

u32 SoatcarState::getCPUTemp() { return _getInt(CONST_CPU_TEMP); }
void SoatcarState::setCPUTemp(u32 value) { _setInt(CONST_CPU_TEMP, value); }

u32 SoatcarState::getCPULoad() { return _getInt(CONST_CPU_LOAD); }
void SoatcarState::setCPULoad(u32 value) { _setInt(CONST_CPU_LOAD, value); }

u32 SoatcarState::getMemoryLoad() { return _getInt(CONST_RAM_LOAD); }
void SoatcarState::setMemoryLoad(u32 value) { _setInt(CONST_RAM_LOAD, value); }

u32 SoatcarState::getDiskLoad() { return _getInt(CONST_DISK_LOAD); }
void SoatcarState::setDiskLoad(u32 value) { _setInt(CONST_DISK_LOAD, value); }

u32 SoatcarState::getLoadAverage() { return _getInt(CONST_LOAD_AVERAGE); }
void SoatcarState::setLoadAverage(u32 value) { _setInt(CONST_LOAD_AVERAGE, value); }


//
// Physical sensors
//
bool SoatcarState::isPhysicalSensorReady() { return _getFlag(CONST_IS_PHYSICAL_SENSOR_READY); }
void SoatcarState::setPhysicalSensorReady(bool flag) { _setFlag(CONST_IS_PHYSICAL_SENSOR_READY, flag); }

u32 SoatcarState::getPhysicalSensorLoopRate() { return _getInt(CONST_PHYSICAL_SENSOR_LOOP_RATE); }
void SoatcarState::setPhysicalSensorLoopRate(u32 value) { _setInt(CONST_PHYSICAL_SENSOR_LOOP_RATE, value); }

u32 SoatcarState::getUltrasonicDistance() { return _getInt(CONST_ULTRASONIC_DISTANCE); }
void SoatcarState::setUltrasonicDistance(u32 value) { _setInt(CONST_ULTRASONIC_DISTANCE, value); }


//
// Auto pilot
//
bool SoatcarState::isAutoPilotReady() { return _getFlag(CONST_IS_AUTOPILOT_READY); }
void SoatcarState::setAutoPilotReady(bool flag) { _setFlag(CONST_IS_AUTOPILOT_READY, flag); }

u32 SoatcarState::getAutoPilotLoopRate() { return _getInt(CONST_AUTOPILOT_LOOP_RATE); }
void SoatcarState::setAutoPilotLoopRate(u32 value) { _setInt(CONST_AUTOPILOT_LOOP_RATE, value); }

u32 SoatcarState::getAutoPilotSteeringValue() { return _getInt(CONST_AUTO_STEERING_VALUE); }
void SoatcarState::setAutoPilotSteeringValue(u32 value) { _setInt(CONST_AUTO_STEERING_VALUE, value); }

u32 SoatcarState::getAutoPilotThrottleValue() { return _getInt(CONST_AUTO_THROTTLE_VALUE); }
void SoatcarState::setAutoPilotThrottleValue(u32 value) { _setInt(CONST_AUTO_THROTTLE_VALUE, value); }


//
// Camera
//
bool SoatcarState::isCameraReady() { return _getFlag(CONST_IS_CAMERA_READY); }
void SoatcarState::setCameraReady(bool flag) { _setFlag(CONST_IS_CAMERA_READY, flag); }

u32 SoatcarState::getCameraLoopRate() { return _getInt(CONST_CAMERA_LOOP_RATE); }
void SoatcarState::setCameraLoopRate(u32 value) { _setInt(CONST_CAMERA_LOOP_RATE, value); }

bool SoatcarState::isRecording() { return _getFlag(CONST_IS_RECORDING); }
void SoatcarState::setRecording(bool flag) { _setFlag(CONST_IS_RECORDING, flag); }

char* SoatcarState::getRecordingPrefix() { return _getString(CONST_RECORDING_PREFIX); }
void SoatcarState::setRecordingPrefix(char* value)  { _setString(CONST_RECORDING_PREFIX, value); }

char* SoatcarState::getRecordingFolder() { return _getString(CONST_RECORDING_FOLDER); }
void SoatcarState::setRecordingFolder(char* value)  { _setString(CONST_RECORDING_FOLDER, value); }

u32 SoatcarState::getRecordingImageIndex() { return _getInt(CONST_RECORDING_IMAGE_INDEX); }
void SoatcarState::setRecordingImageIndex(u32 value) { _setInt(CONST_RECORDING_IMAGE_INDEX, value); }


//
// Remote monitoring web server
//
bool SoatcarState::isRemoteMonitoringReady() { return _getFlag(CONST_IS_REMOTE_MONITORING_READY); }
void SoatcarState::setRemoteMonitoringReady(bool flag) { _setFlag(CONST_IS_REMOTE_MONITORING_READY, flag); }

u32 SoatcarState::getRemoteMonitoringLoopRate() { return _getInt(CONST_REMOTE_MONITORING_LOOP_RATE); }
void SoatcarState::setRemoteMonitoringLoopRate(u32 value) { _setInt(CONST_REMOTE_MONITORING_LOOP_RATE, value); }


//
// LED Management
//
bool SoatcarState::isLedManagementReady() { return _getFlag(CONST_IS_LED_MANAGER_READY); }
void SoatcarState::setLedManagementReady(bool flag) { _setFlag(CONST_IS_LED_MANAGER_READY, flag); }

u32 SoatcarState::getLedManagementLoopRate() { return _getInt(CONST_LED_MANAGER_LOOP_RATE); }
void SoatcarState::setLedManagementLoopRate(u32 value) { _setInt(CONST_LED_MANAGER_LOOP_RATE, value); }


//
// Images
//
u64 SoatcarState::getRawImageNo() { return _getLong(CONST_CAMERA_IMAGE_NO); }
void SoatcarState::setRawImageNo(u64 imageNo) { _setLong(CONST_CAMERA_IMAGE_NO, imageNo); }

DataBuf SoatcarState::getRawImage() { return _getData(CONST_CAMERA_IMAGE); }
void SoatcarState::setRawImage(DataBuf data) { _setData(CONST_CAMERA_IMAGE, data); }

u64 SoatcarState::getTreatedImageNo() { return _getLong(CONST_TREATED_IMAGE_NO); }
void SoatcarState::setTreatedImageNo(u64 imageNo) { _setLong(CONST_TREATED_IMAGE_NO, imageNo); }

DataBuf SoatcarState::getTreatedImage() { return _getData(CONST_TREATED_IMAGE); }
void SoatcarState::setTreatedImage(DataBuf data) { _setData(CONST_TREATED_IMAGE, data); }


