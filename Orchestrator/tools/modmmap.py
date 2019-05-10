import mmap, struct, time
from random import randint

start = time.time()

with open("/var/tmp/soatcarmmf.tmp", "r+") as f:
    # memory-map the file, size 0 means whole file
    mm = mmap.mmap(f.fileno(), 0)
    for x in range(0, 1000):
        outputSteering = randint(0,1000)
        outputThrottle = randint(0,1000)
        mm.seek(0)
        mm.write(struct.pack('I', outputSteering))
        mm.seek(10)
        mm.write(struct.pack('I', outputThrottle))
        mm.seek(30)
        mm.write(struct.pack('I', x))
        time.sleep(0.1)
    # close the map
    mm.close()

end = time.time()
print end - start
print "{} req/s".format(6*10000 / (end - start))
