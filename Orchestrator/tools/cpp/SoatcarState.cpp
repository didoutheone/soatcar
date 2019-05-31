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


double SoatcarState::Translate(double fromMin, double fromMax, double toMin, double toMax, double value, bool inverted)
{
	double val = toMin + ((value - fromMin) * (toMax - toMin) / (fromMax - fromMin));
			
	if (inverted) val = toMax - val;
	
	return val;
}

//
// Common fields
//
bool SoatcarState::GetStopFlag() { return _getFlag(CONST_STOP_FLAG); }
void SoatcarState::SetStopFlag(bool flag) { _setFlag(CONST_STOP_FLAG, flag); }

bool SoatcarState::IsThrottleAuto() { return _getFlag(CONST_IS_THROTTLE_AUTO); }
void SoatcarState::SetThrottleAuto(bool flag) { _setFlag(CONST_IS_THROTTLE_AUTO, flag); }

bool SoatcarState::IsSteeringAuto() { return _getFlag(CONST_IS_STEERING_AUTO); }
void SoatcarState::SetSteeringAuto(bool flag) { _setFlag(CONST_IS_STEERING_AUTO, flag); }

u32 SoatcarState::GetMaxThrottleLimit() { return _getInt(CONST_MAX_THROTTLE_LIMIT); }
void SoatcarState::SetMaxThrottleLimit(u32 value) { _setInt(CONST_MAX_THROTTLE_LIMIT, value); }

bool SoatcarState::IsConstantThrottleActive() { return _getFlag(CONST_IS_CONSTANT_THROTTLE_ACTIVE); }
void SoatcarState::SetConstantThrottleActive(bool flag) { _setFlag(CONST_IS_CONSTANT_THROTTLE_ACTIVE, flag); }

u32 SoatcarState::GetConstantThrottleValue() { return _getInt(CONST_CONSTANT_THROTTLE_VALUE); }
void SoatcarState::SetConstantThrottleValue(u32 value) { _setInt(CONST_CONSTANT_THROTTLE_VALUE, value); }

char* SoatcarState::GetHostName() { return _getString(CONST_HOSTNAME); }
void SoatcarState::SetHostName(char* value)  { _setString(CONST_HOSTNAME, value); }


//
// Joystick fields
//
bool SoatcarState::IsJoystickReady() { return _getFlag(CONST_IS_JOYSTICK_READY); }
void SoatcarState::SetJoystickReady(bool flag) { _setFlag(CONST_IS_JOYSTICK_READY, flag); }

u32 SoatcarState::GetJoystickLoopRate() { return _getInt(CONST_JOYSTICK_LOOP_RATE); }
void SoatcarState::SetJoystickLoopRate(u32 value) { _setInt(CONST_JOYSTICK_LOOP_RATE, value); }

u32 SoatcarState::GetJoystickSteeringValue() { return _getInt(CONST_JOYSTICK_STEERING_VALUE); }
void SoatcarState::SetJoystickSteeringValue(u32 value) { _setInt(CONST_JOYSTICK_STEERING_VALUE, value); }

u32 SoatcarState::GetJoystickThrottleValue() { return _getInt(CONST_JOYSTICK_THROTTLE_VALUE); }
void SoatcarState::SetJoystickThrottleValue(u32 value) { _setInt(CONST_JOYSTICK_THROTTLE_VALUE, value); }


//
// PWM part
//
bool SoatcarState::IsActuatorReady() { return _getFlag(CONST_IS_ACTUATOR_READY); }
void SoatcarState::SetActuatorReady(bool flag) { _setFlag(CONST_IS_ACTUATOR_READY, flag); }

u32 SoatcarState::GetActuatorLoopRate() { return _getInt(CONST_ACTUATOR_LOOP_RATE); }
void SoatcarState::SetActuatorLoopRate(u32 value) { _setInt(CONST_ACTUATOR_LOOP_RATE, value); }

