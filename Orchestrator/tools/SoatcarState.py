import mmap, struct

class SoatcarState:
    # 3Mo to share : 1 Mo = 1 048 576 bytes for flags and int values, 1MB for raw image, 1MB for filtered image
    # Configuration flags and values
    CONST_STOP_FLAG						= 0		# byte
    CONST_THROTTLE_AUTO					= 10	# byte
    CONST_THROTTLE_MAX_PCT				= 15	# int
    CONST_STEERING_AUTO					= 20	# byte
    
    # Parts ready and loop rates
    CONST_CAMERA_READY					= 30	# byte
    CONST_CAMERA_LOOP_RATE				= 40	# int
    CONST_AUTO_PILOT_READY				= 50	# byte
    CONST_AUTO_PILOT_LOOP_RATE			= 60	# int
    CONST_JOYSTICK_READY				= 70	# byte
    CONST_JOYSTICK_LOOP_RATE			= 80	# int
    CONST_ACTUATOR_READY				= 90	# byte
    CONST_ACTUATOR_LOOP_RATE			= 100	# int
    CONST_ULTRASONIC_READY				= 110	# byte
    CONST_ULTRASONIC_LOOP_RATE			= 120	# int
    CONST_WEBSERVER_READY				= 130	# byte
    CONST_WEBSERVER_LOOP_RATE			= 140	# int

    # Throttle and steering values required between 0 and 100000
    CONST_THROTTLE_VALUE				= 150	# int
    CONST_STEERING_VALUE				= 160	# int

    # state variables
    CONST_ULTRASONIC_DISTANCE			= 170	# int
    CONST_CPU_TEMP						= 180	# int
    CONST_CPU_LOAD						= 190	# int
    CONST_LOAD_AVERAGE					= 200	# int
    CONST_MEMORY_LOAD					= 210	# int
    CONST_DISK_LOAD						= 220	# int

    # Images
    CONST_CAMERA_IMAGE					= 1048576 # int for the size followed by the image itself
    CONST_CAMERA_TREATED_IMAGE			= 2097152 # int for the size followed by the image itself
    
    
    # Constructeur
    def __init__(self):
        # init mmap file
        fd=open("/var/tmp/soatcarmmf.tmp", "r+")
        # memory-map the file, size 0 means whole file
        self.mm = mmap.mmap(fd.fileno(), 0)
        # we can close the file descriptor as soon as we have opened the MMF
        fd.close()
    
    # destructeur
    def __del__(self):
        # close the map
        self.mm.close()
    
    
    # methodes utilitaires
    def _getFlag(self, offset):
        if self.mm[offset] == '\x00':
            return False
        else:
            return True
    
    def _setFlag(self, offset, flag):
        if flag:
            self.mm[offset] = '\x01'
        else:
            self.mm[offset] = '\x00'
    
    def _getInt(self, offset):
        sl = slice(offset,offset+4)
        return struct.unpack('I', self.mm[sl])[0]
    
    def _setInt(self, offset, val):
        self.mm.seek(offset)
        self.mm.write(struct.pack('I', val))
    
    # this returns bytes
    def _getArray(self, offset):
        sl = slice(offset,offset+4)
        length = struct.unpack('I', self.mm[sl])[0]
        sl = slice(offset+4,offset+4+length)
        return self.mm[sl]
    
    # data should be bytes: bytes(string) or bytes read from a file for example
    def _setArray(self, offset, data):
        self.mm.seek(offset)
        self.mm.write(struct.pack('I', len(data)))
        self.mm.write(data)
    
    
    # methodes publiques
    def getStopFlag(self):
        return self._getFlag(self.CONST_STOP_FLAG)
    
    def setStopFlag(self, flag):
        self._setFlag(self.CONST_STOP_FLAG, flag)
    
    def getThrottle(self):
        return self._getInt(self.CONST_THROTTLE_VALUE)
    
    def setThrottle(self, value):
        self._setInt(self.CONST_THROTTLE_VALUE, value)
    
    def getImage(self):
        return self._getArray(self.CONST_CAMERA_IMAGE)
    
    def setImage(self, data):
        self._setArray(self.CONST_CAMERA_IMAGE, data)
