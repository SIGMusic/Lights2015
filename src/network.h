/**
 * SIGMusic Lights 2015
 * Header file for network configuration
 */

#pragma once

#include <stdint.h>

// Generates a 40-bit nRF address
#define RF_ADDRESS(endpoint)    (0x5349474D00LL | ((endpoint) & 0xFF))

// The endpoint ID of the base station
#define BASE_STATION_ID         0x00

// Time to dwell on each channel before hopping
#define DWELL_TIME              10 // ms

// The structure of a packet specified in the protocol
typedef struct packet {
    uint8_t data[3];
    uint8_t sync;
} packet_t;

// The list of channels to use, in order
uint8_t channels[] = {
    37, 41, 30, 24, 23, 58, 13, 35,  4, 80, 17, 22, 26, 10, 16, 49,
    48, 28, 57,  3,  5, 18, 27,  8, 67, 38, 51, 19, 32, 72, 68, 65,
    55, 76, 47, 46, 31, 71, 60, 78, 20, 43, 82, 83, 53, 77, 64, 12,
    34, 21, 63, 45, 70, 69, 75, 29, 36, 44, 25, 62, 39, 54, 59,  6,
    11, 40, 73,  9,  7, 61, 56,  1, 42, 52, 79, 15, 74,  2, 50, 33,
      0, 14, 81, 66
};
