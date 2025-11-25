#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(traffic_sim, LOG_LEVEL_INF);

#include "common.h"

/**
 * @brief Main entry point for the traffic simulator thread.
 * @param p1 Unused.
 * @param p2 Unused.
 * @param p3 Unused.
 */
void traffic_sim_thread_entry(void *p1, void *p2, void *p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    LOG_INF("Traffic Simulator Started (Auto-generating vehicles every 10s)");

    k_sleep(K_SECONDS(2));

    while (1) {
        sensor_data_t s_data;

        /* 1. Simulate a Light Vehicle (Normal Speed) */
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 360; 
        s_data.timestamp_end = s_data.timestamp_start + 360;
        s_data.axle_count = 2;
        s_data.type = VEHICLE_LIGHT;
        
        LOG_INF("SIMULATION: Generating Light Vehicle (50 km/h)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);
        
        k_sleep(K_SECONDS(10));

        /* 1b. Simulate a Light Vehicle in WARNING band (~58 km/h) */
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 310;
        s_data.timestamp_end = s_data.timestamp_start + 310;
        s_data.axle_count = 2;
        s_data.type = VEHICLE_LIGHT;

        LOG_INF("SIMULATION: Generating Light Vehicle (58 km/h - Warning)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);

        k_sleep(K_SECONDS(10));

        /* 2. Simulate a Heavy Vehicle (Infraction) */
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 360; 
        s_data.timestamp_end = s_data.timestamp_start + 360;
        s_data.axle_count = 3;
        s_data.type = VEHICLE_HEAVY;

        LOG_INF("SIMULATION: Generating Heavy Vehicle (50 km/h - Infraction!)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);

        k_sleep(K_SECONDS(10));
        
        /* 3. Simulate High Speed Light Vehicle (Infraction) */
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 225; 
        s_data.timestamp_end = s_data.timestamp_start + 225;
        s_data.axle_count = 2;
        s_data.type = VEHICLE_LIGHT;

        LOG_INF("SIMULATION: Generating Light Vehicle (80 km/h - Infraction!)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);
        
        k_sleep(K_SECONDS(10));

        /* 3b. Simulate Heavy Vehicle in WARNING band (~38 km/h) */
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 474;
        s_data.timestamp_end = s_data.timestamp_start + 474;
        s_data.axle_count = 3;
        s_data.type = VEHICLE_HEAVY;

        LOG_INF("SIMULATION: Generating Heavy Vehicle (38 km/h - Warning)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);

        k_sleep(K_SECONDS(10));
    }
}

/**
 * @brief Thread Definition for Traffic Simulator
 */
K_THREAD_DEFINE(traffic_sim_tid, 1024, traffic_sim_thread_entry, NULL, NULL, NULL, 8, 0, 0);

