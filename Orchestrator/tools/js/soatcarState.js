var fs = require('fs');
var mmap = require('./mmap-io/mmap-io');


class StateManager {

	initConstants() {							// Adresse de l'info dans le fichier partagé
		this.CONST_STOP_FLAG = 0;
		this.CONST_THROTTLE_VALUE = 150;
		this.CONST_IMAGE_NO = 230;
		this.CONST_CAMERA_IMAGE = 1048576;

		this.CONST_STEERING_INPUT
		this.CONST_THROTTLE_INPUT
		this.CONST_CONSTANT_THROTTLE_VALUE
		this.CONST_CONSTANT_THROTTLE_ACTIVE
		this.CONST_STEERING_AUTO
		this.CONST_THROTTLE_AUTO
		this.CONST_STEERING_OUTPUT
		this.CONST_THROTTLE_OUTPUT
		this.CONST_CPU_TEMP
		this.CONST_CPU_LOAD
		this.CONST_RAM_LOAD
		this.CONST_DISK_LOAD
		this.CONST_ULTRASONIC_DISTANCE
		this.CONST_SENSORS_READY
		this.CONST_ACTUATOR_READY
		this.CONST_JOYSTICK_READY
		this.CONST_CAMERA_READY
		this.CONST_AUTOPILOT_READY
		this.CONST_REMOTE_MONITOR_READY
		this.CONST_SENSORS_LOOP_RATE_HZ
		this.CONST_ACTUATOR_LOOP_RATE_HZ
		this.CONST_JOYSTICK_LOOP_RATE_HZ
		this.CONST_CAMERA_LOOP_RATE_HZ
		this.CONST_AUTOPILOT_LOOP_RATE_HZ
		this.CONST_REMOTE_MONITOR_LOOP_RATE_HZ
		this.CONST_HOSTNAME
		this.CONST_DRIVING_MODE
		this.CONST_IS_RECORDING
		this.CONST_RECORDING_PREFIX
		this.CONST_RECORDING_FOLDER
		this.CONST_RECORDING_IMAGE_INDEX
		this.CONST_MAX_THROTTLE_LIMIT
		this.
		this.
		;
	}

	constructor() {
		this.initConstants();
		var fd = fs.openSync("/var/tmp/soatcarmmf.tmp", "r+");
		var size = fs.statSync("/var/tmp/soatcarmmf.tmp").size;
		this.buffer = mmap.map(size, mmap.PROT_WRITE, mmap.MAP_SHARED, fd);
		fs.closeSync(fd);
		return 0;
	}

	// ---------------
	// PRIVATE METHODS
	// ---------------
	_getFlag(offset) {
		if(this.buffer[offset] != 0) return true;
		else return false;
	}

	_setFlag(offset, flag) {
		if(flag) this.buffer[offset] = 1;
		else this.buffer[offset] = 0;
	}

	_getInt(offset) {
		// Unsigned int does not exist so hope for int not to be too long
    	var res = (this.buffer[offset + 3] << 24) | (this.buffer[offset + 2] << 16) | (this.buffer[offset + 1] << 8) | this.buffer[offset + 0];
		return res >>> 0;
	}

	_setInt(offset, val) {
		this.buffer[offset + 3] = val >> 24 & 0xFF;
		this.buffer[offset + 2] = val >> 16 & 0xFF;
		this.buffer[offset + 1] = val >> 8 & 0xFF;
		this.buffer[offset + 0] = val >> 0 & 0xFF;
	}

	_getLong(offset) {
		// long does not exist in JS: cant use them, we need to output a couple of int
		var up = this._getInt(offset+4);
		var down = this._getInt(offset);
		return [up, down];
	}

	_setLong(offset, vals) {
		// long does not exist in JS: cant use them, we need a couple of int
		this._setInt(offset+4, vals[0]);
		this._setInt(offset, vals[1]);
	}

	_getData(offset) {
		// Get legnth of data
		var length = this._getInt(offset);
		var result = Buffer.alloc(length);
		for(var i = 0; i < length; i++)
		{
			result[i] = this.buffer[offset+4+i];
		}
		return result;
	}

	_setData(offset, abuf) {
		this._setInt(offset, abuf.byteLength);
		for(var i = 0; i < abuf.byteLength; i++)
        {
            this.buffer[offset+4+i] = abuf[i];
        }
	}



	// --------------
	// PUBLIC METHODS
	// --------------
	getStopFlag() {			return this._getFlag(this.CONST_STOP_FLAG);		}
	setStopFlag(flag) {		this._setFlag(this.CONST_STOP_FLAG, flag);		}

	getThrottle() {			return this._getInt(this.CONST_THROTTLE_VALUE);	}
	setThrottle(val) {		this._setInt(this.CONST_THROTTLE_VALUE, val);	}

	// returns a couple of int [up, down]
	getImageNo() {			return this._getLong(this.CONST_IMAGE_NO);		}
	// vals is a couple of int [up, down]
	setImageNo(vals) {		this._setLong(this.CONST_IMAGE_NO, vals);		}

	// Manual controls
	getSteeringInput() {				return this._getInt(this.CONST_STEERING_INPUT);	}
	setSteeringInput(val) {				this._setInt(this.CONST_STEERING_INPUT, val);	}
	getThrottleInput() {				return this._getInt(this.CONST_THROTTLE_INPUT);	}
	setThrottleInput(val) {				this._setInt(this.CONST_THROTTLE_INPUT, val);	}
	getConstantThrottleValue() {		return this._getInt(this.CONST_CONSTANT_THROTTLE_VALUE);	}
	setConstantThrottleValue(val) {		this._setInt(this.CONST_CONSTANT_THROTTLE_VALUE, val);	}
	getConstantThrottleActive() {		return this._getFlag(this.CONST_CONSTANT_THROTTLE_ACTIVE);	}
	setConstantThrottleActive(val) {	this._setFlag(this.CONST_CONSTANT_THROTTLE_ACTIVE, val);	}
	
