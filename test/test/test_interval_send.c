#include "unity.h"
#include <string.h>

// MODULE UNDER TEST
#include "mock_electricui.h"
#include "interval_send.h"
#include "interval_send_private.h"

// DEFINITIONS 
 
// PRIVATE TYPES
 
// PRIVATE DATA
uint32_t simulated_timer = 0;

// These are only used for input/output ptrs with mocks
uint8_t test_variable = 0;

#define NUM_TRACKED_VARS 6

eui_message_t tracked_vars[ NUM_TRACKED_VARS ] = 
{
    EUI_UINT8( "test",  test_variable ),
    EUI_UINT8( "test2", test_variable ),
    EUI_UINT8( "test3", test_variable ),
    EUI_UINT8( "test4", test_variable ),
    EUI_UINT8( "test5", test_variable ),
    EUI_UINT8( "test6", test_variable ),
    // EUI_UINT8( "test7", test_variable ),
};

send_info_t send_list[ 5 ] = { 0 };


// PRIVATE FUNCTIONS
void simulate_tick( void )
{
    simulated_timer++;
}

void simulate_ticks( uint32_t number )
{
    simulated_timer += number;
}

// SETUP, TEARDOWN
void setUp(void)
{
    simulated_timer = 0;

    memset( &send_list, 0, sizeof(send_list) );
    interval_send_init( &send_list, (sizeof(send_list)/sizeof(*send_list)) );
}
 
void tearDown(void)
{

}

// TESTS
void test_add_id_basic( void )
{   
    // Add one sender via message ID
    // 10ms rate
    find_tracked_object_ExpectAndReturn( "test", &tracked_vars[0] );
    interval_send_add_id( "test", 10 );

    // Simulate our loop until it's auto-sent
    eui_send_untracked_Expect( &tracked_vars[0] );

    // Tick enough for one send
    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_add_id_missing( void )
{   
    // Add a sender with a message ID that doesn't exist
    find_tracked_object_ExpectAndReturn( "blah", 0 );
    interval_send_add_id( "blah", 10 );

    // We don't expect anything to send

    for( uint32_t i = 0; i < 20; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_add_basic( void )
{   
    // Add one sender via direct pointer
    // 10ms rate
    interval_send_add( &tracked_vars[0], 10 );

    // Simulate our loop until it's auto-sent
    eui_send_untracked_Expect( &tracked_vars[0] );

    // Tick enough for one send
    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_add_update_interval( void )
{   
    interval_send_add( &tracked_vars[0], 10 );

    // Simulate our loop until it's auto-sent once
    eui_send_untracked_Expect( &tracked_vars[0] );

    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }

    // Update it to 100ms
    interval_send_add( &tracked_vars[0], 100 );
    eui_send_untracked_Expect( &tracked_vars[0] );

    for( uint32_t i = 0; i < 100; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_remove_id( void )
{   
    find_tracked_object_ExpectAndReturn( "test", &tracked_vars[0] );
    interval_send_add_id( "test", 10 );

    find_tracked_object_ExpectAndReturn( "test", &tracked_vars[0] );
    interval_send_remove_id( "test" );

    // Tick enough for one send
    for( uint32_t i = 0; i < 20; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_remove( void )
{
    interval_send_add( &tracked_vars[0], 10 );
    interval_send_remove( &tracked_vars[0] );

    // We don't expect sends because it's been removed
    for( uint32_t i = 0; i < 100; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_add_update_remove( void )
{   
    // Implicit remove by asking for an interval of 0

    interval_send_add( &tracked_vars[0], 10 );

    eui_send_untracked_Expect( &tracked_vars[0] );

    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }

    interval_send_add( &tracked_vars[0], 0 );

    for( uint32_t i = 0; i < 100; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_empty( void )
{   
    // We haven't setup any items, so nothing should be sent

    // Tick across the entire counter space
    // For test perf reasons, we tick the counter 65k steps before
    // checking if the library wants to send anything
    // 0xFFFFFFFF worth of ticks, in 0xFFFF batches 
    for( uint32_t i = 0; i < 0xFFFF; i++ )
    {
        simulate_ticks( 0xFFFF);
        interval_send_tick( simulated_timer ); 
    }
}

void test_long_run( void )
{   
    // Run the sender with millisecond ticks over a 10-minute span
    interval_send_add( &tracked_vars[0], 1000 );

    const uint8_t simulate_minutes = 10;
    const uint32_t simulate_ms = (simulate_minutes * 60 * 1000);

    // Over an expected 10-minute period, how many packets sent?
    // 1000ms is 1/second, therefore min -> seconds
    const uint32_t expect_n_send = simulate_minutes * 60;

    for( uint32_t i = 0; i < expect_n_send; i++ )
    {
        eui_send_untracked_Expect( &tracked_vars[0] );
    }

    for( uint32_t i = 0; i <= simulate_ms; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_stop_start_id( void )
{   
    find_tracked_object_ExpectAndReturn( "test", &tracked_vars[0] );
    interval_send_add_id( "test", 10 );
    eui_send_untracked_Expect( &tracked_vars[0] );

    // Tick enough for one send
    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }

    // Disable the sender
    find_tracked_object_ExpectAndReturn( "test", &tracked_vars[0] );
    interval_send_stop_id( "test" );

    for( uint32_t i = 0; i < 50; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }

    // Re-enable the sender
    find_tracked_object_ExpectAndReturn( "test", &tracked_vars[0] );
    interval_send_start_id( "test" );
    eui_send_untracked_Expect( &tracked_vars[0] );

    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_stop_start( void )
{   
    // When added they automatically start
    interval_send_add( &tracked_vars[0], 10 );
    eui_send_untracked_Expect( &tracked_vars[0] );

    // Tick enough for one send
    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }

    // Disable the sender
    interval_send_stop( &tracked_vars[0] );

    for( uint32_t i = 0; i < 50; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }

    // Re-enable the sender
    interval_send_start( &tracked_vars[0] );
    eui_send_untracked_Expect( &tracked_vars[0] );

    for( uint32_t i = 0; i < 10; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}

void test_add_many( void )
{   
    // Add multiple senders
    interval_send_add( &tracked_vars[0], 10 );
    interval_send_add( &tracked_vars[1], 10 );
    interval_send_add( &tracked_vars[2], 10 );
    interval_send_add( &tracked_vars[3], 10 );

    for( uint8_t i = 0; i < NUM_TRACKED_VARS; i++ )
    {
        printf("Setting up %i with ptr:%p\n", i, &tracked_vars[i]);
    }

    // Simulate our loop until it's auto-sent
    eui_send_untracked_Expect( &tracked_vars[0] );
    eui_send_untracked_Expect( &tracked_vars[1] );
    eui_send_untracked_Expect( &tracked_vars[2] );
    eui_send_untracked_Expect( &tracked_vars[3] );

    // printf("b\n");
    // eui_send_untracked_Expect( &tracked_vars[1] );
    // printf("c\n");
    // eui_send_untracked_Expect( &tracked_vars[2] );
    // printf("d\n");
    // eui_send_untracked_Expect( &tracked_vars[3] );
    // printf("e\n");
    // eui_send_untracked_Expect( &tracked_vars[4] );
    // printf("f\n");
    // eui_send_untracked_Expect( &tracked_vars[5] );
    // printf("h\n");
    // eui_send_untracked_Expect( &tracked_vars[6] );


    // Tick enough for one send
    for( uint32_t i = 0; i < 11; i++ )
    {
        simulate_tick();
        interval_send_tick( simulated_timer ); 
    }
}
