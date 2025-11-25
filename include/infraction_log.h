#ifndef INFRACTION_LOG_H
#define INFRACTION_LOG_H
#include <zephyr/kernel.h>
#include "common.h"

/* > Infraction Record */
typedef struct infraction_record {
    int64_t timestamp_ms;
    vehicle_type_t type;
    uint32_t speed_kmh;
    uint32_t limit_kmh;
    bool valid_read;
    char plate[10];
} infraction_record_t;

/**
 * @brief Adds an infraction record to the log.
 * @param record The infraction record to add.
 */
void infraction_log_add(const infraction_record_t *record);

/**
 * @brief Gets the most recent infraction records from the log.
 * @param max_records The maximum number of records to get.
 * @param out_records The array to store the records.
 * @return The number of records copied.
 */
size_t infraction_log_get_recent(size_t max_records, infraction_record_t *out_records);

/**
 * @brief Gets the counters for the infraction log.
 * @param light_count The count of light vehicles.
 * @param heavy_count The count of heavy vehicles.
 * @param valid_reads The count of valid reads.
 * @param invalid_reads The count of invalid reads.
 */
void infraction_log_get_counters(uint32_t *light_count, uint32_t *heavy_count, uint32_t *valid_reads, uint32_t *invalid_reads);

#endif