	// Auto controls
	getSteeringAuto() {		return this._getInt(this.CONST_STEERING_AUTO);	}
	setSteeringAuto(val) {	this._setInt(this.CONST_STEERING_AUTO, val);	}
	getThrottleAuto() {		return this._getInt(this.CONST_THROTTLE_AUTO);	}
	setThrottleAuto(val) {	this._setInt(this.CONST_THROTTLE_AUTO, val);	}

	// Outputs controls
	getSteeringOutput() {		return this._getInt(this.CONST_STEERING_OUTPUT);	}
	setSteeringOutput(val) {	this._setInt(this.CONST_STEERING_OUTPUT, val);		}
	getThrottleOutput() {		return this._getInt(this.CONST_THROTTLE_OUTPUT);	}
	setThrottleOutput(val) {	this._setInt(this.CONST_THROTTLE_OUTPUT, val);	}

	// Sensors
	getCpuTemp() {					return this._getInt(this.CONST_CPU_TEMP);			}
	setCpuTemp(val) {				this._setInt(this.CONST_CPU_TEMP, val);				}
	getCpuLoad() {					return this._getInt(this.CONST_CPU_LOAD);			}
	setCpuLoad(val) {				this._setInt(this.CONST_CPU_LOAD, val);				}
	getRamLoad() {					return this._getInt(this.CONST_RAM_LOAD);			}
	setRamLoad(val) {				this._setInt(this.CONST_RAM_LOAD, val);				}
	getDiskLoad() {					return this._getInt(this.CONST_DISK_LOAD);			}
	setDiskLoad(val) {				this._setInt(this.CONST_DISK_LOAD, val);			}
	getUltrasonicDistance() {		return this._getInt(this.CONST_ULTRASONIC_DISTANCE);}
	setUltrasonicDistance(val) {	this._setInt(this.CONST_ULTRASONIC_DISTANCE, val);	}

	// Status & Parts
	getSensorsReady() {				return this._getFlag(this.CONST_SENSORS_READY);			}
	getActuatorReady() {			return this._getFlag(this.CONST_ACTUATOR_READY);		}
	getJoystickReady() {			return this._getFlag(this.CONST_JOYSTICK_READY);		}
	getCameraReady() {				return this._getFlag(this.CONST_CAMERA_READY);			}
	getAutoPilotReady() {			return this._getFlag(this.CONST_AUTOPILOT_READY);		}
	getRemoteMonitorReady() {		return this._getFlag(this.CONST_REMOTE_MONITOR_READY);	}
	setRemoteMonitorReady(val) {	this._setFlag(this.CONST_REMOTE_MONITOR_READY, val);	}
	//
	getSensorsLoopRate() { 			return this._getInt(this.CONST_SENSORS_LOOP_RATE_HZ);			}
	getActuatorLoopRate() { 		return this._getInt(this.CONST_ACTUATOR_LOOP_RATE_HZ); 			}
	getJoystickLoopRate() { 		return this._getInt(this.CONST_JOYSTICK_LOOP_RATE_HZ); 			}
	getCameraLoopRate() { 			return this._getInt(this.CONST_CAMERA_LOOP_RATE_HZ); 			}
	getAutoPilotLoopRate() { 		return this._getInt(this.CONST_AUTOPILOT_LOOP_RATE_HZ); 		}
	getRemoteMonitorLoopRate() { 	return this._getInt(this.CONST_REMOTE_MONITOR_LOOP_RATE_HZ);	}
	setRemoteMonitorLoopRate(val) { this._setInt(this.CONST_REMOTE_MONITOR_LOOP_RATE_HZ); 			}

	// Parameters
	getHostname() { 			return this._getData(this.CONST_HOSTNAME); 				}
	getDrivingMode() { 			return this._getInt(this.CONST_DRIVING_MODE); 			}
	setDrivingMode(val) { 		this._setInt(this.CONST_DRIVING_MODE, val); 			}
	getIsRecording() { 			return this._getFlag(this.CONST_IS_RECORDING); 			}
	setIsRecording(val) { 		this._setFlag(this.CONST_IS_RECORDING, val); 			}
	getRecordingPrefix() { 		return this._getData(this.CONST_RECORDING_PREFIX); 		}
	getRecordingFolder() { 		return this._getData(this.CONST_RECORDING_FOLDER); 		}
	getRecordingImageIndex() { 	return this._getInt(this.CONST_RECORDING_IMAGE_INDEX); 	}
	getMaxThrottleLimit() { 	return this._getInt(this.CONST_MAX_THROTTLE_LIMIT); 	}
	setMaxThrottleLimit(val) { 	this._setInt(this.CONST_MAX_THROTTLE_LIMIT); 			}
	getStopFlag() {				return this._getFlag(this.CONST_STOP_FLAG);				}
	setStopFlag(flag) {			this._setFlag(this.CONST_STOP_FLAG, flag);				}

	// return an ArrayBuffer
	getImage() {			return this._getData(this.CONST_CAMERA_IMAGE);	}
	// takes an ArrayBuffer as argument
	setImage(aBuf) {		this._setData(this.CONST_CAMERA_IMAGE, aBuf);	}
}




