/* Includes */
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "adl.h"

#include "adl-oneshot-timer.h"
#include "adl-oneshot-task.h"
#include "adl-task.h"

#include "adl-util-limited-range-int.h"
#include "integer-param.h"

#include "thermistor.h"
#include "adafruit-neopixel-adl.h"

/* Defines, enums, typedefs */

enum eTemperatureState
{
    eTemperatureState_Low,
    eTemperatureState_OK,
    eTemperatureState_High
};

#define LOW_TEMPERATURE_COLOR 0,0,32
#define OK_TEMPERATURE_COLOR 0,32,0
#define HIGH_TEMPERATURE_COLOR 32,0,0

/* Constants */

static const uint8_t RELAY_PIN = 2;

/* Local Variables */

static Thermistor * s_pThermistor;
static AdafruitNeoPixelADL * s_pNeoPixels;

static IntegerParam * s_pTargetTemperature;
static IntegerParam * s_pTargetRange;
static IntegerParam * s_pTimerPeriod;

static ADLOneShotTimer s_temperature_timer(3000);
static bool s_game_won = false;

/* Private Functions */

static void setThermometer(enum eTemperatureState eCurrentTemperatureState)
{
    s_pNeoPixels->pixels().clear();
    switch (eCurrentTemperatureState)
    {
    case eTemperatureState_Low:
        s_pNeoPixels->pixels().setPixelColor(0, LOW_TEMPERATURE_COLOR);
        break;
    case eTemperatureState_OK:
        s_pNeoPixels->pixels().setPixelColor(1, OK_TEMPERATURE_COLOR);
        break;
    case eTemperatureState_High:
        s_pNeoPixels->pixels().setPixelColor(2, HIGH_TEMPERATURE_COLOR);
        break;
    }
    s_pNeoPixels->pixels().show();
}

static eTemperatureState getTemperatureState()
{
    enum eTemperatureState eCurrentTemperatureState = eTemperatureState_OK;
    int8_t temperature = (int8_t)(s_pThermistor->reading() + 0.5f);
    int8_t lower_threshold = s_pTargetTemperature->get() - s_pTargetRange->get();
    int8_t upper_threshold = s_pTargetTemperature->get() + s_pTargetRange->get();

    if (temperature < lower_threshold)
    {
        eCurrentTemperatureState = eTemperatureState_Low;
        s_temperature_timer.set_period(s_pTimerPeriod->get());
        s_temperature_timer.start();
    }
    else if (temperature > upper_threshold)
    {
        eCurrentTemperatureState = eTemperatureState_High;
    }
    return eCurrentTemperatureState;
}

static void game_task_fn(ADLTask& task, void * pTaskData)
{
    (void)task;
    (void)pTaskData;

    enum eTemperatureState eCurrentTemperatureState = getTemperatureState();

    if (eCurrentTemperatureState != eTemperatureState_OK)
    {
        s_temperature_timer.set_period(s_pTimerPeriod->get());
        s_temperature_timer.start();
    }
    else
    {
        if (s_temperature_timer.check_and_reset())
        {
            adl_logln(LOG_APP, "Game won!");
            s_game_won = true;
        }
    }
    
    setThermometer(eCurrentTemperatureState);
}
static ADLTask game_task(20, game_task_fn, NULL);

static void log_task_fn(ADLTask& task, void * pTaskData)
{
    static const char * s_state_strings[]
    {
        "too low",
        "ok",
        "too high"
    };

    (void)task;
    (void)pTaskData;

    adl_logln(LOG_APP, "Temp: %" PRIu8 "C (%s)",
        (int8_t)(s_pThermistor->reading() + 0.5f),
        s_state_strings[getTemperatureState()]
    );
}
static ADLTask log_task(500, log_task_fn, NULL);

/* Public Functions */

void adl_custom_setup(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)ndevices; (void)nparams;
    
    s_pThermistor = (Thermistor*)pdevices[0];
    s_pNeoPixels = (AdafruitNeoPixelADL*)pdevices[1];

    s_pTargetTemperature = (IntegerParam*)pparams[0];
    s_pTargetRange = (IntegerParam*)pparams[1];
    s_pTimerPeriod = (IntegerParam*)pparams[2];

    adl_logln(LOG_APP, "Cryptology 'Just Right' Game");
    adl_logln(LOG_APP, "Target temperature: %" PRIi32 " +/- %" PRIi32,
        s_pTargetTemperature->get(),
        s_pTargetRange->get()
    );

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
}

void adl_custom_loop(DeviceBase * pdevices[], int ndevices, ParameterBase * pparams[], int nparams)
{
    (void)pdevices; (void)ndevices; (void)pparams; (void)nparams;

    if (!s_game_won)
    {
        game_task.run();
        log_task.run();
    }
    else
    {
        digitalWrite(RELAY_PIN, HIGH);
    }
}
