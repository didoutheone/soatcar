import SoatcarState
import binascii

# Initialisation
scState = SoatcarState.SoatcarState()

print("Set Stop flag to {}".format(False))
scState.setStopFlag(False)
stopFlag=scState.getStopFlag()
print("Stop flag is {}".format(stopFlag))

print("Set Stop flag to {}".format(True))
scState.setStopFlag(True)
stopFlag=scState.getStopFlag()
print("Stop flag is {}".format(stopFlag))

print("Set Throttle to {}".format(1566))
scState.setThrottle(1566)
throttle=scState.getThrottle()
print("Throttle is {}".format(throttle))

print("Set ImageNo to {}".format(9223372036854775800))
scState.setImageNo(9223372036854775800)
imgNo=scState.getImageNo()
print("ImageNo is {}".format(imgNo))


string = "Python is interesting."
arr = bytes(string)
print("Set Image to {}".format(arr))
scState.setImage(arr)
image=scState.getImage()
print("Image is {}".format(image))


f=open("testfile","rb")
contents=f.read()
f.close()
print("Set Image to:\n{}\nAnd len is: {}".format(binascii.hexlify(contents), len(contents)))
scState.setImage(contents)
image2=scState.getImage()
print("\n\n\nImage is:\n{}\nAnd len is {}".format(binascii.hexlify(image2), len(image2)))


