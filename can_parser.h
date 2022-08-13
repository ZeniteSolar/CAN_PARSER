#ifndef CAN_PARSER_H
#define CAN_PARSER_H
#include "can_parser_types.h"

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
    uint32_t time_without_messages; // Time passed without messages
    const uint32_t timeout;         // Time that has to pass without messages
    uint32_t connected;
} can_timeout_t;

typedef struct
{
    struct
    {
        const uint8_t signature;
        const can_topics_t *topics;
        can_timeout_t timeout;
    };
} can_module_t;

typedef struct
{
    uint8_t size;
    can_module_t *module;
} can_modules_t;

/*
 *   can topics namespace
 */
#define CAN_TOPICS_NAME(NAME) CAN_TOPICS_##NAME

/*
 *  use to set a timeout when registering a module (see CAN_REGISTER_MODULES example)
 */

#define CAN_SET_TIMEOUT(TIMEOUT)                            \
    {                                                       \
        0, (uint32_t)TIMEOUT, 0 \
    }

/*
 *  Cast messsage pointer to type TYPE and declare with name NAME
 */
#define CAN_DECLARE_MSG_OF_TYPE(TYPE, NAME, MSG) \
    if (MSG == NULL || MSG->data == NULL)        \
        return;                                  \
    TYPE *NAME = (TYPE *)(void *)msg->raw

/*
 *   Register a new group of topics,
 * all topics have a hash (id) and a callback (parse)
 */
#define CAN_REGISTER_TOPICS(NAME, ...)                          \
    const can_topic_t CAN_TOPIC_##NAME[] = {                    \
        __VA_ARGS__};                                           \
    const can_topics_t CAN_TOPICS_NAME(NAME) = {                \
        .size = sizeof(CAN_TOPIC_##NAME) / sizeof(can_topic_t), \
        .topics = CAN_TOPIC_##NAME,                             \
    }
/*
 *   Register a new group of modules
 * all modules have a hash (signature) and a group of topics
 * Example: CAN_REGISTER_MODULES({signature, &topics, CAN_SET_TIMEOUT(seconds to timeout)})
 */
#define CAN_REGISTER_MODULES(...)                                \
    can_module_t _can_modules[] = {                        \
        __VA_ARGS__};                                            \
    const can_modules_t can_modules =                            \
        {                                                        \
            .size = sizeof(_can_modules) / sizeof(can_module_t), \
            .module = _can_modules,                              \
    }
/*
 * Define the parser
 * \param NAME is the name of the parser
 * \param F_CLK is the frequecny that the parser will be called by the user(it is used to calculate timeouts)
 */

#define CAN_REGISTER_PARSER(NAME, F_CLK)                                                                         \
    void can_parse_topics(const can_topics_t *topics, can_msg_t *msg);                                           \
    void can_parse_##NAME(can_msg_t *msg);                                                                          \
    void can_check_timeout(can_timeout_t *timeout, const uint32_t f_clk);                                       \
                                                                                                                 \
    void can_check_timeout(can_timeout_t *timeout, const uint32_t f_clk)                                         \
    {                                                                                                            \
        if (timeout->timeout == 0)                                                                               \
        {                                                                                                        \
            return;                                                                                              \
        }                                                                                                        \
        if (++timeout->time_without_messages >= timeout->timeout * f_clk)                                        \
        {                                                                                                        \
            timeout->connected = 0;                                                                              \
        }                                                                                                        \
    }                                                                                                            \
    void can_parse_topics(const can_topics_t *topics, can_msg_t *msg)                                            \
    {                                                                                                            \
        for (uint8_t i = 0; i < topics->size; i++)                                                               \
        {                                                                                                        \
            if (topics->topics[i].id == msg->id)                                                                 \
            {                                                                                                    \
                topics->topics[i].parse(msg);                                                                    \
                break; /* topics dont have more than one id*/                                                    \
            }                                                                                                    \
            if (i == (topics->size - 1))                                                                         \
            {                                                                                                    \
                printf("Unrecognized topic!");                                                                   \
            }                                                                                                    \
        }                                                                                                        \
    }                                                                                                            \
                                                                                                                 \
    void can_parse_##NAME(can_msg_t *msg)                                                                           \
    {                                                                                                            \
        static const uint32_t f_clk = 5;                                                                         \
        for (uint8_t _i = 0; _i < can_modules.size; _i++)                                                        \
        {                                                                                                        \
            printf("timeout %d", can_modules.module[_i].timeout.timeout);                                        \
            if (can_modules.module[_i].signature == msg->signature)                                              \
            {                                                                                                    \
                can_modules.module[_i].timeout.connected = 1;                                                    \
                can_parse_topics(can_modules.module[_i].topics, msg);                                            \
            }                                                                                                    \
            else                                                                                                 \
            {                                                                                                    \
                can_check_timeout(&can_modules.module[_i].timeout, f_clk);                                       \
                printf("%d is: %d", can_modules.module[_i].signature, can_modules.module[_i].timeout.connected); \
                if (_i == (can_modules.size - 1))                                                                \
                {                                                                                                \
                    printf("Unrecognized module! with signature %d", can_modules.module[_i].signature);          \
                }                                                                                                \
            }                                                                                                    \
        }                                                                                                        \
    }
#endif // CAN_PARSER_H