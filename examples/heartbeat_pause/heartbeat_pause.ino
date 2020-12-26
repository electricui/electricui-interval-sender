/*
 *  Tracks inbound heartbeats to disable sending.
*/

#include "electricui.h"
#include "interval_send.h"

// Follows the same behaviour as the hello-blink
uint8_t   blink_enable = 1;
uint8_t   led_state    = 0;
uint16_t  glow_time    = 200;
uint32_t  led_timer    = 0;

char device_name[] = "Manual Send HB";

#define HEARTBEAT_EXPECTED_MS 800

uint32_t last_heartbeat = 0;
uint8_t heartbeat_ok_count = 0;

interval_send_requested_t iv_send_pool[5] = { 0 };

// Use the EUI_INTERFACE_CB macro to include the diagnostics callback 
eui_interface_t serial_interface = EUI_INTERFACE_CB( &serial_write, &eui_callback ); 

eui_message_t tracked_variables[] = 
{
  EUI_UINT16( "led_state",  led_state ),
  EUI_UINT16( "lit_time",   glow_time ),
  EUI_CHAR_ARRAY( "name",   device_name ),
};

void setup() 
{
  Serial.begin( 115200 );
  pinMode( LED_BUILTIN, OUTPUT );

  eui_setup_interface( &serial_interface );
  EUI_TRACK( tracked_variables );
  eui_setup_identifier( "status", 6 );

  led_timer = millis();

  // Setup the tracked sender pool, then add the led_state variable to it
  interval_send_init( &iv_send_pool, 5 );
  interval_send_add_id( "led_state", 50 );
}

void loop() 
{
  // Monitor heartbeat status
  if( millis() - last_heartbeat > HEARTBEAT_EXPECTED_MS )
  {
    heartbeat_ok_count = 0;
  }
  
  bool heartbeat_ok = ( heartbeat_ok_count > 3 );

  // Library wide enable/disable controlled by heartbeat status
  interval_send_enable( heartbeat_ok );

  // Process inbound serial data
  while( Serial.available() > 0 )  
  {  
    eui_parse( Serial.read(), &serial_interface );
  }

  interval_send_tick( millis() );

  // Standard hello_blink demo
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

    // Watch for the internal heartbeat message
    if( strcmp( (char*)name_rx, EUI_INTERNAL_HEARTBEAT ) == 0 )
    {
      if( millis() - last_heartbeat < HEARTBEAT_EXPECTED_MS )
      {
        // Increment the counter, but clamp to 20
        heartbeat_ok_count = min(heartbeat_ok_count++, 20);
      }

      last_heartbeat = millis();
    }
  }
}

// EUI output over serial
void serial_write( uint8_t *data, uint16_t len )
{
  Serial.write( data, len );
}