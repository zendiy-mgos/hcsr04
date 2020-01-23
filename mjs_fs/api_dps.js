// HC-SR04 Driver. Source C API is defined at:
// [mgos_hcsr04.h](https://github.com/?/mgos_hcsr04/tree/master/src/mgos_hcsr04.h)

let HCSR04 = {
  _crt: ffi('void *mgos_hcsr04_create(int, int)'),
  _cls: ffi('void mgos_hcsr04_close(void *)'),
  _gd: ffi('float mgos_hcsr04_get_distance(void *)'),

  // **`HCSR04.create(trig_pin, echo_pin)`**
  // Create a HC-SR04 sensor instance.
  // Return value: an object with the methods described below, or
  // 'null' in case of a failure.
  // Example:
  // ```javascript
  // let sensor = HCSR04.create(2, 4);
  // let d = sensor.getDistance();
  // print('Distance: ', (isNaN(d) ? 'error reading distance' : d));
  // ```
  create: function(trig_pin, echo_pin) {
    let handle = HCSR04._crt(trig_pin, echo_pin);
    if (!handle) return null; 
    let obj = Object.create(HCSR04._proto);
    obj.handle = handle;
    return obj;
  },

  _proto: {
    handle: null,
    // **`sensor.close()`**
    // Close HC-SR04 handle. Return value: none.
    close: function() {
      return HCSR04._cls(this.handle);
    },

    // **`sensor.getDistance()`**
    // Return distance in millimiters or 'NaN' in case of a failure.
    getDistance: function() {
      return HCSR04._gd(this.handle);
    },
  },
};