#include <zephyr/ztest.h>
#include "../../src/sensor_fsm.h"

ZTEST(radar_fsm, test_start_start_end_finalize_light)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
    
    sensor_fsm_handle_start(&fsm, 1000);
    sensor_fsm_handle_start(&fsm, 1100);
    sensor_fsm_handle_end(&fsm, 1400);
    
    struct sensor_data out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    
    zassert_true(ok, "Finalize should produce data");
    zassert_equal(out.duration_ms, 400, "Duration mismatch");
    zassert_equal(out.axle_count, 2, "Axle count mismatch");
    zassert_equal(out.type, VEHICLE_LIGHT, "Type should be LIGHT");
}

ZTEST(radar_fsm, test_timeout_without_end_no_data)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
   
    sensor_fsm_handle_start(&fsm, 1000);
   
    struct sensor_data out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    zassert_false(ok, "No end signal, should not produce data");
}

ZTEST(radar_fsm, test_end_without_start_no_data)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
    
    sensor_fsm_handle_end(&fsm, 1500);
    
    struct sensor_data out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    zassert_false(ok, "End without start should not produce data");
}

ZTEST(radar_fsm, test_heavy_classification)
{
    struct sensor_fsm fsm;
    sensor_fsm_init(&fsm);
    
    sensor_fsm_handle_start(&fsm, 1000);
    sensor_fsm_handle_start(&fsm, 1050);
    sensor_fsm_handle_start(&fsm, 1100); /* 3 eixos */
    sensor_fsm_handle_end(&fsm, 1500);
    
    struct sensor_data out;
    bool ok = sensor_fsm_finalize(&fsm, &out);
    zassert_true(ok, "Finalize should produce data");
    zassert_equal(out.axle_count, 3, "Axle count mismatch");
    zassert_equal(out.type, VEHICLE_HEAVY, "Type should be HEAVY");
}

ZTEST_SUITE(radar_fsm, NULL, NULL, NULL, NULL, NULL);
