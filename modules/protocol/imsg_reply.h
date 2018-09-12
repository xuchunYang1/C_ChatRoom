//The server sends a reply information to the client
#pragma once
/*includes*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "imsg.h"
/*defines*/
/*imsg id*/
#define IMSG_REPLY 6

#define ID 10
#define REPLY_INFO_LEN 64
#define USER_NAME_LEN 20
#ifdef __cplusplus
extern "C"{
#endif
typedef enum
{
    REPLY_NONE,
    SIGN_IN_SUCCESS,
    SIGN_IN_FAILED,
    SIGN_UP_SUCCESS,
    SIGN_UP_FAILED,
    SIGN_OUT_SUCCESS,
    SIGN_OUT_FAILED,
    ONLINE_ALONE,
    OFFLINE_ALONE,
    GROUP_UP_SUCCESS,
    GROUP_UP_FAILED,
}ReplyFlag;

typedef struct _imsg_reply
{
    ReplyFlag flag;
    char id[ID];
    char name[USER_NAME_LEN];
    char info[REPLY_INFO_LEN];
}imsg_reply_info_t;

//打包解包函数
/***********************************************
* 功能: 打包在线链表协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针(数据包)
* 参数2: 指向imsg_list_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间
************************************************/
static inline uint16_t imsg_reply_info_pack(imsg_t *msg, const imsg_reply_info_t *in_msg)
{
    //入口判断
    if (msg == NULL || in_msg == NULL)
        return 0;

    //将协议赋值给msg->payload
    memcpy(msg->payload, in_msg, sizeof(imsg_reply_info_t));
    msg->msgid = IMSG_REPLY;
    msg->payload_len = sizeof(imsg_reply_info_t);

    return msg->payload_len;
}
/***********************************************
* 功能: 解包在线链表协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针
* 参数2: 指向imsg_list_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间
************************************************/
static inline uint16_t imsg_reply_info_unpack(const imsg_t *msg, imsg_reply_info_t *out_msg)
{
    if (msg == NULL || out_msg == NULL)
        return 0;
    //判断协议是否完整
    if (msg->payload_len != sizeof(imsg_reply_info_t))
        return 0;

    //将协议赋值给out
    memcpy(out_msg, msg->payload, msg->payload_len);
    return msg->payload_len;
}

#ifdef __cplusplus
}
#endif
