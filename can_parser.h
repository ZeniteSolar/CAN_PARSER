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

/*
*  Cast messsage pointer to type TYPE and declare with name NAME
*/
#define CAN_DECLARE_MSG_OF_TYPE(TYPE, NAME, MSG)  \
   if (MSG == NULL || MSG->data == NULL)\
        return;\
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
* all modules have a hash (signature) and a topic 
*/
#define CAN_REGISTER_MODULES(...)                                \
    const can_module_t _can_modules[] = {                        \
        __VA_ARGS__};                                            \
    const can_modules_t can_modules =                            \
        {                                                        \
            .size = sizeof(_can_modules) / sizeof(can_module_t), \
            .module = _can_modules,                              \
    }
/*
* Define the parser 
*/
#define CAN_REGISTER_PARSER(NAME)                                     \
void can_parse_topics(const can_topics_t *topics, can_msg_t *msg);    \
void can_parse_##NAME(can_msg_t *msg) ;                               \
                                                                      \
    void can_parse_topics(const can_topics_t *topics, can_msg_t *msg) \
    {                                                                 \
        for (uint8_t i = 0; i < topics->size; i++)                    \
        {                                                             \
            if (topics->topics[i].id == msg->id)                      \
            {                                                         \
                topics->topics[i].parse(msg);                         \
                break; /* topics dont have more than one id*/         \
            }                                                         \
            if (i == (topics->size - 1))                              \
            {                                                         \
                printf("Unrecognized topic!");                        \
            }                                                         \
        }                                                             \
    }                                                                 \
                                                                      \
    void can_parse_##NAME(can_msg_t *msg)                             \
    {                                                                 \
                                                                      \
        for (uint8_t i = 0; i < can_modules.size; i++)                \
        {                                                             \
            if (can_modules.module[i].signature == msg->signature)    \
            {                                                         \
                can_parse_topics(can_modules.module[i].topics, msg);  \
                break; /* message dont have more than one module*/    \
            }                                                         \
            if (i == (can_modules.size - 1))                          \
            {                                                         \
                printf("Unrecognized module! with signature %d"       \
                    ,can_modules.module[i].signature);                \
            }                                                         \
        }                                                             \
    };
