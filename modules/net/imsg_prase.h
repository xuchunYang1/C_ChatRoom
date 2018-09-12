//解析数据包
#pragma once
#include <protocol/common.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif
typedef enum
{
    IMSG_PARSE_STATE_UNINIT = 0,
    IMSG_PARSE_STATE_IDLE,
    IMSG_PARSE_STATE_GOT_STX1,
    IMSG_PARSE_STATE_GOT_STX2,
    IMSG_PARSE_STATE_GOT_LEN_H,
    IMSG_PARSE_STATE_GOT_LEN_L,
    IMSG_PARSE_STATE_GOT_SEQ, //Temp no use
    IMSG_PARSE_STATE_GOT_MSGID,
    IMSG_PARSE_STATE_GOT_PAYLOAD,
    IMSG_PARSE_STATE_GOT_CRC_H,
    IMSG_PARSE_STATE_GOT_CRC_L,
    IMSG_PARSE_STATE_GOT_BAD_CRC,
    IMSG_PARSE_STATE_GOT_COMPLETE,
}imsg_parse_state_t;

int imsg_parse_char(imsg_t* out_msg, imsg_parse_state_t *status,uint8_t c, int *payload_index);
#ifdef __cplusplus
}
#endif
