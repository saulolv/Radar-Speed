#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(traffic_sim, LOG_LEVEL_INF);

#include "common.h"

void traffic_sim_thread_entry(void *p1, void *p2, void *p3) {
    LOG_INF("Traffic Simulator Started (Auto-generating vehicles every 5s)");

    k_sleep(K_SECONDS(2)); // Wait for system to settle

    while (1) {
        sensor_data_t s_data;

        // 1. Simulate a Light Vehicle (Normal Speed)
        // Distance 5m, Speed 50km/h
        // Time = Dist / Speed = 0.005 km / 50 km/h = 0.0001 h = 0.36 s = 360ms
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 360; 
        s_data.timestamp_end = s_data.timestamp_start + 360;
        s_data.axle_count = 2;
        s_data.type = VEHICLE_LIGHT;
        
        LOG_INF("SIMULATION: Generating Light Vehicle (50 km/h)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);
        
        k_sleep(K_SECONDS(5));

        // 1b. Simulate a Light Vehicle in WARNING band (~58 km/h)
        // 58 km/h -> duration ≈ (5000 * 36) / (580 * 10) ≈ 310 ms
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 310;
        s_data.timestamp_end = s_data.timestamp_start + 310;
        s_data.axle_count = 2;
        s_data.type = VEHICLE_LIGHT;

        LOG_INF("SIMULATION: Generating Light Vehicle (58 km/h - Warning)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);

        k_sleep(K_SECONDS(5));

        // 2. Simulate a Heavy Vehicle (Infraction)
        // Limit is 40 km/h. Let's go 50 km/h.
        // Time = 360ms (same speed as above, but for heavy it's a violation)
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 360; 
        s_data.timestamp_end = s_data.timestamp_start + 360;
        s_data.axle_count = 3;
        s_data.type = VEHICLE_HEAVY;

        LOG_INF("SIMULATION: Generating Heavy Vehicle (50 km/h - Infraction!)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);

        k_sleep(K_SECONDS(5));
        
        // 3. Simulate High Speed Light Vehicle (Infraction)
        // 80 km/h. Time = 225ms
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 225; 
        s_data.timestamp_end = s_data.timestamp_start + 225;
        s_data.axle_count = 2;
        s_data.type = VEHICLE_LIGHT;

        LOG_INF("SIMULATION: Generating Light Vehicle (80 km/h - Infraction!)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);
        
        k_sleep(K_SECONDS(5));

        // 3b. Simulate Heavy Vehicle in WARNING band (~38 km/h)
        // 38 km/h -> duration ≈ (5000 * 36) / (380 * 10) ≈ 474ms
        s_data.timestamp_start = k_uptime_get();
        s_data.duration_ms = 474;
        s_data.timestamp_end = s_data.timestamp_start + 474;
        s_data.axle_count = 3;
        s_data.type = VEHICLE_HEAVY;

        LOG_INF("SIMULATION: Generating Heavy Vehicle (38 km/h - Warning)");
        k_msgq_put(&sensor_msgq, &s_data, K_NO_WAIT);

        k_sleep(K_SECONDS(5));
    }
}

K_THREAD_DEFINE(traffic_sim_tid, 1024, traffic_sim_thread_entry, NULL, NULL, NULL, 8, 0, 0);

