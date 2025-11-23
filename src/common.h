#ifndef COMMON_H
#define COMMON_H
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

// Vehicle Types
enum vehicle_type {
    VEHICLE_LIGHT,
    VEHICLE_HEAVY,
    VEHICLE_UNKNOWN
};

// Data from Sensor Thread to Main Thread
struct sensor_data {
    int64_t timestamp_start;
    int64_t timestamp_end;
    uint32_t duration_ms;
    uint32_t axle_count;
    enum vehicle_type type;
};

// Display Status
enum display_status {
    STATUS_NORMAL,
    STATUS_WARNING,
    STATUS_INFRACTION
};

// Data for Display
struct display_data {
    uint32_t speed_kmh;
    uint32_t limit_kmh;
    enum vehicle_type type;
    enum display_status status;
    char plate[10]; // Optional, for result display
    uint32_t axle_count; // For UX display
    uint32_t warning_kmh; // Threshold for yellow status
};

// ZBUS: Camera Trigger
struct camera_trigger {
    uint32_t speed_kmh;
    enum vehicle_type type;
};

// ZBUS: Camera Result
struct camera_result {
    char plate[10];
    bool valid_read; // If the camera successfully read a plate
};

// Channels
ZBUS_CHAN_DECLARE(camera_trigger_chan);
ZBUS_CHAN_DECLARE(camera_result_chan);

// Message Queues
extern struct k_msgq sensor_msgq;
extern struct k_msgq display_msgq;

// Helper functions
bool validate_plate(const char *plate);
uint32_t calculate_speed(uint32_t distance_mm, uint32_t duration_ms);
#endif
