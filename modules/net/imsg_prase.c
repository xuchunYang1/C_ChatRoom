#include "imsg_prase.h"
/***********************************************
* Function name: imsg_parse_char
* Description  : Parse each incoming byte
* Parameter 1  : A pointer to imsg_t
* Parameter 2  : Prase state
* Parameter 3  : Incoming byte
* Parameter 4  : payload index
* Return value : successfully return 0 or -1
* Finite state machine
* State transition
*************************************************/
int imsg_parse_char(imsg_t* out_msg, imsg_parse_state_t *status, uint8_t c, int *payload_index)
{
	if(!out_msg || !status)
		return -1;
	switch(*status)
	{
		case IMSG_PARSE_STATE_UNINIT:
		case IMSG_PARSE_STATE_IDLE:
			if(c == IMSG_STX1)
			{
				*status = IMSG_PARSE_STATE_GOT_STX1;
				memset(out_msg,0,sizeof(imsg_t));
                *payload_index = 0;
			}
		break;
		case IMSG_PARSE_STATE_GOT_STX1:
			if(c == IMSG_STX2)
			{
				*status = IMSG_PARSE_STATE_GOT_STX2;
			}
		break;
		case IMSG_PARSE_STATE_GOT_STX2:
			*status = IMSG_PARSE_STATE_GOT_MSGID;
			out_msg->msgid = c;
		break;
		case IMSG_PARSE_STATE_GOT_MSGID:
			*status = IMSG_PARSE_STATE_GOT_LEN_L;
			out_msg->payload_len = c;
		break;
		case IMSG_PARSE_STATE_GOT_LEN_L:
			*status = IMSG_PARSE_STATE_GOT_LEN_H;
			out_msg->payload_len += c << 8;
		break;
		case IMSG_PARSE_STATE_GOT_LEN_H:
			*status = IMSG_PARSE_STATE_GOT_CRC_L;
			out_msg->checksum = c;
		break;
		case IMSG_PARSE_STATE_GOT_CRC_L:
			*status = IMSG_PARSE_STATE_GOT_CRC_H;
			out_msg->checksum += c << 8;
		break;
		case IMSG_PARSE_STATE_GOT_CRC_H:
			*status = IMSG_PARSE_STATE_GOT_PAYLOAD;
            out_msg->payload[*payload_index] = c;
            *payload_index += 1;

            if(out_msg->payload_len == *payload_index) //The packet is one byte situation
            {
                if(out_msg->checksum == crc16(out_msg->payload,out_msg->payload_len))
                {
                    *status = IMSG_PARSE_STATE_GOT_COMPLETE;
                }
                else
                {
                    *status = IMSG_PARSE_STATE_GOT_BAD_CRC;
                }
            }
		break;
		case IMSG_PARSE_STATE_GOT_PAYLOAD:
            if(*payload_index >= out_msg->payload_len - 1)
			{
                if(c == IMSG_STX1) //Handle sticky packet
				{
					*status = IMSG_PARSE_STATE_GOT_STX1;
					memset(out_msg,0,sizeof(imsg_t));
                    *payload_index = 0;
				}
				else
				{
                    out_msg->payload[*payload_index] = c; //Last byte
                    *payload_index += 1;
				}
				if(out_msg->checksum == crc16(out_msg->payload,out_msg->payload_len))
				{
					*status = IMSG_PARSE_STATE_GOT_COMPLETE;
				}
				else
				{
					*status = IMSG_PARSE_STATE_GOT_BAD_CRC;
				}
			}
			else
			{
                out_msg->payload[*payload_index] = c;
                *payload_index += 1;
			}
		break;
	}
	if(*status == IMSG_PARSE_STATE_GOT_COMPLETE)
	{
		*status = IMSG_PARSE_STATE_IDLE;
		return 0;
	}
	else
	{
		return -1;
	}
}	
