#include <zephyr/ztest.h>
#include <string.h>
#include "common.h"

/**
 * @brief Test suite for integration simple
 */
ZTEST_SUITE(integration_simple, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test case for sensor data structure
 */
ZTEST(integration_simple, test_sensor_data_structure)
{
    struct sensor_data s_data = {
        .timestamp_start = 1000,
        .duration_ms = 360,
        .timestamp_end = 1360,
        .axle_count = 2,
        .type = VEHICLE_LIGHT
    };
    
    zassert_equal(s_data.axle_count, 2, "Axle count should be 2");
    zassert_equal(s_data.type, VEHICLE_LIGHT, "Should be light vehicle");
    zassert_equal(s_data.duration_ms, 360, "Duration should be 360ms");
}

/**
 * @brief Test case for display data structure
 */
ZTEST(integration_simple, test_display_data_structure)
{
    struct display_data d_data = {
        .speed_kmh = 50,
        .limit_kmh = 60,
        .type = VEHICLE_LIGHT,
        .status = STATUS_NORMAL,
        .axle_count = 2,
        .warning_kmh = 54
    };
    
    zassert_equal(d_data.speed_kmh, 50, "Speed should be 50");
    zassert_equal(d_data.status, STATUS_NORMAL, "Status should be normal");
    zassert_equal(d_data.type, VEHICLE_LIGHT, "Type should be light");
}

/**
 * @brief Test case for camera trigger structure
 */
ZTEST(integration_simple, test_camera_trigger_structure)
{
    struct camera_trigger trigger = {
        .speed_kmh = 80,
        .type = VEHICLE_LIGHT
    };
    
    zassert_equal(trigger.speed_kmh, 80, "Trigger speed should be 80");
    zassert_equal(trigger.type, VEHICLE_LIGHT, "Should be light vehicle");
}

/**
 * @brief Test case for camera result structure
 */
ZTEST(integration_simple, test_camera_result_structure)
{
    struct camera_result result = {
        .valid_read = true
    };
    strcpy(result.plate, "ABC1D23");
    
    zassert_true(result.valid_read, "Should be valid read");
    zassert_true(validate_plate(result.plate), "Plate should be valid");
}

/**
 * @brief Test case for speed and classification integration
 */
ZTEST(integration_simple, test_speed_and_classification_integration)
{

    uint32_t distance_mm = 5000;
    uint32_t duration_ms = 360;
    uint32_t speed_kmh = calculate_speed(distance_mm, duration_ms);
    
    zassert_equal(speed_kmh, 50, "Speed should be 50 km/h");
    
    uint32_t light_limit = CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH;
    uint32_t heavy_limit = CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH;
    
    zassert_true(speed_kmh <= light_limit, "Light vehicle within limit");
    zassert_true(speed_kmh > heavy_limit, "Would be infraction for heavy vehicle");
}

/**
 * @brief Test case for status determination
 */
ZTEST(integration_simple, test_status_determination)
{
    uint32_t speed = 55;
    uint32_t limit = 60;
    uint32_t warning_threshold = (limit * 90) / 100; // 54 km/h
    
    enum display_status status;
    
    if (speed > limit) {
        status = STATUS_INFRACTION;
    } else if (speed >= warning_threshold) {
        status = STATUS_WARNING;
    } else {
        status = STATUS_NORMAL;
    }
    
    zassert_equal(status, STATUS_WARNING, "55 km/h should be WARNING (limit 60, threshold 54)");
}

/**
 * @brief Test case for heavy vehicle speed
 */
ZTEST(integration_simple, test_heavy_vehicle_speed)
{
    uint32_t speed = 50;
    uint32_t heavy_limit = CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH;
    
    enum display_status status = (speed > heavy_limit) ? STATUS_INFRACTION : STATUS_NORMAL;
    
    zassert_equal(status, STATUS_INFRACTION, "Heavy vehicle at 50 km/h should be infraction");
    zassert_equal(heavy_limit, 40, "Heavy vehicle limit should be 40 km/h");
}
