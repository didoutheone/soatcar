#ifndef DEF_SOATCARSTATE
#define DEF_SOATCARSTATE

#include <stdbool.h>

// 3Mo to share : 1 Mo = 1 048 576 bytes for flags and int values, 1MB for raw image, 1MB for filtered image
// Configuration flags and values
#define CONST_STOP_FLAG						0	// byte
#define CONST_THROTTLE_AUTO					10	// byte
#define CONST_THROTTLE_MAX_PCT				15	// int
#define CONST_STEERING_AUTO					20	// byte

// Parts ready and loop rates
#define CONST_CAMERA_READY					30	// byte
#define CONST_CAMERA_LOOP_RATE				40	// int
#define CONST_AUTO_PILOT_READY				50	// byte
#define CONST_AUTO_PILOT_LOOP_RATE			60	// int
#define CONST_JOYSTICK_READY				70	// byte
#define CONST_JOYSTICK_LOOP_RATE			80	// int
#define CONST_ACTUATOR_READY				90	// byte
#define CONST_ACTUATOR_LOOP_RATE			100	// int
#define CONST_ULTRASONIC_READY				110	// byte
#define CONST_ULTRASONIC_LOOP_RATE			120	// int
#define CONST_WEBSERVER_READY				130	// byte
#define CONST_WEBSERVER_LOOP_RATE			140	// int

// Throttle and steering values required between 0 and 100000
#define CONST_THROTTLE_VALUE				150	// int
#define CONST_STEERING_VALUE				160	// int

// sensor variables
#define CONST_ULTRASONIC_DISTANCE			170	// int
#define CONST_CPU_TEMP						180	// int
#define CONST_CPU_LOAD						190	// int
#define CONST_LOAD_AVERAGE					200	// int
#define CONST_MEMORY_LOAD					210	// int
#define CONST_DISK_LOAD						220	// int

// Images
#define CONST_IMAGE_NO						230	// long
#define CONST_TREATED_IMAGE_NO				240	// long
#define CONST_CAMERA_IMAGE					1048576 // int for the size followed by the image itself
#define CONST_CAMERA_TREATED_IMAGE			2097152 // int for the size followed by the image itself


#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

struct DataBuf
{
    u32 length;		// data length
    u8* buffer; 	// data
};
typedef struct DataBuf DataBuf;

class SoatcarState
{
	private:
	// MMap buffer
	u8* _mmap;
	// Retrieve a bool from the state
	bool _getFlag(int offset);
	// Set a bool in the state
	void _setFlag(int offset, bool flag);
	// Retrieve an int from the state
	u32 _getInt(int offset);
	// Set an int in the state
	void _setInt(int offset, u32 val);
	// Retrieve a long from the state
	u64 _getLong(int offset);
	// Set a long in the state
	void _setLong(int offset, u64 val);
	// Retrieve a string from the state
	char* _getString(int offset);
	// Set a string in the state
	void _setString(int offset, char* str);
	// Retrieve some data from the state
	DataBuf _getData(int offset);
	// Set some data in the state
	void _setData(int offset, DataBuf data);

	public:
	// ctor: give it the memory map file full path
	SoatcarState(const char* filename);
	
	// Transform source value in source range to destination value in destination range. May be inverted.
	double Translate(double fromMin, double fromMax, double toMin, double toMax, double value, bool inverted);
	
	// State elements retrieval methods
	bool GetStopFlag();
	void SetStopFlag(bool flag);

	bool IsThrottleAuto();
	void SetThrottleAuto(bool flag);

	bool IsSteeringAuto();
	void SetSteeringAuto(bool flag);

	u32 GetThrottleMaxPct();
	void SetThrottleMaxPct(u32 value);


	bool IsCameraReady();
	void SetCameraReady(bool flag);

	u32 GetCameraLoopRate();
	void SetCameraLoopRate(u32 value);

	bool IsAutoPilotReady();
	void SetAutoPilotReady(bool flag);

	u32 GetAutoPilotLoopRate();
	void SetAutoPilotLoopRate(u32 value);

	bool IsJoystickReady();
	void SetJoystickReady(bool flag);

	u32 GetJoystickLoopRate();
	void SetJoystickLoopRate(u32 value);

	bool IsActuatorReady();
	void SetActuatorReady(bool flag);

	u32 GetActuatorLoopRate();
	void SetActuatorLoopRate(u32 value);

	bool IsUltrasonicReady();
	void SetUltrasonicReady(bool flag);

	u32 GetUltrasonicLoopRate();
	void SetUltrasonicLoopRate(u32 value);

	bool IsWebServerReady();
	void SetWebServerReady(bool flag);

	u32 GetWebServerLoopRate();
	void SetWebServerLoopRate(u32 value);


	u32 GetThrottleValue();
	void SetThrottleValue(u32 value);

	u32 GetSteeringValue();
	void SetSteeringValue(u32 value);


	u32 GetUltrasonicDistance();
	void SetUltrasonicDistance(u32 value);

	u32 GetCPUTemp();
	void SetCPUTemp(u32 value);

	u32 GetCPULoad();
	void SetCPULoad(u32 value);

	u32 GetLoadAverage();
	void SetLoadAverage(u32 value);

	u32 GetMemoryLoad();
	void SetMemoryLoad(u32 value);

	u32 GetDiskLoad();
	void SetDiskLoad(u32 value);


	u64 GetImageNo();
	void SetImageNo(u64 imageNo);

	DataBuf GetImage();
	void SetImage(DataBuf data);

	u64 GetTreatedImageNo();
	void SetTreatedImageNo(u64 imageNo);

	DataBuf GetTreatedImage();
	void SetTreatedImage(DataBuf data);

};
#endif