//Client sends group registration information
#pragma once
/*includes*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "imsg.h"
/*defines*/
/*imsg id*/
#define IMSG_GROUP_INFO 5

#define GROUP_NAME_LEN 20
#define FOUNDERLEN 20
#define MAX_GROUP_MEMBER 20
#define MEMBER_NAME_LEN 20
#ifdef __cplusplus
extern "C"{
#endif

typedef struct _imsg_group_info
{
    char name[GROUP_NAME_LEN];
    char founder[FOUNDERLEN];
    char member[MAX_GROUP_MEMBER][MEMBER_NAME_LEN];
    uint8_t size;
}imsg_group_info_t;

//打包解包函数
/***********************************************
* 功能: 打包群组信息协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针(数据包)
* 参数2: 群名称
* 参数3: 存放群组链表的一个一维数组
* 参数4: 当前群组长度
* 参数5: 群组注册时间
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间
************************************************/
static inline uint16_t imsg_group_info_pack(imsg_t *msg, const imsg_group_info_t *in_msg)
{
    //入口判断
    if (msg == NULL || in_msg == NULL)
        return 0;

    //将协议赋值给msg->payload
    memcpy(msg->payload, in_msg, sizeof(imsg_group_info_t));
    msg->msgid = IMSG_GROUP_INFO;
    msg->payload_len = sizeof(imsg_group_info_t);

    return msg->payload_len;
}
/***********************************************
* 功能: 解包群组信息协议结构体
* 参数1: 指向imsg_t类型的一个结构体指针
* 参数2: 指向imsg_group_info_t的一个指针
* 返回值: 协议的长度
* inline 解决简单函数的频繁调用，不消耗栈空间
************************************************/
static inline uint16_t imsg_group_info_unpack(const imsg_t *msg, imsg_group_info_t *out_msg)
{
    if (msg == NULL || out_msg == NULL)
        return 0;
    //判断协议是否完整
    if (msg->payload_len != sizeof(imsg_group_info_t))
        return 0;

    //将协议赋值给out
    memcpy(out_msg, msg->payload, msg->payload_len);
    return msg->payload_len;
}

#ifdef __cplusplus
}
#endif
