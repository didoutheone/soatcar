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
		double translate(double fromMin, double fromMax, double toMin, double toMax, double value, bool inverted);
		
		//
		// State elements retrieval methods
		//
		
		// Common fields
		bool getStopFlag();
		void setStopFlag(bool flag);

		bool isSteeringAuto();
		void setSteeringAuto(bool flag);

		bool isThrottleAuto();
		void setThrottleAuto(bool flag);

		u32 getMaxThrottleLimit();
		void setMaxThrottleLimit(u32 value);

		bool isConstantThrottleActive();
		void setConstantThrottleActive(bool flag);

		u32 getConstantThrottleValue();
		void setConstantThrottleValue(u32 value);

		char* getHostName();
		void setHostName(char* value);


		// Joystick fields
		bool isJoystickReady();
		void setJoystickReady(bool flag);

		u32 getJoystickLoopRate();
		void setJoystickLoopRate(u32 value);

		u32 getJoystickSteeringValue();
		void setJoystickSteeringValue(u32 value);

		u32 getJoystickThrottleValue();
		void setJoystickThrottleValue(u32 value);


		// PWM part
		bool isActuatorReady();
		void setActuatorReady(bool flag);

		u32 getActuatorLoopRate();
		void setActuatorLoopRate(u32 value);

		u32 getSteeringOutput();
		void setSteeringOutput(u32 value);

		u32 getThrottleOutput();
		void setThrottleOutput(u32 value);


		// System sensors
		bool isSystemSensorReady();
		void setSystemSensorReady(bool flag);

		u32 getSystemSensorLoopRate();
		void setSystemSensorLoopRate(u32 value);

		u32 getCPUTemp();
		void setCPUTemp(u32 value);

		u32 getCPULoad();
		void setCPULoad(u32 value);

		u32 getMemoryLoad();
		void setMemoryLoad(u32 value);

		u32 getDiskLoad();
		void setDiskLoad(u32 value);

		u32 getLoadAverage();
		void setLoadAverage(u32 value);


		// Physical sensors
		bool isPhysicalSensorReady();
		void setPhysicalSensorReady(bool flag);

		u32 getPhysicalSensorLoopRate();
		void setPhysicalSensorLoopRate(u32 value);

		u32 getUltrasonicDistance();
		void setUltrasonicDistance(u32 value);


		// Auto pilot
		bool isAutoPilotReady();
		void setAutoPilotReady(bool flag);

		u32 getAutoPilotLoopRate();
		void setAutoPilotLoopRate(u32 value);

		u32 getAutoPilotSteeringValue();
		void setAutoPilotSteeringValue(u32 value);

		u32 getAutoPilotThrottleValue();
		void setAutoPilotThrottleValue(u32 value);
		
		
		// Camera
		bool isCameraReady();
		void setCameraReady(bool flag);

		u32 getCameraLoopRate();
		void setCameraLoopRate(u32 value);

		bool isRecording();
		void setRecording(bool flag);

		char* getRecordingPrefix();
		void setRecordingPrefix(char* value);

		char* getRecordingFolder();
		void setRecordingFolder(char* value);

		u32 getRecordingImageIndex();
		void setRecordingImageIndex(u32 value);


		// Remote monitoring web server
		bool isRemoteMonitoringReady();
		void setRemoteMonitoringReady(bool flag);

		u32 getRemoteMonitoringLoopRate();
		void setRemoteMonitoringLoopRate(u32 value);

		
		// LED Management
		bool isLedManagementReady();
		void setLedManagementReady(bool flag);

		u32 getLedManagementLoopRate();
		void setLedManagementLoopRate(u32 value);


		// Images
		u64 getRawImageNo();
		void setRawImageNo(u64 imageNo);

		DataBuf getRawImage();
		void setRawImage(DataBuf data);

		u64 getTreatedImageNo();
		void setTreatedImageNo(u64 imageNo);

		DataBuf getTreatedImage();
		void setTreatedImage(DataBuf data);

};
#endif