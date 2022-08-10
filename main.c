#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "can_ids.h"

typedef struct
{
    union
    {
        uint8_t raw[8];
        struct
        {
            uint8_t signature;
            struct
            {
                uint8_t motor_on : 1;
                uint8_t dms : 1;
                uint8_t reverse : 1;
            } motor;
            uint8_t D;
            uint8_t I;
        };
    };
} can_mic_motor_msg_t;

typedef struct
{
    uint8_t id;
    union
    {
        uint8_t raw[8];
        struct
        {
            uint8_t signature;
            uint8_t data[7];
        };
    };

} can_msg_t;

typedef struct
{
    uint8_t id;
    void (*parse)(can_msg_t *msg);
} can_topic_t;

typedef struct
{
    uint8_t size;
    const can_topic_t *topics;
} can_topics_t;

typedef struct
{
    struct
    {
        uint8_t signature;
        const can_topics_t *topics;
    };
} can_module_t;

typedef struct
{
    uint8_t size;
    const can_module_t *module;
} can_modules_t;

#define CAN_TOPICS_NAME(NAME) CAN_TOPICS_##NAME

#define CAN_REGISTER_TOPICS(NAME, ...)                          \
    const can_topic_t CAN_TOPIC_##NAME[] = {                    \
        __VA_ARGS__};                                           \
    const can_topics_t CAN_TOPICS_NAME(NAME) = {                \
        .size = sizeof(CAN_TOPIC_##NAME) / sizeof(can_topic_t), \
        .topics = CAN_TOPIC_##NAME,                             \
    }

#define CAN_REGISTER_MODULES(...)                                \
    const can_module_t _can_modules[] = {                        \
        __VA_ARGS__};                                            \
    const can_modules_t can_modules =                            \
        {                                                        \
            .size = sizeof(_can_modules) / sizeof(can_module_t), \
            .module = _can_modules,                              \
    }

static void parse_mic_state(can_msg_t *msg)
{

    printf("mic state");
}
static void parse_mic_motor(can_msg_t *msg)
{
    if (msg == NULL || msg->data == NULL)
    {
        return;
    }

    can_mic_motor_msg_t *mic_motor = (can_mic_motor_msg_t *)(void *)msg->data;

    for (int i = 0; i < 7; i++)
    {
        printf("motor[%d] %d\n", i, mic_motor->raw[i]);
    }
    printf("\nmic_motor D: %d", mic_motor->D);
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

void can_parse_topics(const can_topics_t *topics, can_msg_t *msg)
{
    for (uint8_t i; i < topics->size; i++)
    {
        if (topics->topics[i].id == msg->id)
        {
            topics->topics[i].parse(msg);
            break; // topics dont have more than one id
        }
        if (i == (topics->size - 1))
        {
            printf("Unrecognized topic!");
        }
    }
}

void can_parse(can_msg_t *msg)
{

    for (uint8_t i; i < can_modules.size; i++)
    {
        if (can_modules.module[i].signature == msg->signature)
        {
            can_parse_topics(can_modules.module[i].topics, msg);
            break; // message dont have more than one module
        }
        if (i == (can_modules.size - 1))
        {
            printf("Unrecognized module!");
        }
    }
}

int main()
{
    can_msg_t msg = {
        .signature = CAN_SIGNATURE_MIC19,
        .id = CAN_MSG_MIC19_MOTOR_ID,
        .data[2] = 129};
    can_parse(&msg);
}

