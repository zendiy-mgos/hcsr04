#include <stdint.h>
#include "mgos.h"
#include "mgos_hcsr04.h"

struct mgos_hcsr04 {
  int trig_pin;
  int echo_pin;
};

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

  uint32_t timeout = 1000000L;
  uint64_t check_ticks;
  int64_t pulse_start;

  // Make sure that trigger pin is LOW.
  mgos_gpio_write(handle->trig_pin, 0);
  mgos_usleep(5);

  // Hold trigger for 10 microseconds, which is signal for sensor to measure distance.
  mgos_gpio_write(handle->trig_pin, 1);
  mgos_usleep(10);
  mgos_gpio_write(handle->trig_pin, 0);
  
  // wait for any previous pulse to end
  pulse_start = mgos_time_micros();
  check_ticks = UINT64_MAX;
  while (1 == mgos_gpio_read(handle->echo_pin)) {
    --check_ticks;
    if (check_ticks == 0) {
      if ((mgos_time_micros() - pulse_start) > timeout) {
        LOG(LL_ERROR, ("Error awaiting previous pulse to end, on pin %d", handle->echo_pin));  
        return -1;
      }
      check_ticks = UINT64_MAX;
    }
  };

  // wait for the pulse to start
  pulse_start = mgos_time_micros();
  check_ticks = UINT64_MAX;
  while (1 != mgos_gpio_read(handle->echo_pin)) {
    --check_ticks;
    if (check_ticks == 0) {
      if ((mgos_time_micros() - pulse_start) > timeout) {
        LOG(LL_ERROR, ("Error awaiting pulse to start, on pin %d", handle->echo_pin));  
        return -1;
      }
      check_ticks = UINT64_MAX;
    }
  };
  
  pulse_start = mgos_time_micros();

  // wait for the pulse to stop
  check_ticks = UINT64_MAX;
  while (1 == mgos_gpio_read(handle->echo_pin)) {
    --check_ticks;
    if (check_ticks == 0) {
      if ((mgos_time_micros() - pulse_start) > timeout) {
        LOG(LL_ERROR, ("Error awaiting pulse to stop, on pin %d", handle->echo_pin));  
        return -1;
      }
      check_ticks = UINT64_MAX;
    }
  };
  
  return (mgos_time_micros() - pulse_start);
}

float mgos_hcsr04_get_distance(struct mgos_hcsr04 *handle) { 
  return mgos_hcsr04_get_distance_t(handle,
    DEFAULT_TEMPERATURE);
}

float mgos_hcsr04_get_distance_t(struct mgos_hcsr04 *handle, float temperature) { 
  long duration = mgos_hcsr04_get_echo(handle);
  if (duration == -1) return NAN;
    
  float sound_speed = 0.3313 + 0.000606 * temperature; // Cair ≈ (331.3 + 0.606 ⋅ ϑ) m/s
  float distance = (duration / 2) * sound_speed;
  if (distance > 4003) return NAN;
  if (distance < 19.7) return NAN;
  return 
}

float mgos_hcsr04_get_distance_avg(struct mgos_hcsr04 *handle,
                                   int attempts_count,
                                   int attempts_delay) {
  return mgos_hcsr04_get_distance_avg_t(handle, attempts_count,
    attempts_delay, DEFAULT_TEMPERATURE);
}

float mgos_hcsr04_get_distance_avg_t(struct mgos_hcsr04 *handle,
                                     int attempts_count,
                                     int attempts_delay,
                                     float temperature) {
  int not_nan_count = 0;
  float result = 0;
  if (handle != NULL) {
    if (attempts_delay <= 0) attempts_delay = DEFAULT_AVG_ATTEMPTS_DELAY;
    for (int i = 0; i < attempts_count; ++i) {
      if (i > 0) mgos_msleep(attempts_delay);
      float m = mgos_hcsr04_get_distance_t(handle, temperature);
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
