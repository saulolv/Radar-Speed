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
 * @brief Checks whether a plate matches a symbolic pattern.
 * Pattern tokens:
 *  - 'L': alphabetic character (case-insensitive)
 *  - 'N': numeric character
 *  - any other char: must match literally
 */
static inline bool util_matches_pattern(const char *plate, const char *pattern)
{
	size_t plate_len = strlen(plate);
	size_t pattern_len = strlen(pattern);

	if (plate_len != pattern_len) {
		return false;
	}

	for (size_t i = 0; i < pattern_len; ++i) {
		char token = pattern[i];
		char c = util_to_upper_char(plate[i]);

		if (token == 'L') {
			if (!util_is_letter(c)) {
				return false;
			}
		} else if (token == 'N') {
			if (!util_is_digit(c)) {
				return false;
			}
		} else if (token != c) {
			return false;
		}
	}

	return true;
}

/**
 * @brief Validates a Mercosul plate number.
 * @param plate The plate number to validate.
 * @return True if the plate number is valid, false otherwise.
 */
bool validate_plate(const char *plate) {
	char sanitized[16];
	size_t sanitized_len = 0;

	for (size_t i = 0; plate[i] != '\0'; ++i) {
		char c = plate[i];
		if (c == ' ' || c == '-' || c == '_') {
			continue;
		}
		if (sanitized_len >= ARRAY_SIZE(sanitized) - 1) {
			return false;
		}
		sanitized[sanitized_len++] = c;
	}
	sanitized[sanitized_len] = '\0';

	if (sanitized_len != 7) {
		return false;
	}

	static const char *mercosur_patterns[] = {
		"LLLNLNN", /* Brasil */
		"LLNNNLL", /* Argentina */
		"LLLNNNN", /* Uruguay */
		"LLLLNNN", /* Paraguay cars */
		"NNNLLLL", /* Paraguay motorcycles */
		"LLNNNNN", /* Bolivia */
	};

	for (size_t i = 0; i < ARRAY_SIZE(mercosur_patterns); ++i) {
		if (util_matches_pattern(sanitized, mercosur_patterns[i])) {
			return true;
		}
	}

	return false;
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
