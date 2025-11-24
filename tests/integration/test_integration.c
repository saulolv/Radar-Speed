#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include "../../src/common.h"

/*
 * Integration Test: ZBUS Flow
 * We will simulate the Main Thread publishing a trigger, 
 * and check if the Camera Thread (mocked or real) receives it.
 * 
 * Since we are running unit tests, we might not want to spawn ALL threads.
 * We will define a simple test that uses ZBUS APIs.
 */

ZBUS_CHAN_DEFINE(camera_trigger_chan, camera_trigger_t, NULL, NULL, ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(0));
ZBUS_CHAN_DEFINE(camera_result_chan, camera_result_t, NULL, NULL, ZBUS_OBSERVERS_EMPTY, ZBUS_MSG_INIT(0));

ZBUS_SUBSCRIBER_DEFINE(test_sub, 4);

ZTEST(radar_integration, test_zbus_trigger)
{
    // Subscribe
    zbus_chan_add_obs(&camera_trigger_chan, &test_sub, K_NO_WAIT);

    // Publish Trigger
    camera_trigger_t trig = { .speed_kmh = 80, .type = VEHICLE_LIGHT };
    int ret = zbus_chan_pub(&camera_trigger_chan, &trig, K_NO_WAIT);
    zassert_equal(ret, 0, "Publish failed");

    // Wait for it
    const struct zbus_channel *chan;
    ret = zbus_sub_wait(&test_sub, &chan, K_MSEC(100));
    zassert_equal(ret, 0, "Wait failed");
    zassert_equal(chan, &camera_trigger_chan, "Wrong channel");

    // Read back
    camera_trigger_t read_trig;
    zbus_chan_read(&camera_trigger_chan, &read_trig, K_NO_WAIT);
    zassert_equal(read_trig.speed_kmh, 80, "Speed mismatch");
}

ZTEST_SUITE(radar_integration, NULL, NULL, NULL, NULL, NULL);

