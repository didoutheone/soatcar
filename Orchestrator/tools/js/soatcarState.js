var fs = require('fs');
var mmap = require('./mmap-io/mmap-io');


class StateManager {

	initConstants() {
		this.CONST_STOP_FLAG = 0;
		this.CONST_THROTTLE_VALUE = 150;
		this.CONST_IMAGE_NO = 230;
		this.CONST_CAMERA_IMAGE = 1048576;
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
	getStopFlag() {
		return this._getFlag(this.CONST_STOP_FLAG);
	}

	setStopFlag(flag) {
		this._setFlag(this.CONST_STOP_FLAG, flag);
	}

	getThrottle() {
		return this._getInt(this.CONST_THROTTLE_VALUE);
	}

	setThrottle(val) {
		this._setInt(this.CONST_THROTTLE_VALUE, val);
	}

	// returns a couple of int [up, down]
	getImageNo() {
		return this._getLong(this.CONST_IMAGE_NO);
	}

	// vals is a couple of int [up, down]
	setImageNo(vals) {
		this._setLong(this.CONST_IMAGE_NO, vals);
	}

	// return an ArrayBuffer
	getImage() {
		return this._getData(this.CONST_CAMERA_IMAGE);
	}

	// takes an ArrayBuffer as argument
	setImage(aBuf) {
		this._setData(this.CONST_CAMERA_IMAGE, aBuf);
	}
}



// TESTS
var instance = new StateManager();

console.log("Set Stop flag to " + false),
instance.setStopFlag(false);
stopFlag = instance.getStopFlag();
console.log("Stop flag is: " + stopFlag);

console.log("Set Stop flag to " + true),
instance.setStopFlag(true);
stopFlag = instance.getStopFlag();
console.log("Stop flag is: " + stopFlag);

console.log("Set Throttle to " + 123456),
instance.setThrottle(123456);
throttle = instance.getThrottle();
console.log("Throttle is: " + throttle);

console.log("Set ImageNo to 9223372036854775800 that is not representable in JS, so put 4294967288 in first four bytes and 2147483647 in last four bytes");
vals = [2147483647,4294967288];
instance.setImageNo(vals);
imgNo = instance.getImageNo();
console.log("ImageNo is: " + imgNo + " that means: " + imgNo[0].toString(16) + " " + imgNo[1].toString(16));


file = fs.readFileSync("testfile");
console.log("Set image to " + JSON.stringify(file));
instance.setImage(file);
readfile = instance.getImage();
console.log("Now image is: " + JSON.stringify(readfile));



