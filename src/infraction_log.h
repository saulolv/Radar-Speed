#ifndef INFRACTION_LOG_H
#define INFRACTION_LOG_H
#include <zephyr/kernel.h>
#include "common.h"

struct infraction_record {
    int64_t timestamp_ms;
    enum vehicle_type type;
    uint32_t speed_kmh;
    uint32_t limit_kmh;
    bool valid_read;
    char plate[10];
};

void infraction_log_add(const struct infraction_record *record);
size_t infraction_log_get_recent(size_t max_records, struct infraction_record *out_records);

void infraction_log_get_counters(uint32_t *light_count, uint32_t *heavy_count, uint32_t *valid_reads, uint32_t *invalid_reads);

#endif
