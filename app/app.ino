















/*
app
Created on 2018-11-08
By the Arduino Description Language tool.
 

        ADL based application for a thermistor hot/cold/just right game
    
*/





#include <stdint.h>

#include "adl.h"


#include <Adafruit_NeoPixel.h>



    #include "adl-util-limited-range-int.h"



#include "potential-divider.h"

#include "thermistor.h"

#include "adafruit-neopixel-adl.h"



#include "integer-param.h"



static Thermistor s_thermistor = Thermistor(A7, 10000, 3977, 8200, FIXED_PULLUP);

static AdafruitNeoPixelADL s_neopixels = AdafruitNeoPixelADL(11, 3, NEO_GRB + NEO_KHZ800);


static DeviceBase * s_devices[] = 
{
    
    &s_thermistor
    
    ,
    
    
    &s_neopixels
    
    
};


static IntegerParam s_temp = IntegerParam(0, INT32_MIN, INT32_MAX, true, true);

static IntegerParam s_range = IntegerParam(0, INT32_MIN, INT32_MAX, true, true);

static IntegerParam s_timer = IntegerParam(0, INT32_MIN, INT32_MAX, true, true);


static ParameterBase * s_params[] = 
{
    
    &s_temp
    
    ,
    
    
    &s_range
    
    ,
    
    
    &s_timer
    
    
};






int handle_device1_command(char const * const command, char * reply)
{
    return s_thermistor.command_handler(command, reply);
}

int handle_device2_command(char const * const command, char * reply)
{
    return s_neopixels.command_handler(command, reply);
}


static COMMAND_HANDLER adl_devices[] = {
    
    handle_device1_command,
    
    handle_device2_command,
    
};

COMMAND_HANDLER& adl_get_device_cmd_handler(DEVICE_ADDRESS address)
{
    return adl_devices[address-1];
}

DeviceBase& adl_get_device(DEVICE_ADDRESS address)
{
    return *s_devices[address-1];
}


int handle_param1_command(char const * const command, char * reply)
{
    return s_temp.command_handler(command, reply);
}

int handle_param2_command(char const * const command, char * reply)
{
    return s_range.command_handler(command, reply);
}

int handle_param3_command(char const * const command, char * reply)
{
    return s_timer.command_handler(command, reply);
}


static COMMAND_HANDLER adl_params[] = {
    
    handle_param1_command,
    
    handle_param2_command,
    
    handle_param3_command,
    
};

COMMAND_HANDLER& adl_get_param_cmd_handler(PARAM_ADDRESS address)
{
    return adl_params[address-1];
}

ParameterBase& adl_get_param(PARAM_ADDRESS address)
{
    return *s_params[address-1];
}



void adl_board_send(char * to_send)
{
    Serial.println(to_send);
}


void setup()
{
    adl_on_setup_start();

    Serial.begin(115200);
    
    adl_nonvolatile_setup();

    adl_logging_setup(Serial);

    
    // Setup for Thermistor
    s_thermistor.setup();
    // END Thermistor setup
    
    // Setup for NeoPixels
    s_neopixels.setup();
    // END NeoPixels setup
    

    
    // Setup for temp
    s_temp.setup();
    // END temp setup
    
    // Setup for range
    s_range.setup();
    // END range setup
    
    // Setup for timer
    s_timer.setup();
    // END timer setup
    

    adl_custom_setup(s_devices, ADL_DEVICE_COUNT, s_params, ADL_PARAM_COUNT);

    adl_on_setup_complete();
    
    if (0)
    {
        adl_delay_start( 0 );
    }
}



void loop()
{
    adl_handle_any_pending_commands();
    adl_service_timer();
    adl_custom_loop(s_devices, ADL_DEVICE_COUNT, s_params, ADL_PARAM_COUNT);
}



        void serialEvent()
        {
            while (Serial.available())
            {
                adl_add_char((char)Serial.read());
            }
        }



