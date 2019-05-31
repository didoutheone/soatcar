#ifndef DEF_SOATCARSTATE
#define DEF_SOATCARSTATE

#include <stdbool.h>

#define MIN_STATE_FIELDS_VALUE 0
#define MAX_STATE_FIELDS_VALUE 10000

// 3Mo to share : 1 Mo = 1 048 576 bytes for flags and int values splitted by part, 1MB for raw image, 1MB for filtered image
    
// Common fields
#define CONST_STOP_FLAG						 0		// byte - Si cette valeur passe à true (1), toutes les parts doivent s'arrêter
#define CONST_IS_STEERING_AUTO				 10		// byte - La voiture gère t'elle la direction en mode autonome ?
#define CONST_IS_THROTTLE_AUTO				 20		// byte - La voiture gère t'elle la vitesse en mode autonome ?
#define CONST_MAX_THROTTLE_LIMIT			 30		// int - Valeur maximale de throttle admise dans la plage [0 - 10000]
#define CONST_IS_CONSTANT_THROTTLE_ACTIVE	 40		// byte - La vitesse est-elle fixée / constante ?
#define CONST_CONSTANT_THROTTLE_VALUE		 50		// int - Fixer une valeur de vitesse constante dans la plage [0 - 10000]
#define CONST_HOSTNAME						 60		// string - hostname servant d'identifiant de la soatcar / l'ordi qui héberge le code

// Joystick
#define CONST_IS_JOYSTICK_READY				 10000	// byte - La part Joystick est-elle prête ?
#define CONST_JOYSTICK_LOOP_RATE			 10010	// int - Fréquence en Hz de la boucle de traitement des inputs du joystick
#define CONST_JOYSTICK_STEERING_VALUE        10020	// int - Valeur de steering en entrée manuelle dans la plage [0 - 10000]
#define CONST_JOYSTICK_THROTTLE_VALUE		 10030	// int - Valeur de throttle en entrée manuelle dans la plage [0 - 10000]

// PWM
#define CONST_IS_ACTUATOR_READY				 20000	// byte - La part Actuator/PWM est-elle prête ?
#define CONST_ACTUATOR_LOOP_RATE			 20010	// int - Fréquence en Hz de la boucle de traitement des Actuator/PWM
#define CONST_STEERING_OUTPUT				 20020	// int - La valeur de steering réellement utilisée pour le PWM de direction dans la plage [0 - 10000]
#define CONST_THROTTLE_OUTPUT				 20030	// int - La valeur de throttle réellement utilisée pour le PWM de vitesse dans la plage [0 - 10000]

// System sensors
#define CONST_IS_SYSTEM_SENSOR_READY		 30000	// byte - La part System Sensor (CPU load, RAM, ...) est-elle prête ?
#define CONST_SYSTEM_SENSOR_LOOP_RATE		 30010	// int - Fréquence en Hz de la boucle de traitement des Sensors système
#define CONST_CPU_TEMP						 30020	// int - Température du CPU
#define CONST_CPU_LOAD						 30030	// int - Pourcentage de charge du CPU
#define CONST_RAM_LOAD						 30040	// int - Pourcentage de charge de la RAM
#define CONST_DISK_LOAD						 30050	// int - Pourcentage d'utilisation de l'espace disque
#define CONST_LOAD_AVERAGE					 30060	// int - Load average x 100 : cf top

// Physical sensors
#define CONST_IS_PHYSICAL_SENSOR_READY		 40000	// byte - La part physical Sensor (Ultrasons, ...) est-elle prête ?
#define CONST_PHYSICAL_SENSOR_LOOP_RATE		 40010	// int - Fréquence en Hz de la boucle de traitement des Sensors physiques
#define CONST_ULTRASONIC_DISTANCE			 40020	// int - Distance en mm détectée par le détecteur à ultrasons

// Auto pilot
#define CONST_IS_AUTOPILOT_READY			 50000	// byte - La part d'auto pilote est-elle prête ?
#define CONST_AUTOPILOT_LOOP_RATE			 50010	// int - Fréquence en Hz de la boucle d'auto pilotage
#define CONST_AUTO_STEERING_VALUE			 50020	// int - Valeur de steering en mode automatique dans la plage [0 - 10000]
#define CONST_AUTO_THROTTLE_VALUE			 50030	// int - Valeur de throttle en mode automatique dans la plage [0 - 10000]

// Caméra
#define CONST_IS_CAMERA_READY				 60000	// byte - La part caméra est-elle prête ?
#define CONST_CAMERA_LOOP_RATE				 60010	// int - Fréquence en Hz de la boucle de récupération d'images par la caméra
#define CONST_IS_RECORDING					 60020	// byte - Booléen indiquant si la voiture est en train d'enregistrer des immages
#define CONST_RECORDING_PREFIX				 60030	// string - Chaine de caractères qui sera le préfixe des images enregistrées
#define CONST_RECORDING_FOLDER				 60040	// string - Chaine de caractère indiquant le répertoire local dans lequel enregistrer les images
#define CONST_RECORDING_IMAGE_INDEX			 60050	// int - Entier indiquant le numéro de l'image enregistrée, au sein de l'enregistrement en cours

