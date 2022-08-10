#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>

#include "can_ids.h"
#include "can_parser.h"

struct test_t
{
    uint8_t signature; // Senders signature. Units:
    struct
    { // Motor state
        uint8_t motor_on : 1;
        uint8_t dms_on : 1;
        uint8_t reverse : 1;
        uint8_t _unused : 5;
    } motor;
    uint8_t d; // Motor Duty Cycle. Units: %
    uint8_t i; // Motor Soft Start. Units: %
};
struct test_t test;

static void parse_mic_state(can_msg_t *msg)
{

    printf("mic state");
}
static void parse_mic_motor(can_msg_t *msg)
{
    CAN_DECLARE_MSG_OF_TYPE(can_mic19_motor_msg_t, mic_motor, msg);

    test.signature = mic_motor->signature;
    test.motor.motor_on = mic_motor->motor.motor_on;
    test.motor.dms_on = mic_motor->motor.dms_on;
    test.motor.reverse = mic_motor->motor.reverse;
    test.d = mic_motor->d;
    test.i = mic_motor->i;
}

static void parse_mswi_state(can_msg_t *msg)
{
    printf("mswi state");
}

static void parse_mswi_motor(can_msg_t *msg)
{
    printf("mswi motor");
}

CAN_REGISTER_TOPICS(mic,
                    {CAN_MSG_MIC19_STATE_ID, &parse_mic_state},
                    {CAN_MSG_MIC19_MOTOR_ID, &parse_mic_motor});
CAN_REGISTER_TOPICS(mswi,
                    {CAN_MSG_MSWI19_STATE_ID, &parse_mswi_state},
                    {CAN_MSG_MSWI19_MOTOR_ID, &parse_mswi_motor});

CAN_REGISTER_MODULES({CAN_SIGNATURE_MIC19, &CAN_TOPICS_NAME(mic)},
                     {CAN_SIGNATURE_MSWI19, &CAN_TOPICS_NAME(mswi)});

CAN_REGISTER_PARSER(mam);

int main()
{

    /*
     *   Test case
     */
    can_msg_t msg;

    msg.raw[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_MIC19;
    msg.raw[CAN_MSG_MIC19_MOTOR_D_BYTE] = 128;
    msg.raw[CAN_MSG_MIC19_MOTOR_I_BYTE] = 255;

    msg.raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] =
        ((1) << CAN_MSG_MIC19_MOTOR_MOTOR_MOTOR_ON_BIT);

    msg.raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
        ((0) << CAN_MSG_MIC19_MOTOR_MOTOR_DMS_ON_BIT);

    msg.raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
        ((1) << CAN_MSG_MIC19_MOTOR_MOTOR_REVERSE_BIT);
    msg.id = CAN_MSG_MIC19_MOTOR_ID;

    can_parse_mam(&msg);

    printf("test.signature: %d\n", test.signature);
    printf("test.d: %d\n", test.d);
    printf("test.i: %d\n", test.i);
    printf("test.motor.motor_on: %d\n", test.motor.motor_on);
    printf("test.motor.dms_on: %d\n", test.motor.dms_on);
    printf("test.motor.reverse: %d\n", test.motor.reverse);

    assert(CAN_SIGNATURE_MIC19 == test.signature);
    assert(128 == test.d);
    assert(255 == test.i);
    assert(1 == test.motor.motor_on);
    assert(0 == test.motor.dms_on);
    assert(1 == test.motor.reverse);
}