u32 SoatcarState::GetSteeringOutput() { return _getInt(CONST_STEERING_OUTPUT); }
void SoatcarState::SetSteeringOutput(u32 value) { _setInt(CONST_STEERING_OUTPUT, value); }

u32 SoatcarState::GetThrottleOutput() { return _getInt(CONST_THROTTLE_OUTPUT); }
void SoatcarState::SetThrottleOutput(u32 value) { _setInt(CONST_THROTTLE_OUTPUT, value); }


//
// System sensors
//
bool SoatcarState::IsSystemSensorReady() { return _getFlag(CONST_IS_SYSTEM_SENSOR_READY); }
void SoatcarState::SetSystemSensorReady(bool flag) { _setFlag(CONST_IS_SYSTEM_SENSOR_READY, flag); }

u32 SoatcarState::GetSystemSensorLoopRate() { return _getInt(CONST_SYSTEM_SENSOR_LOOP_RATE); }
void SoatcarState::SetSystemSensorLoopRate(u32 value) { _setInt(CONST_SYSTEM_SENSOR_LOOP_RATE, value); }

u32 SoatcarState::GetCPUTemp() { return _getInt(CONST_CPU_TEMP); }
void SoatcarState::SetCPUTemp(u32 value) { _setInt(CONST_CPU_TEMP, value); }

u32 SoatcarState::GetCPULoad() { return _getInt(CONST_CPU_LOAD); }
void SoatcarState::SetCPULoad(u32 value) { _setInt(CONST_CPU_LOAD, value); }

u32 SoatcarState::GetMemoryLoad() { return _getInt(CONST_RAM_LOAD); }
void SoatcarState::SetMemoryLoad(u32 value) { _setInt(CONST_RAM_LOAD, value); }

u32 SoatcarState::GetDiskLoad() { return _getInt(CONST_DISK_LOAD); }
void SoatcarState::SetDiskLoad(u32 value) { _setInt(CONST_DISK_LOAD, value); }

u32 SoatcarState::GetLoadAverage() { return _getInt(CONST_LOAD_AVERAGE); }
void SoatcarState::SetLoadAverage(u32 value) { _setInt(CONST_LOAD_AVERAGE, value); }


//
// Physical sensors
//
bool SoatcarState::IsPhysicalSensorReady() { return _getFlag(CONST_IS_PHYSICAL_SENSOR_READY); }
void SoatcarState::SetPhysicalSensorReady(bool flag) { _setFlag(CONST_IS_PHYSICAL_SENSOR_READY, flag); }

u32 SoatcarState::GetPhysicalSensorLoopRate() { return _getInt(CONST_PHYSICAL_SENSOR_LOOP_RATE); }
void SoatcarState::SetPhysicalSensorLoopRate(u32 value) { _setInt(CONST_PHYSICAL_SENSOR_LOOP_RATE, value); }

u32 SoatcarState::GetUltrasonicDistance() { return _getInt(CONST_ULTRASONIC_DISTANCE); }
void SoatcarState::SetUltrasonicDistance(u32 value) { _setInt(CONST_ULTRASONIC_DISTANCE, value); }


//
// Auto pilot
//
bool SoatcarState::IsAutoPilotReady() { return _getFlag(CONST_IS_AUTOPILOT_READY); }
void SoatcarState::SetAutoPilotReady(bool flag) { _setFlag(CONST_IS_AUTOPILOT_READY, flag); }

u32 SoatcarState::GetAutoPilotLoopRate() { return _getInt(CONST_AUTOPILOT_LOOP_RATE); }
void SoatcarState::SetAutoPilotLoopRate(u32 value) { _setInt(CONST_AUTOPILOT_LOOP_RATE, value); }

u32 SoatcarState::GetAutoPilotSteeringValue() { return _getInt(CONST_AUTO_STEERING_VALUE); }
void SoatcarState::SetAutoPilotSteeringValue(u32 value) { _setInt(CONST_AUTO_STEERING_VALUE, value); }

