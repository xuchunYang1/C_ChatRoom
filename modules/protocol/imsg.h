//数据包结构
#pragma once
#include "imsg_crc.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define IMSG_HEADER_LEN 7 //消息头长
#define IMSG_STX1 0xfe
#define IMSG_STX2 0xfd  

#define PAYLOADLEN 1024*2
#ifdef __cplusplus
extern "C"{
#endif

typedef struct _imsg
{
	uint16_t checksum; 	//CRC16 校验值
	uint8_t msgid;		//消息ID号
    uint16_t seq; //消息序列号
	uint16_t payload_len;	//payload长度
	uint8_t payload[PAYLOADLEN];  //消息内容
}imsg_t;

//把结构体放进buf里
/***********************************************
* 功能: 将数据包放进缓冲区buffer
* 参数1: buffer缓冲区
* 参数2: 数据包
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间	
*/
static inline uint16_t imsg_to_put_buffer(uint8_t *buf, const imsg_t *msg)
{
	uint16_t crc = 0x0000;
	if (buf == NULL || msg == NULL)
		return 0;
	//存放每个字节内容
	buf[0] = IMSG_STX1;
	buf[1] = IMSG_STX2;
	buf[2] = msg->msgid;
	buf[3] = (uint8_t)(msg->payload_len & 0xFF);
	buf[4] = (uint8_t)(msg->payload_len >> 8);
	
	//crc校验
	crc = crc16(msg->payload, msg->payload_len);
	buf[5] = (uint8_t)(crc & 0xFF);
	buf[6] = (uint8_t)(crc >> 8);
	
	memcpy(&buf[7], msg->payload, msg->payload_len);
	return (IMSG_HEADER_LEN + msg->payload_len);	
}

#ifdef __cplusplus
}
#endif
