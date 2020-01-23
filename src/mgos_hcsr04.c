#include "mgos.h"
#include "mgos_hcsr04.h"

struct mgos_hcsr04 {  
  int trig_pin;
  int echo_pin;
};

static inline void mg_play_trig_sound(int trig_pin, uint32_t duration) {
  //send trigger
  mgos_gpio_write(trig_pin, 1);
  // wait 10 microseconds
  mgos_usleep(duration);
  // stop the trigger
  mgos_gpio_write(trig_pin, 0);
}

static inline uint64_t mg_trig_echo_uptime() {
  return (uint64_t)(1000000 * mgos_uptime());
}

static inline float mg_await_echo(int echo_pin, uint32_t timeout) {
  if (timeout == 0) timeout = 1000000L;
  uint64_t startMicros = mg_trig_echo_uptime();

  // wait for any previous pulse to end
  while (mgos_gpio_read(echo_pin) == 1) {
    if ((mg_trig_echo_uptime() - startMicros) > timeout) {
      return NAN;
    }
  }
  // wait for the pulse to start
  while (mgos_gpio_read(echo_pin) != 1) {
    if ((mg_trig_echo_uptime() - startMicros) > timeout) {
      return NAN;
    }
  }  
  // wait for the pulse to stop
  uint64_t start = uptime();
  while (mgos_gpio_read(echo_pin) == 1) {
    if ((mg_trig_echo_uptime() - startMicros) > timeout) {
      return NAN;
    }
  }

  float duration = (mg_trig_echo_uptime() - start);
  return duration;
}

float mg_trig_echo_get_distance(int trig_pin, uint32_t trig_duration,
                        int echo_pin, uint32_t echo_timeout) {
  // play the trigger sound
  mg_play_trig_sound(trig_pin, trig_duration);
  // await the sound's echo
  float duration = mg_await_echo(echo_pin, echo_timeout);
  if (isnan(duration)) return NAN;
  // calculate distance (millimiters)  
  return ((duration * 0.034 / 2) * 10); 
}

struct mgos_hcsr04 *mgos_hcsr04_create(int trig_pin, int echo_pin) {
  if (!mgos_gpio_set_mode(trig_pin, MGOS_GPIO_MODE_OUTPUT) ||
      !mgos_gpio_set_mode(echo_pin, MGOS_GPIO_MODE_INPUT) ||
      !mgos_gpio_set_pull(echo_pin, MGOS_GPIO_PULL_UP));

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

float mgos_hcsr04_get_distance(struct mgos_hcsr04 *handle) {  
  if (handle == NULL) return NAN;
  return mg_trig_echo_get_distance(handle->trig_pin,
    10, handle->echo_pin, 0);
}

bool mgos_hcsr04_init(void) {
  return true;
}
