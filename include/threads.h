#ifndef THREADS_H
#define THREADS_H

/**
 * @brief Entry point for the sensor thread.
 * @param p1 Pointer to the sensor thread data.
 * @param p2 Pointer to the sensor thread data.
 * @param p3 Pointer to the sensor thread data.
 */
void sensor_thread_entry(void *p1, void *p2, void *p3);

/**
 * @brief Entry point for the display thread.
 * @param p1 Pointer to the display thread data.
 * @param p2 Pointer to the display thread data.
 * @param p3 Pointer to the display thread data.
 */
void display_thread_entry(void *p1, void *p2, void *p3);

#endif

