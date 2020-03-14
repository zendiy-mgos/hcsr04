#include "mgos.h"
#include "mgos_hcsr04.h"

struct mgos_hcsr04 {  
  int trig_pin;
  int echo_pin;
};

/* static inline uint64_t hcsr04_uptime() {
  return (uint64_t)(1000000 * mgos_uptime());
}

unsigned long hcsr04_pulse_in(uint8_t pin, uint8_t state, unsigned long timeout) {
  uint64_t start_us = hcsr04_uptime();

  // wait for any previous pulse to end
  while (state == mgos_gpio_read(pin)) {
    if ((hcsr04_uptime() - start_us) > timeout) {
      return 0;
    }
  }

  // wait for the pulse to start
  while (state != mgos_gpio_read(pin)) {
    if ((hcsr04_uptime() - start_us) > timeout) {
      return 0;
    }
  }

  uint64_t start = hcsr04_uptime();

  // wait for the pulse to stop
  while (state == mgos_gpio_read(pin)) {
    if ((hcsr04_uptime() - start_us) > timeout) {
      return 0;
    }
  }
  return (uint32_t)(hcsr04_uptime() - start);
} */

struct mgos_hcsr04 *mgos_hcsr04_create(int trig_pin, int echo_pin) {
  if (!mgos_gpio_setup_output(trig_pin, 0) ||
      !mgos_gpio_setup_input(echo_pin, MGOS_GPIO_PULL_UP)) {
    return NULL;
  }
  struct mgos_hcsr04 *handle = calloc(1, sizeof(*handle));
  if (handle == NULL) return NULL;
  
  handle->trig_pin = trig_pin;
  handle->echo_pin = echo_pin; 
  return handle;
}

void mgos_hcsr04_close(struct mgos_hcsr04 *handle) {
  free(handle);
  handle = NULL;
}

long mgos_hcsr04_get_echo(struct mgos_hcsr04 *handle) {
  if (handle == NULL) return -1;
  
  // Make sure that trigger pin is LOW.
  mgos_gpio_write(handle->trig_pin, 0);
  mgos_usleep(5);

  // Hold trigger for 10 microseconds, which is signal for sensor to measure distance.
  mgos_gpio_write(handle->trig_pin, 1);
  mgos_usleep(10);
  mgos_gpio_write(handle->trig_pin, 0);

  // wait for the pulse to start
  uint64_t timeout = 1000000;
  while (1 != mgos_gpio_read(handle->echo_pin) && timeout > 0) {
    --timeout;
  };

  if (timeout != 0) {
    int64_t pulse_start = mgos_time_micros();

    // wait for the pulse to stop
    timeout = 1000000;
    while (1 == mgos_gpio_read(handle->echo_pin && timeout > 0) {
      --timeout;
    }
  }
  return (timeout > 0 ? (mgos_time_micros() - pulse_start) : -1);
}

float mgos_hcsr04_get_distance(struct mgos_hcsr04 *handle) { 
  return mgos_hcsr04_get_distance_ex(handle, 19.307);
}

float mgos_hcsr04_get_distance_ex(struct mgos_hcsr04 *handle, float temperature) { 
  long duration = mgos_hcsr04_get_echo(handle);
  if (duration == -1) return NAN;
  if (duration > 4003) return NAN;
    
  float sound_speed = 0.3313 + 0.000606 * temperature; // Cair ≈ (331.3 + 0.606 ⋅ ϑ) m/s
  float distance = (duration / 2) * sound_speed;
  
	return (distance < 0 ? NAN : distance);
}

float mgos_hcsr04_get_distance_avg(struct mgos_hcsr04 *handle,
                                   int attempts_count,
                                   int attempts_delay) {
  int not_nan_count = 0;
  float result = 0;
  if (handle != NULL) {
    if (attempts_delay <= 0) attempts_delay = DEFAULT_AVG_ATTEMPTS_DELAY;
    for (int i = 0; i < attempts_count; ++i) {
      if (i > 0) mgos_msleep(attempts_delay);
      float m = mgos_hcsr04_get_distance(handle);
      if (!isnan(m)) {
        result += m;
        ++not_nan_count;
      }
    }
  }
  return (not_nan_count == 0 ? NAN : (result / not_nan_count));
}


bool mgos_hcsr04_init(void) {
  return true;
}
