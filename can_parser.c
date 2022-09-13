#include <stdio.h>
#include "can_parser.h"

void can_parse_topics(const can_topics_t *topics, can_msg_t *msg)
{
    for (uint8_t i = 0; i < topics->size; i++)
    {
        if (topics->topics[i].id == msg->id)
        {
            topics->topics[i].parse(msg);
            break; /* topics dont have more than one id*/
        }
        if (i == (topics->size - 1))
        {
            printf("Unrecognized topic! of id %d\n", msg->id);
        }
    }
}

void can_parser(can_parser_t *parser, can_msg_t *msg)
{

    for (uint8_t i = 0; i < parser->size; i++)
    {

        if (parser->module[i].signature == msg->signature)
        {
            parser->time_without_messages[i] = 0;
            can_parse_topics(parser->module[i].topics, msg);
            break; /*messages only have one module*/
        }
        if (i == (parser->size - 1))
        {
            printf("Unrecognized module! of signature %d", msg->signature);
        }
    }
}

void can_update_timeout(can_parser_t *parser)
{
    for (uint8_t i = 0; i < parser->size; i++)
    {
        can_check_timeout(parser, &parser->time_without_messages[i], &parser->module[i]);
    }
}

void can_check_timeout(can_parser_t *parser, uint32_t *time_without_messages, const can_module_t *module)
{

    if (++*time_without_messages >= module->timeout * parser->frequency)
    {
        printf("timeout of module with signature %d\n", module->signature);
        *time_without_messages = 0;
        can_handle_timeout(module->signature);
    }
}