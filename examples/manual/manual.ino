/*
 *  Basic demo of interval_send with manual calls.
 *  Callback "start" and "stop" allow basic control from UI buttons
*/

#include "electricui.h"
#include "interval_send.h"

uint8_t   blink_enable = 1; // if the blinker should be running
uint8_t   led_state  = 0;   // track if the LED is illuminated
uint16_t  glow_time  = 200; // in milliseconds

uint32_t  led_timer  = 0;   // track when the light turned on or off

char device_name[] = "Manual Send";

eui_interface_t serial_comms = EUI_INTERFACE( &serial_write ); 

void enable_sender( void );
void disable_sender( void );

eui_message_t tracked_variables[] = 
{
  EUI_UINT8(  "led_blink",  blink_enable ),
  EUI_UINT8(  "led_state",  led_state ),
  EUI_UINT16( "lit_time",   glow_time ),
  EUI_CHAR_ARRAY( "name",   device_name ),

  EUI_FUNC( "start", enable_sender ),
  EUI_FUNC( "stop", disable_sender ),
};

void setup() 
{
  // Setup the serial port and status LED
  Serial.begin( 115200 );
  pinMode( LED_BUILTIN, OUTPUT );

  eui_setup_interface( &serial_comms );
  EUI_TRACK( tracked_variables );
  eui_setup_identifier( "manual", 5 );

  led_timer = millis();

  // Setup the sender with the led_state message ID
  // Will be sent every 50 milliseconds
  interval_send_add_id( "led_state", 50 );

  // Sending starts when added, but can be paused manually 
  interval_send_stop_id( "led_state" );
}

void enable_sender( void )
{
  interval_send_start_id( "led_state" );
}

void disable_sender( void )
{
  interval_send_stop_id( "led_state" );
}

void loop() 
{
  while( Serial.available() > 0 )  
  {  
    eui_parse( Serial.read(), &serial_interface );
  }

  // Provide the current time in milliseconds, allowing the 
  // library to send data to the UI if needed
  interval_send_tick( millis() ); 

  // Standard hello_blink behaviour
  if( blink_enable )
  {
    if( millis() - led_timer >= glow_time )
    {
      led_state = !led_state;
      led_timer = millis();
    }    
  }

  digitalWrite( LED_BUILTIN, led_state ); 
}
  
void serial_write( uint8_t *data, uint16_t len )
{
  Serial.write( data, len );
}
