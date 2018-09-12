//发送命令协议
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "imsg.h"
//消息ID号
#define IMSG_CMD 3

#ifdef __cplusplus
extern "C"{
#endif
typedef enum
{
    CMD_NONE,
    CMD_ALL_PEOPLE,
    CMD_OF_GROUP,
}chat_cmd;

typedef struct _imsg_cmd
{
    chat_cmd cmd;
}imsg_cmd_t;

//打包解包函数
/***********************************************
* 功能: 打包命令协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针(数据包)
* 参数2: 命令
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间	
*/
static inline uint16_t imsg_cmd_pack(imsg_t *msg, const imsg_cmd_t *in_msg)
{
	//入口判断
    if (msg == NULL || in_msg == NULL)
		return 0;

	//将协议赋值给msg->payload
    memcpy(msg->payload, in_msg, sizeof(imsg_cmd_t));
	msg->msgid = IMSG_CMD;
	msg->payload_len = sizeof(imsg_cmd_t);
	
	return msg->payload_len;
}
/***********************************************
* 功能: 解包命令协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针
* 参数2: 指向imsg_cmd_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间	
*/
static inline uint16_t imsg_cmd_unpack(const imsg_t *msg, imsg_cmd_t *out_msg)
{
    if (msg == NULL || out_msg == NULL)
		return 0;
	//判断协议是否完整
	if (msg->payload_len != sizeof(imsg_cmd_t))
		return 0;
	
	//将协议赋值给out
    memcpy(out_msg, msg->payload, msg->payload_len);
	return msg->payload_len;
}
#ifdef __cplusplus
}
#endif
