//登录注册协议
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "imsg.h"
//消息ID号
#define IMSG_SIGN_IN_SIGN_UP 1

#define NICKNAMELEN	20
#define PASSWORDLEN 20
 
#ifdef __cplusplus
extern "C"{
#endif
//注册、登录、登出标志
typedef enum 
{	
    SIGN_NONE = 0,
    SIGN_IN,
    SIGN_UP,
}SignFlag;

//定义协议结构体
typedef struct _imsg_sign_in_sign_up
{
	SignFlag sign_flag; 
    char nickname[NICKNAMELEN]; //nickname or id
	char password[PASSWORDLEN];	
}imsg_sign_in_sign_up_t;

//打包解包函数
/***********************************************
* 功能: 打包登录注册协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针(数据包)
* 参数2: 指向imsg_sign_in_sign_up_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间
************************************************/
static inline uint16_t imsg_sign_in_sign_up_pack(imsg_t *msg, const imsg_sign_in_sign_up_t *in_msg)
{
    if (msg == NULL || in_msg == NULL)
        return 0;
    //将协议赋值给msg->payload
    memcpy(msg->payload, in_msg, sizeof(imsg_sign_in_sign_up_t));
    msg->msgid = IMSG_SIGN_IN_SIGN_UP;
    msg->payload_len = sizeof(imsg_sign_in_sign_up_t);

    return msg->payload_len;
}
/***********************************************
* 功能: 解包登录注册协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针
* 参数2: 指向imsg_sign_in_sign_up_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间	
************************************************/
static inline uint16_t imsg_sign_in_sign_up_unpack(const imsg_t *msg, imsg_sign_in_sign_up_t *out_msg)
{
    if (msg == NULL || out_msg == NULL)
		return 0;
	//判断协议是否完整
	if (msg->payload_len != sizeof(imsg_sign_in_sign_up_t))
		return 0;
	
	//将协议赋值给out
    memcpy(out_msg, msg->payload, msg->payload_len);
	return msg->payload_len;
}

#ifdef __cplusplus
}
#endif
 
