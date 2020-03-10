/*
 * Copyright (c) 2020 ZenDIY
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * HC-SR04 (ultrasonic distance sensor) API.
 */

#ifndef MGOS_HCSR04_H_
#define MGOS_HCSR04_H_

#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_AVG_ATTEMPTS_DELAY 5

/* Opaque sensor's handle */
struct mgos_hcsr04;

/* Initialize the sensor. Return an opaque
 * sensor's handle, or `NULL` on error.
 */
struct mgos_hcsr04 *mgos_hcsr04_create(int trig_pin, int echo_pin);

/* Close sensor's handle. */
void mgos_hcsr04_close(struct mgos_hcsr04 *handle);

/* Return distance in millimiters or 'NAN' on failure. */
float mgos_hcsr04_get_distance(struct mgos_hcsr04 *handle);

/* Perform multiple measurements and return the average
 * distance in millimiters or 'NAN' on failure.
 */
float mgos_hcsr04_get_distance_avg(struct mgos_hcsr04 *sensor,
                                   int attempts_count,
                                   int attempts_delay);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_HCSR04_H_ */
