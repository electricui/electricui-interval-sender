#ifndef EUI_INTERVAL_SEND_H
#define EUI_INTERVAL_SEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#include "electricui.h"

typedef struct {
    eui_message_t *tracked;
    uint32_t interval;
    uint32_t last_sent;
    uint8_t enabled;            
} send_info_t;

typedef struct {
	uint32_t interval;
    char id[EUI_MAX_MSGID_SIZE];
} interval_send_requested_t;

void interval_send_init( send_info_t *send_pool, uint8_t pool_size );

void interval_send_enable( bool enabled );

void interval_send_tick( uint32_t global_ms );

uint32_t interval_send_time_remaining_id( char *id, uint32_t global_ms );
uint32_t interval_send_time_remaining( eui_message_t *tracked, uint32_t global_ms );

uint32_t interval_send_last_timestamp_id( char *id );
uint32_t interval_send_last_timestamp( eui_message_t *tracked );

uint8_t interval_send_enabled_id( char *id );
uint8_t interval_send_enabled( eui_message_t *tracked );

void interval_send_add_id( char *id, uint32_t interval );
void interval_send_add( eui_message_t *tracked, uint32_t interval );

void interval_send_remove_id( char *id );
void interval_send_remove( eui_message_t *tracked );

void interval_send_start_id( char *id );
void interval_send_start( eui_message_t *tracked );

void interval_send_stop_id( char *id );
void interval_send_stop( eui_message_t *tracked );

#ifdef __cplusplus
}
#endif


#endif //end EUI_INTERVAL_SEND_H