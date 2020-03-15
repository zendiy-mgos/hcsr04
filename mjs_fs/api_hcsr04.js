// HC-SR04 Driver. Source C API is defined at:
// [mgos_hcsr04.h](https://github.com/zendiy-mgos/hcsr04/blob/master/include/mgos_hcsr04.h)

let HCSR04 = {
  _crt: ffi('void *mgos_hcsr04_create(int, int)'),
  _cls: ffi('void mgos_hcsr04_close(void *)'),
  _ge: ffi('int mgos_hcsr04_get_echo(void *)'),
  _gd: ffi('float mgos_hcsr04_get_distance(void *)'),
  _gdt: ffi('float mgos_hcsr04_get_distance_t(void *, float)'),
  _gdavg: ffi('float mgos_hcsr04_get_distance_avg(void *, int, int)'),
  _gdavgt: ffi('float mgos_hcsr04_get_distance_avg_t(void *, int, int, float)'),

  // **`HCSR04.create(trig_pin, echo_pin)`**
  // Create a HC-SR04 sensor instance.
  // Return value: an object with the methods described below, or
  // 'null' in case of a failure.
  // Example:
  // ```javascript
  // let sensor = HCSR04.create(2, 4);
  // let d = sensor.getDistance();
  // print('Distance:', (isNaN(d) ? 'error reading distance' : d));
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

    // **`sensor.getEcho()`**
    // Return echo duration in microseconds or -1 on failure.
    getEcho: function() {
      return HCSR04._ge(this.handle);
    },

    // **`sensor.getDistance(temperature)`**
    // Return distance in millimiters or 'NaN' in case of a failure.
    getDistance: function(temperature) {
      if (temperature) {
        return HCSR04._gdt(this.handle, temperature);
      }
      return HCSR04._gd(this.handle);
    },

    // **`sensor.getAvgDistance(attemptsCount, attemptsDelay)`**
    // Perform multiple measurement attempts and return the average
    // distance (in millimiters) or 'NaN' in case of a failure.
    // If attemptsDelay is 0(zero) or not specified, the default
    // 5ms delay value is used. 
    getAvgDistance: function(attemptsCount, attemptsDelay, temperature) {
      if (temperature) {
        return HCSR04._gdavgt(this.handle,
                              (attemptsCount || 0),
                              (attemptsDelay || -1),
                              temperature);
      }
      return HCSR04._gdavg(this.handle,
                           (attemptsCount || 0),
                           (attemptsDelay || -1));
    },
  },
};