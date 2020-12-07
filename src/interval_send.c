#include "interval_send.h"
#include "interval_send_private.h"


send_info_t send_list[ SEND_COUNT_MAX ] = { 0 };
uint8_t send_list_count = 0;

void interval_send_tick( uint32_t global_ms )
{
    send_info_t *item;

    // Walk through the array of items to send
    for( uint8_t i = 0; i < SEND_COUNT_MAX; i++ )
    {
        // Use a pointer to the entry in the array for convenience
        item = &send_list[i];
        if( item->tracked )
        {
            // Check if the object needs to be sent right now
            if(    item->enabled
                && ( global_ms - item->last_sent > item->interval ) )
            {
                // Because we already have the pointer, we send it as untracked
                // eui_send_tracked does the lookup from the ID string
                eui_send_untracked( item->tracked );

                // Update the last-sent timestamp
                item->last_sent = global_ms;
            }
        }
    }

}


uint32_t interval_send_time_remaining_id( char *id, uint32_t global_ms )
{
    return interval_send_time_remaining( eui_find_tracked_object(id), global_ms );
}

uint32_t interval_send_time_remaining( eui_message_t *tracked, uint32_t global_ms )
{
    send_info_t *item = select_list_entry( tracked );

    if( item )
    {
        return global_ms - item->last_sent;
    }

    return 0;
}


uint32_t interval_send_last_timestamp_id( char *id )
{
    return interval_send_last_timestamp( eui_find_tracked_object(id) );
}

uint32_t interval_send_last_timestamp( eui_message_t *tracked )
{
    send_info_t *item = select_list_entry( tracked );

    if( item )
    {
        return item->last_sent;
    }

    return 0;
}


uint8_t interval_send_enabled_id( char *id )
{
    return interval_send_enabled( eui_find_tracked_object(id) );
}

uint8_t interval_send_enabled( eui_message_t *tracked )
{
    send_info_t *item = select_list_entry( tracked );

    if( item )
    {
        return item->enabled;   
    }

    return 0;
}



void interval_send_add_id( char *id, uint32_t interval )
{
    interval_send_add( eui_find_tracked_object(id), interval );
}

void interval_send_add( eui_message_t *tracked, uint32_t interval )
{
    // Check if it is already in the list
    send_info_t *item = select_list_entry( tracked );

    if( item )
    {
        // Just update the interval
        item->interval = interval;
    }
    else if( send_list_count < SEND_COUNT_MAX )
    {
        // Add the entry to the interval send list
        item = &send_list[send_list_count];

        item->tracked = tracked;
        item->interval = interval;
        item->last_sent = 0;
        item->enabled = 1;

        // TODO find an empty slot instead of assuming a sorted array
    }
}


void interval_send_remove_id( char *id )
{
    interval_send_remove( eui_find_tracked_object(id) );
}

void interval_send_remove( eui_message_t *tracked )
{
    send_info_t *item = select_list_entry( tracked );

    if( item )
    {
        // Remove the item from the list
        item->tracked = 0;
        item->interval = 0;
        item->last_sent = 0;
        item->enabled = 0;

        // TODO shuffle entries forward to fill the gap
    }

}


void interval_send_start_id( char *id )
{
    interval_send_start( eui_find_tracked_object(id) );
}

void interval_send_start( eui_message_t *tracked )
{
    send_info_t *item = select_list_entry( tracked );

    if( item )
    {
        item->enabled = 1;
    }
}


void interval_send_stop_id( char *id )
{
    interval_send_stop( eui_find_tracked_object(id) );
}

void interval_send_stop( eui_message_t *tracked )
{
    send_info_t *item = select_list_entry( tracked );

    if( item )
    {
        item->enabled = 0;
    }
}




send_info_t* select_list_entry( eui_message_t *tracked )
{
    send_info_t *item;

    // Walk the list and find the matching pointer
    for( uint8_t i = 0; i < SEND_COUNT_MAX; i++ )
    {
        item = &send_list[i];
        if( item->tracked == tracked )
        {
            return item;
        }
    }

    return 0;
}