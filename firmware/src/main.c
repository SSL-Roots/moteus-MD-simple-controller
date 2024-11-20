/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

void TMR2_EventHandler(uint32_t status, uintptr_t context); 

uint8_t can_msg[24];

typedef enum
{
    APP_STATE_CAN_RECEIVE,
    APP_STATE_CAN_TRANSMIT,
    APP_STATE_CAN_IDLE,
    APP_STATE_CAN_USER_INPUT,
    APP_STATE_CAN_XFER_SUCCESSFUL,
    APP_STATE_CAN_XFER_ERROR
} APP_STATES;

/* Variable to save application state */
static APP_STATES state = APP_STATE_CAN_USER_INPUT;
/* Variable to save Tx/Rx transfer status and context */
static uint32_t status = 0;
static uint32_t xferContext = 0;
/* Variable to save Tx/Rx message */
//static uint32_t messageID = 0;
//static uint8_t message[64];
//static uint8_t messageLength = 0;
//static uint8_t rx_message[64];
//static uint32_t rx_messageID = 0;
//static uint8_t rx_messageLength = 0;
//static uint32_t timestamp = 0;
//static CANFD_MSG_RX_ATTRIBUTE msgAttr = CANFD_MSG_RX_DATA_FRAME;

void APP_CAN_Callback(uintptr_t context)
{
    xferContext = context;

    /* Check CAN Status */
    status = CAN1_ErrorGet();

    if ((status & (CANFD_ERROR_TX_RX_WARNING_STATE | CANFD_ERROR_RX_WARNING_STATE |
                   CANFD_ERROR_TX_WARNING_STATE | CANFD_ERROR_RX_BUS_PASSIVE_STATE |
                   CANFD_ERROR_TX_BUS_PASSIVE_STATE | CANFD_ERROR_TX_BUS_OFF_STATE)) == CANFD_ERROR_NONE)
    {
        switch ((APP_STATES)context)
        {
            case APP_STATE_CAN_RECEIVE:
            case APP_STATE_CAN_TRANSMIT:
            {
                state = APP_STATE_CAN_XFER_SUCCESSFUL;
                break;
            }
            default:
                break;
        }
    }
    else
    {
        state = APP_STATE_CAN_XFER_ERROR;
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    TMR2_CallbackRegister(TMR2_EventHandler, 0);
    TMR2_Start();
    CAN1_CallbackRegister( APP_CAN_Callback, (uintptr_t)APP_STATE_CAN_TRANSMIT, 1 );
    
    can_msg[0]  = 0x01; //write int8 1bite
    can_msg[1]  = 0x00; // adr 0x00(mode))
//    can_msg[0]  = 0x00;
//    can_msg[1]  = 0xFF;
    can_msg[2]  = 0x0a; //mode = position
    can_msg[3]  = 0x04; //wite int16
    can_msg[4]  = 0x02; // write 2byte
    can_msg[5]  = 0x20; //adr 0x20(position~)
    can_msg[6]  = 0x00; // position = 0x8000(=nan)
    can_msg[7]  = 0x80;
    can_msg[8]  = 0x00; // adr 0x21 speed = 0x2000
    can_msg[9]  = 0x20;
    can_msg[10] = 0x04; //write int16 
    can_msg[11] = 0x01; //write 1byte
    can_msg[12] = 0x25; //adr 0x25(=max torque)
    can_msg[13] = 0x01; //max torque = 0x0010
    can_msg[14] = 0x00;
    can_msg[15] = 0x04; //write int16
    can_msg[16] = 0x01;
    can_msg[17] = 0x27;
    can_msg[18] = 0x00;
    can_msg[19] = 0x80;
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

void TMR2_EventHandler(uint32_t status, uintptr_t context){
    LATBbits.LATB9 = ~LATBbits.LATB9;
    if(CAN1_MessageTransmit(0x8001, 20, &can_msg[0], 1, CANFD_MODE_FD_WITHOUT_BRS, CANFD_MSG_TX_DATA_FRAME) == false){
        LATCbits.LATC7  = 0;
    }else{
        LATCbits.LATC7  = 1;
    }
}
/*******************************************************************************
 End of File
*/

