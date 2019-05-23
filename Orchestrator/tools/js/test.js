
var state = require('./soatcarState.js');

// TESTS
var instance = new state.StateManager();

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

