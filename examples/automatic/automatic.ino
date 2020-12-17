/*
 *  Allows the UI to setup/teardown and modify interval_send
 *  Improves on IntervalRequester polling behaviour by auto sending
*/

#include "electricui.h"
#include "interval_send.h"

// Follows the same behaviour as the hello-blink
uint8_t   blink_enable = 1;
uint8_t   led_state    = 0;
uint16_t  glow_time    = 200;
uint32_t  led_timer    = 0;

char device_name[] = "Auto Send";

// Inbound custom type from UI with ID and interval
interval_send_requested_t ui_interval_request = { 0 };

eui_interface_t serial_interface = EUI_INTERFACE_CB( &serial_write, &eui_callback ); 

eui_message_t tracked_variables[] = 
{
  EUI_UINT8( "led_state",  led_state ),
  EUI_UINT16( "lit_time",   glow_time ),
  EUI_CHAR_ARRAY( "name",   device_name ),

  EUI_CUSTOM( "isreq",   ui_interval_request ),
};

void setup() 
{
  Serial.begin( 115200 );
  pinMode( LED_BUILTIN, OUTPUT );

  eui_setup_interface( &serial_interface );
  EUI_TRACK( tracked_variables );
  eui_setup_identifier( "auto", 4 );

  led_timer = millis();
}

void loop() 
{
  while( Serial.available() > 0 )  
  {  
    eui_parse( Serial.read(), &serial_interface );
  }

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

void eui_callback( uint8_t message )
{
  if( message == EUI_CB_TRACKED )
  {
    uint8_t *name_rx = serial_interface.packet.id_in;

    // UI sent a request packet
    if( strcmp( (char*)name_rx, "isreq" ) == 0 )
    {
      // Configure the request 
      // use the data from our tracked variable
      interval_send_add_id( ui_interval_request.id, ui_interval_request.interval );
    }
  }
}

void serial_write( uint8_t *data, uint16_t len )
{
  Serial.write( data, len );
}