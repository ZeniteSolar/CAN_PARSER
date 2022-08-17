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
        const uint32_t timeout; // Time that has to pass without messages
                                //(should be in seconds if can_update_timeout is called periodically and this frequency is set when parser was registered)
    };
} can_module_t;

typedef struct
{
    const uint8_t size;
    const can_module_t *module;
    uint32_t *time_without_messages;
} can_modules_t;

/*
 *   can topics namespace
 */
#define CAN_TOPICS_NAME(NAME) CAN_TOPICS_##NAME

/*
 *   can topics namespace
 */
#define CAN_MODULES_NAME(NAME) CAN_MODULES_##NAME

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
    static const can_topic_t CAN_TOPIC_##NAME[] = {             \
        __VA_ARGS__};                                           \
    static const can_topics_t CAN_TOPICS_NAME(NAME) = {         \
        .size = sizeof(CAN_TOPIC_##NAME) / sizeof(can_topic_t), \
        .topics = CAN_TOPIC_##NAME,                             \
    }
/*
 *   Register a new group of modules
 * all modules have a hash (signature) and a group of topics
 * Example: CAN_REGISTER_MODULES({signature, &CAN_TOPICS_NAME(topics), seconds to timeout})
 */
#define CAN_REGISTER_MODULES(NAME, ...)                                                                \
    static const can_module_t _can_modules_##NAME[] = {                                                \
        __VA_ARGS__};                                                                                  \
    static uint32_t _time_without_messages_##NAME[sizeof(_can_modules_##NAME) / sizeof(can_module_t)]; \
    static can_modules_t CAN_MODULES_NAME(NAME) =                                                      \
        {                                                                                              \
            .size = sizeof(_can_modules_##NAME) / sizeof(can_module_t),                                \
            .module = _can_modules_##NAME,                                                             \
            .time_without_messages = _time_without_messages_##NAME,                                    \
    }
/*
 * Define the parser
 * \param F_CLK is the frequecny can_update_timeout will be called by the user(it is used to calculate timeouts)
 */
#define CAN_REGISTER_PARSER(F_CLK)                                                       \
    void can_parse_topics(const can_topics_t *topics, can_msg_t *msg);                   \
    /*                                                                                   \
     *   can_parser                                                                      \
     *  main function of the parser, parse a message msg                                 \
     * \param modules is a group of modules defined by CAN_REGISTER_MODULES              \
     * \param *msg is a pointer of the msg buffer                                        \
     */                                                                                  \
    void can_parser(can_modules_t *modules, can_msg_t *msg);                             \
    void can_check_timeout(uint32_t *time_without_messages, const can_module_t *module); \
    void can_handle_timeout(uint8_t signature);                                          \
    /*                                                                                   \
     *   can_update_timeout                                                              \
     *   updates the timeout of all modules                                              \
     *   this function should be called at a F_CLK frequency                             \
     *   \param modules is a group of modules defined by CAN_REGISTER_MODULES            \
     */                                                                                  \
    void can_update_timeout(can_modules_t *modules);                                     \
                                                                                         \
    void can_parse_topics(const can_topics_t *topics, can_msg_t *msg)                    \
    {                                                                                    \
        for (uint8_t i = 0; i < topics->size; i++)                                       \
        {                                                                                \
            if (topics->topics[i].id == msg->id)                                         \
            {                                                                            \
                topics->topics[i].parse(msg);                                            \
                break; /* topics dont have more than one id*/                            \
            }                                                                            \
            if (i == (topics->size - 1))                                                 \
            {                                                                            \
                printf("Unrecognized topic! of id %d\n", msg->id);                       \
            }                                                                            \
        }                                                                                \
    }                                                                                    \
                                                                                         \
    void can_parser(can_modules_t *modules, can_msg_t *msg)                              \
    {                                                                                    \
                                                                                         \
        for (uint8_t i = 0; i < modules->size; i++)                                      \
        {                                                                                \
                                                                                         \
            if (modules->module[i].signature == msg->signature)                          \
            {                                                                            \
                modules->time_without_messages[i] = 0;                                   \
                can_parse_topics(modules->module[i].topics, msg);                        \
                break; /*messages only have one module*/                                 \
            }                                                                            \
            if (i == (modules->size - 1))                                                \
            {                                                                            \
                printf("Unrecognized module! of signature %d", msg->signature);          \
            }                                                                            \
        }                                                                                \
    };                                                                                   \
                                                                                         \
    void can_update_timeout(can_modules_t *modules)                                      \
    {                                                                                    \
        for (uint8_t i = 0; i < modules->size; i++)                                      \
        {                                                                                \
            can_check_timeout(&modules->time_without_messages[i], &modules->module[i]);  \
        }                                                                                \
    }                                                                                    \
    void can_check_timeout(uint32_t *time_without_messages, const can_module_t *module)  \
    {                                                                                    \
                                                                                         \
        if (++*time_without_messages >= module->timeout * F_CLK)                         \
        {                                                                                \
            printf("timeout of module with signature %d\n", module->signature);          \
            *time_without_messages = 0;                                                  \
            can_handle_timeout(module->signature);                                       \
        }                                                                                \
    }
