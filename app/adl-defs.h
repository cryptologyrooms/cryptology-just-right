#ifndef _ADL_DEFS_H_
#define _ADL_DEFS_H_



enum adl_devices
{
    ADL_DEVICE_BOARD,

    ADL_DEVICE_THERMISTOR,

    ADL_DEVICE_NEOPIXELS,

};
typedef enum adl_devices ADL_DEVICES;

enum adl_parameters
{

    ADL_PARAM_TEMP,

    ADL_PARAM_RANGE,

    ADL_PARAM_TIMER,

};
typedef enum adl_parameters ADL_PARAMETERS;

typedef uint16_t ADL_NV_ADDR;

static const int ADL_DEVICE_COUNT = 2;
static const int ADL_PARAM_COUNT = 3;

static const int ADL_BUFFER_SIZE = 64;

static const unsigned long ADL_TICK_MS = 10;

#endif