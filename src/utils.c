#include <zephyr/kernel.h>
#include <string.h>
#include "common.h"

/**
 * @brief Converts a character to uppercase.
 * @param c The character to convert.
 * @return The uppercase character.
 */
static inline char util_to_upper_char(char c) {
	if (c >= 'a' && c <= 'z') return (char)(c - 'a' + 'A');
	return c;
}

/**
 * @brief Checks if a character is a letter.
 * @param c The character to check.
 * @return True if the character is a letter, false otherwise.
 */
static inline bool util_is_letter(char c) {
	char u = util_to_upper_char(c);
	return (u >= 'A' && u <= 'Z');
}

/**
 * @brief Checks if a character is a digit.
 * @param c The character to check.
 * @return True if the character is a digit, false otherwise.
 */
static inline bool util_is_digit(char c) {
	return (c >= '0' && c <= '9');
}

/**
 * @brief Validates a Mercosul plate number.
 * @param plate The plate number to validate.
 * @return True if the plate number is valid, false otherwise.
 */
bool validate_plate(const char *plate) {
	if (strlen(plate) != 7) return false;

    bool is_brasil = util_is_letter(plate[0]) && util_is_letter(plate[1]) && util_is_letter(plate[2]) &&
                     util_is_digit(plate[3]) &&
                     util_is_letter(plate[4]) &&
                     util_is_digit(plate[5]) && util_is_digit(plate[6]);

    bool is_argentina = util_is_letter(plate[0]) && util_is_letter(plate[1]) &&
                        util_is_digit(plate[2]) && util_is_digit(plate[3]) && util_is_digit(plate[4]) &&
                        util_is_letter(plate[5]) && util_is_letter(plate[6]);

    bool is_uruguay = util_is_letter(plate[0]) && util_is_letter(plate[1]) && util_is_letter(plate[2]) &&
                      util_is_digit(plate[3]) && util_is_digit(plate[4]) && util_is_digit(plate[5]) && util_is_digit(plate[6]);

    bool is_paraguay = util_is_letter(plate[0]) && util_is_letter(plate[1]) && util_is_letter(plate[2]) && util_is_letter(plate[3]) &&
                       util_is_digit(plate[4]) && util_is_digit(plate[5]) && util_is_digit(plate[6]);

    return is_brasil || is_argentina || is_uruguay || is_paraguay;
}


/**
 * @brief Calculates the speed in km/h based on the distance and duration.
 * @param distance_mm The distance in millimeters.
 * @param duration_ms The duration in milliseconds.
 * @return The speed in km/h.
 */
uint32_t calculate_speed(uint32_t distance_mm, uint32_t duration_ms) {
    if (duration_ms == 0) return 0;

    return (uint32_t)(((uint64_t)distance_mm * 36) / (duration_ms * 10));
}
