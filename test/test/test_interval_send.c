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
    for( uint32_t i = 0; i < number; i++ )
    {
        simulate_tick();
    }
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
    for( uint32_t i = 0; i < 11; i++ )
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
    for( uint32_t i = 0; i < 11; i++ )
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
