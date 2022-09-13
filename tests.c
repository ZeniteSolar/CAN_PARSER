#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>

#include "can_ids.h"
#include "can_parser.h"

uint32_t timeout = 0;

struct test_mic_t
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
struct test_mic_t test_mic;

static void parse_mic_state(can_msg_t *msg)
{

    printf("mic state\n");
}
static void parse_mic_motor(can_msg_t *msg)
{
    CAN_DECLARE_MSG_OF_TYPE(can_mic19_motor_msg_t, mic_motor, msg);

    test_mic.signature = mic_motor->signature;
    test_mic.motor.motor_on = mic_motor->motor.motor_on;
    test_mic.motor.dms_on = mic_motor->motor.dms_on;
    test_mic.motor.reverse = mic_motor->motor.reverse;
    test_mic.d = mic_motor->d;
    test_mic.i = mic_motor->i;
}

static void parse_mswi_state(can_msg_t *msg)
{
    printf("mswi state\n");
}

struct test_mswi_t
{
    uint8_t signature; // Senders signature. Units:
    struct
    { // Motor state
        uint8_t motor_on : 1;
        uint8_t dms_on : 1;
        uint8_t _unused : 6;
    } motor;
    uint8_t d; // Motor Duty Cycle. Units: %
    uint8_t i; // Motor Soft Start. Units: %
} test_mswi;

static void parse_mswi_motor(can_msg_t *msg)
{
    CAN_DECLARE_MSG_OF_TYPE(can_mswi19_motor_msg_t, mswi_motor, msg);
    test_mswi.signature = mswi_motor->signature;
    test_mswi.d = mswi_motor->d;
    test_mswi.i = mswi_motor->i;
    test_mswi.motor.dms_on = mswi_motor->motor.dms_on;
    test_mswi.motor.motor_on = mswi_motor->motor.motor_on;
}

CAN_REGISTER_TOPICS(mic,
                    {CAN_MSG_MIC19_STATE_ID, &parse_mic_state},
                    {CAN_MSG_MIC19_MOTOR_ID, &parse_mic_motor});
CAN_REGISTER_TOPICS(mswi,
                    {CAN_MSG_MSWI19_STATE_ID, &parse_mswi_state},
                    {CAN_MSG_MSWI19_MOTOR_ID, &parse_mswi_motor});

CAN_REGISTER_MODULES(mam_rx, 1, {CAN_SIGNATURE_MIC19, &CAN_TOPICS_NAME(mic), 100},
                     {CAN_SIGNATURE_MSWI19, &CAN_TOPICS_NAME(mswi), 100});

void can_handle_timeout(uint8_t signature)
{
    if (signature == CAN_SIGNATURE_MIC19)
        timeout = 1;
}

