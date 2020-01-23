# HC-SR04 Driver
## Overview
A Mongoose OS library for HC-SR04, a popular ultrasonic distance sensor.
# GET STARTED
Build up your device in few minutes just starting from one of the following samples.

|Sample|Notes|
|--|--|
|[hcsr04-demo](https://github.com/zendiy-mgos/hcsr04-demo)|It shows you how to build a firmware for measuring distances with a HC-SR04 sensor.|
# Usage
Include the library into your `mos.yml` file.
```yaml
libs:
  - origin: https://github.com/zendiy-mgos/hcsr04
```
If you are developing a JavaScript firmware, load `api_hcsr04.js` in your `init.js` file.
```js
load('api_hcsr04.js');
```
