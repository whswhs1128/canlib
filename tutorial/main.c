// CANLIB TUTORIAL - Things to cover:
// * Library organization: drivers, message handling, utils
// * CAN struct
//
// * message transmission and reception
// * can_init()
// * can_send()
// * can_msg_handler() and interrupts
//
// * message types, again
// * message types, but how to format things (can_common.h and can_common.c)
// * most boards will only care about one or two message types
// * new boards (eg. recovery) that send things over CAN will have to add these to canlib 
//   (both the message types themselves and the associated formatting functions).
//
//
// If we have time:
// * utils: transmit and receive buffers
// * utils: timing parameters
// * millis() - not in canlib but will be soon?
//
// NOTE: this is for example only - this will not run in MPLAB without modification
//=======================================================================================

#include "can.h"
#include "can_common.h"
#include "pic18f26k83/pic18f26k83_can.h"
#include "util/timing_util.h"

// Currently board-defined in each repo, will be moved to canlib
#include "timer.h"

// General mplab stuff
#include <xc.h>

// Just for fun
#define LED_ON() (PORTAbits.RA0 = 0)
#define LED_OFF() (PORTAbits.RA0 = 1)

#define BOARD_UNIQUE_ID 0x01

void can_msg_handler(can_msg_t *msg);

int main(void) {
    // set up whatever microcontroller peripherals you need here
    // ...

    // set up global interrupts - needed for receive
    INTCON0bits.GIE = 1;

    // Set up CAN TX pins - using pinout on injector board in this case
    TRISC0 = 0;
    RC0PPS = 0x33;

    // Set up CAN RX pins
    TRISC1 = 1;
    ANSELC1 = 0;
    CANRXPPS = 0x11;

    // set up CAN module
    can_timing_t can_timing;
    can_generate_timing_params(_XTAL_FREQ, &can_timing);
    can_init(&can_timing, can_msg_handler);

    while (1) {
        // The most basic way - don't do this
        can_msg_t msg;
        msg.sid = 0xaa;
        msg.data[0] = 0xca;
        msg.data[1] = 0xfe;
        msg.data_len = 2;

        can_send(&msg);

        __delay_ms(100);

        // The less dumb way
        can_msg_t sensor_msg;
        uint16_t sensor_data = 7;   // whatever
        enum SENSOR_ID sensor_id = SENSOR_PRESSURE_OX;
        build_analog_data_msg(millis(), sensor_id, sensor_data, &sensor_msg);

        can_send(&sensor_msg);

        __delay_ms(100);
    }

    // should never get here
    return 0;
}

static void interrupt interrupt_handler() {
    // check whatever interrupt flags you need to
    // for CAN, bits in PIR5 get set
    if (PIR5) {
        // This actually deals with reading messages from the CAN module registers.
        // When done, it'll call the message handling function you passed to CAN init.
        can_handle_interrupt();
    }
}

static void can_msg_handler(can_msg_t *msg) {
    // typically looks something like:
    uint16_t msg_type = get_message_type(msg);
    switch (msg_type) {
        // do stuff based on message type
        // etc.
    }
}
