import mmap, struct, time, binascii

start = time.time()

with open("/var/tmp/soatcarmmf.tmp", "r+") as f:
    # memory-map the file, size 0 means whole file
    mm = mmap.mmap(f.fileno(), 0)
    # read state
    flag = struct.unpack('B', mm[0])[0]
    print "Stop flag {}".format(flag)
    speed = struct.unpack('I', mm[150:154])[0]
    print "speed {}".format(speed)
    # image
    no = struct.unpack('Q', mm[230:238])[0]
    print "ImageNo {}".format(no)
    length = struct.unpack('I', mm[1048576:1048580])[0]
    sl = slice(1048580,1048580+length)
    imagePath = mm[sl]
    print "image of length {}:\n{}\n".format(len(imagePath), binascii.hexlify(imagePath))
    # close the map
    mm.close()

end = time.time()
print end - start
