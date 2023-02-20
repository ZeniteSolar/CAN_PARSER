#include <stdio.h>
#include <can_ids.h>
#include <can_parser.h>

static void parse_mic_motor(can_msg_t *msg)
{
    can_mic19_motor_msg_t *mic_motor = (can_mic19_motor_msg_t *)msg->raw;

    printf("mic motor signature: %d\n", mic_motor->signature);
    printf("mic motor motor on: %d\n", mic_motor->motor.motor_on);
    printf("mic motor dms on: %d\n", mic_motor->motor.dms_on);
    printf("mic motor reverse: %d\n", mic_motor->motor.reverse);
    printf("mic motor d: %d\n", mic_motor->d);
    printf("mic motor i: %d\n", mic_motor->i);
}

void can_handle_timeout(uint8_t signature)
{
    if (signature == CAN_SIGNATURE_MIC19)
        printf("To much time without mam messages");
}

void get_message(can_msg_t *msg)
{
    msg->raw[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] = CAN_SIGNATURE_MIC19;
    msg->raw[CAN_MSG_MIC19_MOTOR_D_BYTE] = 128;
    msg->raw[CAN_MSG_MIC19_MOTOR_I_BYTE] = 5;

    msg->raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] =
        1 << CAN_MSG_MIC19_MOTOR_MOTOR_MOTOR_ON_BIT;

    msg->raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
        0 << CAN_MSG_MIC19_MOTOR_MOTOR_DMS_ON_BIT;

    msg->raw[CAN_MSG_MIC19_MOTOR_MOTOR_BYTE] |=
        1 << CAN_MSG_MIC19_MOTOR_MOTOR_REVERSE_BIT;
    msg->id = CAN_MSG_MIC19_MOTOR_ID;
}

int main(void)
{
    can_msg_t msg;

    get_message(&msg);

    CAN_REGISTER_TOPICS(mic,
                        {CAN_MSG_MIC19_MOTOR_ID, &parse_mic_motor});
    CAN_REGISTER_MODULES(
        mam_rx, 1, {CAN_SIGNATURE_MIC19, &CAN_TOPICS_NAME(mic), 100}, );

    can_parser(&CAN_PARSER_NAME(mam_rx), &msg);

    can_update_timeout(&CAN_PARSER_NAME(mam_rx));
}