void test_msg_parsing(void)
{

    can_msg_t msg;
    for (int i = 0; i <= 255; i++)
    {
        /*
         *   Test case MIC -> MAM msg
         */
        msg.raw[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_MIC19;
        msg.raw[CAN_MSG_MIC19_MOTOR_D_BYTE] = i;
        msg.raw[CAN_MSG_MIC19_MOTOR_I_BYTE] = 255 - i;

        msg.raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] =
            (((i % 2) == 0) << CAN_MSG_MIC19_MOTOR_MOTOR_MOTOR_ON_BIT);

        msg.raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
            (((i % 2) == 0) << CAN_MSG_MIC19_MOTOR_MOTOR_DMS_ON_BIT);

        msg.raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
            (((i % 2) != 0) << CAN_MSG_MIC19_MOTOR_MOTOR_REVERSE_BIT);
        msg.id = CAN_MSG_MIC19_MOTOR_ID;

        printf("parsing %d ...\n", msg.signature);

        can_parser(&CAN_PARSER_NAME(mam_rx), &msg);

        printf("test_mic.signature: %d\n", test_mic.signature);
        printf("test_mic.d: %d\n", test_mic.d);
        printf("test_mic.i: %d\n", test_mic.i);
        printf("test_mic.motor.motor_on: %d\n", test_mic.motor.motor_on);
        printf("test_mic.motor.dms_on: %d\n", test_mic.motor.dms_on);
        printf("test_mic.motor.reverse: %d\n", test_mic.motor.reverse);

        assert(CAN_SIGNATURE_MIC19 == test_mic.signature);
        assert(i == test_mic.d);
        assert(255 - i == test_mic.i);
        assert(((i % 2) == 0) == test_mic.motor.motor_on);
        assert(((i % 2) == 0) == test_mic.motor.dms_on);
        assert(((i % 2) != 0) == test_mic.motor.reverse);

        /*
         *   Test case MSWI -> MAM msg
         */

        msg.id = CAN_MSG_MSWI19_MOTOR_ID;

        msg.raw[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_MSWI19;

        msg.raw[CAN_MSG_MSWI19_MOTOR_D_BYTE] = 255 - i;

        msg.raw[CAN_MSG_MSWI19_MOTOR_I_BYTE] = i;
        msg.raw[CAN_MSG_MSWI19_MOTOR_MOTOR_BYTE] = 0;
        msg.raw[CAN_MSG_MSWI19_MOTOR_MOTOR_BYTE] |= (((i % 2) == 0) << CAN_MSG_MSWI19_MOTOR_MOTOR_MOTOR_ON_BIT);
        msg.raw[CAN_MSG_MSWI19_MOTOR_MOTOR_BYTE] |= (((i % 2) != 0) << CAN_MSG_MSWI19_MOTOR_MOTOR_DMS_ON_BIT);

        printf("parsing %d ...\n", msg.signature);

        can_parser(&CAN_PARSER_NAME(mam_rx), &msg);

        printf("test_mswi.signature: %d\n", test_mswi.signature);
        printf("test_mswi.d: %d\n", test_mswi.d);
        printf("test_mswi.i: %d\n", test_mswi.i);
        printf("test_mswi.motor.motor_on: %d\n", test_mswi.motor.motor_on);
        printf("test_mswi.motor.dms_on: %d\n", test_mswi.motor.dms_on);

        assert(CAN_SIGNATURE_MSWI19 == test_mswi.signature);
        assert(i == test_mswi.i);
        assert(255 - i == test_mswi.d);
        assert(((i % 2) == 0) == test_mswi.motor.motor_on);
        assert(((i % 2) != 0) == test_mswi.motor.dms_on);
    }
}

void test_timeout(void)
{
    can_msg_t msg;
    msg.id = CAN_MSG_GENERIC_STATE_ID;
    printf("\n==> Testing if timeout is triggered\n");
    timeout = 0;
    for (int i = 1; i <= 100; i++)
    {
        msg.signature = CAN_SIGNATURE_MSWI19;
        can_update_timeout(&CAN_PARSER_NAME(mam_rx));
        can_parser(&CAN_PARSER_NAME(mam_rx), &msg);

        // Testing if timeout_handler is triggered in 100
        printf("i: %d\n", i);
        if (i == 100)
        {
            assert(timeout == 1);
            timeout = 0;
        }
        else
        {
            assert(timeout == 0);
        }
    }
    printf("\n==>Testing if timeout have false triggers\n");
    // Test if timeout have false triggers
    timeout = 0;
    for (int i = 1; i <= 1000; i++)
    {
        msg.signature = CAN_SIGNATURE_MSWI19;
        can_update_timeout(&CAN_PARSER_NAME(mam_rx));
        can_parser(&CAN_PARSER_NAME(mam_rx), &msg);

        msg.signature = CAN_SIGNATURE_MIC19;

        can_parser(&CAN_PARSER_NAME(mam_rx), &msg);
        printf("i: %d\n", i);
        assert(timeout == 0);
    }
}

int main(void)
{
    test_timeout();
    test_msg_parsing();
}