// Remote monitoring web server
#define CONST_IS_REMOTE_MONITORING_READY	 70000	// byte - La part remote monitoring est-elle prête ?
#define CONST_REMOTE_MONITORING_LOOP_RATE	 70010	// int - Fréquence en Hz de la boucle de récupération d'images par la caméra

// Led management
#define CONST_IS_LED_MANAGER_READY			 80000	// byte - La part de gestion des LEDs est-elle prête ?
#define CONST_LED_MANAGER_LOOP_RATE			 80010	// int - Fréquence en Hz de la boucle de gestion des LEDs

// Images
#define CONST_CAMERA_IMAGE_NO				 1000000	// int - Image index
#define CONST_CAMERA_IMAGE					 1000010	// int - Raw image
#define CONST_TREATED_IMAGE_NO		 		 2000000	// int - Treated image index
#define CONST_TREATED_IMAGE			 		 2000010	// int - Treated image



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
	
	//
	// State elements retrieval methods
	//
	
	// Common fields
	bool GetStopFlag();
	void SetStopFlag(bool flag);

	bool IsSteeringAuto();
	void SetSteeringAuto(bool flag);

	bool IsThrottleAuto();
	void SetThrottleAuto(bool flag);

	u32 GetMaxThrottleLimit();
	void SetMaxThrottleLimit(u32 value);

	bool IsConstantThrottleActive();
	void SetConstantThrottleActive(bool flag);

	u32 GetConstantThrottleValue();
	void SetConstantThrottleValue(u32 value);

	char* GetHostName();
	void SetHostName(char* value);


	// Joystick fields
	bool IsJoystickReady();
	void SetJoystickReady(bool flag);

	u32 GetJoystickLoopRate();
	void SetJoystickLoopRate(u32 value);

	u32 GetJoystickSteeringValue();
	void SetJoystickSteeringValue(u32 value);

	u32 GetJoystickThrottleValue();
	void SetJoystickThrottleValue(u32 value);


	// PWM part
	bool IsActuatorReady();
	void SetActuatorReady(bool flag);

	u32 GetActuatorLoopRate();
	void SetActuatorLoopRate(u32 value);

	u32 GetSteeringOutput();
	void SetSteeringOutput(u32 value);

	u32 GetThrottleOutput();
	void SetThrottleOutput(u32 value);


	// System sensors
	bool IsSystemSensorReady();
	void SetSystemSensorReady(bool flag);

	u32 GetSystemSensorLoopRate();
	void SetSystemSensorLoopRate(u32 value);

	u32 GetCPUTemp();
	void SetCPUTemp(u32 value);

	u32 GetCPULoad();
	void SetCPULoad(u32 value);

	u32 GetMemoryLoad();
	void SetMemoryLoad(u32 value);

	u32 GetDiskLoad();
	void SetDiskLoad(u32 value);

	u32 GetLoadAverage();
	void SetLoadAverage(u32 value);


	// Physical sensors
	bool IsPhysicalSensorReady();
	void SetPhysicalSensorReady(bool flag);

	u32 GetPhysicalSensorLoopRate();
	void SetPhysicalSensorLoopRate(u32 value);

	u32 GetUltrasonicDistance();
	void SetUltrasonicDistance(u32 value);


	// Auto pilot
	bool IsAutoPilotReady();
	void SetAutoPilotReady(bool flag);

	u32 GetAutoPilotLoopRate();
	void SetAutoPilotLoopRate(u32 value);

	u32 GetAutoPilotSteeringValue();
	void SetAutoPilotSteeringValue(u32 value);

	u32 GetAutoPilotThrottleValue();
	void SetAutoPilotThrottleValue(u32 value);
	
	
	// Camera
	bool IsCameraReady();
	void SetCameraReady(bool flag);

	u32 GetCameraLoopRate();
	void SetCameraLoopRate(u32 value);

	bool IsRecording();
	void SetRecording(bool flag);

	char* GetRecordingPrefix();
	void SetRecordingPrefix(char* value);

	char* GetRecordingFolder();
	void SetRecordingFolder(char* value);

	u32 GetRecordingImageIndex();
	void SetRecordingImageIndex(u32 value);


	// Remote monitoring web server
	bool IsRemoteMonitoringReady();
	void SetRemoteMonitoringReady(bool flag);

	u32 GetRemoteMonitoringLoopRate();
	void SetRemoteMonitoringLoopRate(u32 value);

	
	// LED Management
	bool IsLedManagementReady();
	void SetLedManagementReady(bool flag);

	u32 GetLedManagementLoopRate();
	void SetLedManagementLoopRate(u32 value);


	// Images
	u64 GetRawImageNo();
	void SetRawImageNo(u64 imageNo);

	DataBuf GetRawImage();
	void SetRawImage(DataBuf data);

	u64 GetTreatedImageNo();
	void SetTreatedImageNo(u64 imageNo);

	DataBuf GetTreatedImage();
	void SetTreatedImage(DataBuf data);

};
#endif