u32 SoatcarState::GetAutoPilotThrottleValue() { return _getInt(CONST_AUTO_THROTTLE_VALUE); }
void SoatcarState::SetAutoPilotThrottleValue(u32 value) { _setInt(CONST_AUTO_THROTTLE_VALUE, value); }


//
// Camera
//
bool SoatcarState::IsCameraReady() { return _getFlag(CONST_IS_CAMERA_READY); }
void SoatcarState::SetCameraReady(bool flag) { _setFlag(CONST_IS_CAMERA_READY, flag); }

u32 SoatcarState::GetCameraLoopRate() { return _getInt(CONST_CAMERA_LOOP_RATE); }
void SoatcarState::SetCameraLoopRate(u32 value) { _setInt(CONST_CAMERA_LOOP_RATE, value); }

bool SoatcarState::IsRecording() { return _getFlag(CONST_IS_RECORDING); }
void SoatcarState::SetRecording(bool flag) { _setFlag(CONST_IS_RECORDING, flag); }

char* SoatcarState::GetRecordingPrefix() { return _getString(CONST_RECORDING_PREFIX); }
void SoatcarState::SetRecordingPrefix(char* value)  { _setString(CONST_RECORDING_PREFIX, value); }

char* SoatcarState::GetRecordingFolder() { return _getString(CONST_RECORDING_FOLDER); }
void SoatcarState::SetRecordingFolder(char* value)  { _setString(CONST_RECORDING_FOLDER, value); }

u32 SoatcarState::GetRecordingImageIndex() { return _getInt(CONST_RECORDING_IMAGE_INDEX); }
void SoatcarState::SetRecordingImageIndex(u32 value) { _setInt(CONST_RECORDING_IMAGE_INDEX, value); }


//
// Remote monitoring web server
//
bool SoatcarState::IsRemoteMonitoringReady() { return _getFlag(CONST_IS_REMOTE_MONITORING_READY); }
void SoatcarState::SetRemoteMonitoringReady(bool flag) { _setFlag(CONST_IS_REMOTE_MONITORING_READY, flag); }

u32 SoatcarState::GetRemoteMonitoringLoopRate() { return _getInt(CONST_REMOTE_MONITORING_LOOP_RATE); }
void SoatcarState::SetRemoteMonitoringLoopRate(u32 value) { _setInt(CONST_REMOTE_MONITORING_LOOP_RATE, value); }


//
// LED Management
//
bool SoatcarState::IsLedManagementReady() { return _getFlag(CONST_IS_LED_MANAGER_READY); }
void SoatcarState::SetLedManagementReady(bool flag) { _setFlag(CONST_IS_LED_MANAGER_READY, flag); }

u32 SoatcarState::GetLedManagementLoopRate() { return _getInt(CONST_LED_MANAGER_LOOP_RATE); }
void SoatcarState::SetLedManagementLoopRate(u32 value) { _setInt(CONST_LED_MANAGER_LOOP_RATE, value); }


//
// Images
//
u64 SoatcarState::GetRawImageNo() { return _getLong(CONST_CAMERA_IMAGE_NO); }
void SoatcarState::SetRawImageNo(u64 imageNo) { _setLong(CONST_CAMERA_IMAGE_NO, imageNo); }

DataBuf SoatcarState::GetRawImage() { return _getData(CONST_CAMERA_IMAGE); }
void SoatcarState::SetRawImage(DataBuf data) { _setData(CONST_CAMERA_IMAGE, data); }

u64 SoatcarState::GetTreatedImageNo() { return _getLong(CONST_TREATED_IMAGE_NO); }
void SoatcarState::SetTreatedImageNo(u64 imageNo) { _setLong(CONST_TREATED_IMAGE_NO, imageNo); }

DataBuf SoatcarState::GetTreatedImage() { return _getData(CONST_TREATED_IMAGE); }
void SoatcarState::SetTreatedImage(DataBuf data) { _setData(CONST_TREATED_IMAGE, data); }


