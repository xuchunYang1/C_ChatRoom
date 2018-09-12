//The server sends an online list to the client
#pragma once
/*includes*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "imsg.h"
/*defines*/
/*imsg id*/
#define IMSG_LIST 4

#define MAX_NUM 50
#define NAMELEN 20
#ifdef __cplusplus
extern "C"{
#endif
typedef enum
{
    LIST_NONE = 0,
    LIST_ALL_PEOPLE,
    LIST_OF_GROUP,
    LIST_CUR_ONLINE,
}ListFlag;

typedef struct _imsg_list
{
    ListFlag list_flag;
    char list[MAX_NUM][NAMELEN];
    uint16_t list_size;
}imsg_list_t;

//打包解包函数
/***********************************************
* 功能: 打包在线链表协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针(数据包)
* 参数2: 指向imsg_list_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间
************************************************/
static inline uint16_t imsg_list_pack(imsg_t *msg, const imsg_list_t *in_msg)
{
    //入口判断
    if (msg == NULL || in_msg == NULL)
        return 0;

    //将协议赋值给msg->payload
    memcpy(msg->payload, in_msg, sizeof(imsg_list_t));
    msg->msgid = IMSG_LIST;
    msg->payload_len = sizeof(imsg_list_t);

    return msg->payload_len;
}
/***********************************************
* 功能: 解包在线链表协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针
* 参数2: 指向imsg_list_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间
************************************************/
static inline uint16_t imsg_list_unpack(const imsg_t *msg, imsg_list_t *out_msg)
{
    if (msg == NULL || out_msg == NULL)
        return 0;
    //判断协议是否完整
    if (msg->payload_len != sizeof(imsg_list_t))
        return 0;

    //将协议赋值给out
    memcpy(out_msg, msg->payload, msg->payload_len);
    return msg->payload_len;
}

#ifdef __cplusplus
}
#endif
