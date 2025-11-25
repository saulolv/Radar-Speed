#ifndef SENSOR_FSM_H
#define SENSOR_FSM_H
#include <zephyr/kernel.h>
#include "common.h"

/* > Heuristics for dynamic axle window calculation */
#define SENSOR_FSM_MIN_AXLE_WINDOW_MS   200U
#define SENSOR_FSM_MAX_AXLE_WINDOW_MS   4000U
#define SENSOR_FSM_REFERENCE_SPEED_KMH  CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH

enum sensor_state {
    SENSOR_IDLE,
    SENSOR_ACTIVE
};

/* > Sensor FSM */
struct sensor_fsm {
    enum sensor_state state;
    int64_t start_time;
    int64_t end_time;
    uint32_t axle_count;
    bool speed_measured;
    uint32_t axle_window_ms;
};

typedef struct sensor_fsm sensor_fsm_t;

/**
 * @brief Classifies the vehicle type based on the number of axles.
 * @param axle_count The number of axles.
 * @return The vehicle type.
 */
static inline vehicle_type_t classify_axles(uint32_t axle_count)
{
    return (axle_count <= 2) ? VEHICLE_LIGHT : VEHICLE_HEAVY;
}

/**
 * @brief Initializes the sensor FSM.
 * @param fsm Pointer to the sensor FSM.
 */
static inline void sensor_fsm_init(struct sensor_fsm *fsm)
{
    fsm->state = SENSOR_IDLE;
    fsm->start_time = 0;
    fsm->end_time = 0;
    fsm->axle_count = 0;
    fsm->speed_measured = false;
    fsm->axle_window_ms = CONFIG_RADAR_AXLE_TIMEOUT_MS;
}

/**
 * @brief Handles the start of a sensor measurement.
 * @param fsm Pointer to the sensor FSM.
 * @param timestamp_ms The timestamp of the start of the measurement.
 */
static inline uint32_t sensor_fsm_get_axle_window_ms(const struct sensor_fsm *fsm)
{
    return fsm->axle_window_ms;
}

static inline uint32_t sensor_fsm_compute_axle_window(uint32_t speed_kmh)
{
    if (speed_kmh == 0U) {
        return CONFIG_RADAR_AXLE_TIMEOUT_MS;
    }

    uint64_t scaled = (uint64_t)CONFIG_RADAR_AXLE_TIMEOUT_MS *
                      (uint64_t)SENSOR_FSM_REFERENCE_SPEED_KMH;
    uint64_t window_ms = scaled / speed_kmh;

    if (window_ms < SENSOR_FSM_MIN_AXLE_WINDOW_MS) {
        window_ms = SENSOR_FSM_MIN_AXLE_WINDOW_MS;
    } else if (window_ms > SENSOR_FSM_MAX_AXLE_WINDOW_MS) {
        window_ms = SENSOR_FSM_MAX_AXLE_WINDOW_MS;
    }

    return (uint32_t)window_ms;
}

static inline void sensor_fsm_handle_start(struct sensor_fsm *fsm, int64_t timestamp_ms)
{
    if (fsm->state == SENSOR_IDLE) {
        fsm->state = SENSOR_ACTIVE;
        fsm->start_time = timestamp_ms;
        fsm->end_time = 0;
        fsm->axle_count = 1;
        fsm->speed_measured = false;
    } else {
        fsm->axle_count++;
    }
}

/**
 * @brief Handles the end of a sensor measurement.
 * @param fsm Pointer to the sensor FSM.
 * @param timestamp_ms The timestamp of the end of the measurement.
 */
static inline bool sensor_fsm_handle_end(struct sensor_fsm *fsm, int64_t timestamp_ms)
{
    if (fsm->state == SENSOR_ACTIVE && !fsm->speed_measured) {
        fsm->end_time = timestamp_ms;
        fsm->speed_measured = true;
        uint32_t duration_ms = (uint32_t)(fsm->end_time - fsm->start_time);
        uint32_t speed_kmh = calculate_speed(CONFIG_RADAR_SENSOR_DISTANCE_MM, duration_ms);
        fsm->axle_window_ms = sensor_fsm_compute_axle_window(speed_kmh);
        return true;
    }
    return false;
}

/**
 * @brief Finalizes the sensor measurement.
 * @param fsm Pointer to the sensor FSM.
 * @param out_data Pointer to the sensor data.
 * @return True if the measurement was finalized, false otherwise.
 */
static inline bool sensor_fsm_finalize(struct sensor_fsm *fsm, sensor_data_t *out_data)
{
    if (fsm->state != SENSOR_ACTIVE) {
        return false;
    }
    bool produced = false;
    
    if (fsm->speed_measured && fsm->end_time > fsm->start_time) {
        out_data->timestamp_start = fsm->start_time;
        out_data->timestamp_end = fsm->end_time;
        out_data->duration_ms = (uint32_t)(fsm->end_time - fsm->start_time);
        out_data->axle_count = fsm->axle_count;
        out_data->type = classify_axles(fsm->axle_count);
        produced = true;
    }
    
    /* Reset to idle regardless, end of measurement window */
    fsm->state = SENSOR_IDLE;
    fsm->start_time = 0;
    fsm->end_time = 0;
    fsm->axle_count = 0;
    fsm->speed_measured = false;
    fsm->axle_window_ms = CONFIG_RADAR_AXLE_TIMEOUT_MS;
    return produced;
}
#endif
