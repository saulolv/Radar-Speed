#include <zephyr/ztest.h>

#include "sensor_fsm.h"

/**
 * @brief Test case for start, start, end, and finalize light vehicle
 */
ZTEST(radar_fsm, test_start_start_end_finalize_light)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
    
    sensor_fsm_handle_start(&fsm, 1000);
    sensor_fsm_handle_start(&fsm, 1100);
    sensor_fsm_handle_end(&fsm, 1400);
    
    sensor_data_t out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    
    zassert_true(ok, "Finalize should produce data");
    zassert_equal(out.duration_ms, 400, "Duration mismatch");
    zassert_equal(out.axle_count, 2, "Axle count mismatch");
    zassert_equal(out.type, VEHICLE_LIGHT, "Type should be LIGHT");
}

/**
 * @brief Test case for timeout without end should not produce data
 */
ZTEST(radar_fsm, test_timeout_without_end_no_data)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
   
    sensor_fsm_handle_start(&fsm, 1000);
   
    sensor_data_t out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    zassert_false(ok, "No end signal, should not produce data");
}

/**
 * @brief Test case for end without start should not produce data
 */
ZTEST(radar_fsm, test_end_without_start_no_data)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
    
    sensor_fsm_handle_end(&fsm, 1500);
    
    sensor_data_t out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    zassert_false(ok, "End without start should not produce data");
}

/**
 * @brief Test case for heavy classification
 */
ZTEST(radar_fsm, test_heavy_classification)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
    
    sensor_fsm_handle_start(&fsm, 1000);
    sensor_fsm_handle_start(&fsm, 1050);
    sensor_fsm_handle_start(&fsm, 1100); /* 3 eixos */
    sensor_fsm_handle_end(&fsm, 1500);
    
    sensor_data_t out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    zassert_true(ok, "Finalize should produce data");
    zassert_equal(out.axle_count, 3, "Axle count mismatch");
    zassert_equal(out.type, VEHICLE_HEAVY, "Type should be HEAVY");
}

/**
 * @brief Test dynamic axle window scales with measured speed
 */
ZTEST(radar_fsm, test_axle_window_scales_with_speed)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);

    /* Fast car -> smaller window */
    sensor_fsm_handle_start(&fsm, 0);
    bool updated_fast = sensor_fsm_handle_end(&fsm, 200); /* 5 m in 200 ms ~ 90 km/h */
    zassert_true(updated_fast, "End should update the window for fast case");
    uint32_t fast_window = sensor_fsm_get_axle_window_ms(&fsm);

    sensor_data_t out;
    (void)sensor_fsm_finalize(&fsm, &out);

    /* Slow car -> larger window */
    sensor_fsm_handle_start(&fsm, 0);
    bool updated_slow = sensor_fsm_handle_end(&fsm, 1000); /* 5 m in 1000 ms ~ 18 km/h */
    zassert_true(updated_slow, "End should update the window for slow case");
    uint32_t slow_window = sensor_fsm_get_axle_window_ms(&fsm);

    zassert_true(fast_window < CONFIG_RADAR_AXLE_TIMEOUT_MS,
                 "Fast vehicle should reduce the window");
    zassert_true(slow_window > CONFIG_RADAR_AXLE_TIMEOUT_MS,
                 "Slow vehicle should increase the window");
    zassert_true(slow_window > fast_window, "Window must grow when speed drops");
}

/**
 * @brief Test suite for radar FSM
 */
ZTEST_SUITE(radar_fsm, NULL, NULL, NULL, NULL, NULL);
