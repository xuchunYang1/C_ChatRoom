//发送数据包
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "wrap.h"
#include <protocol/common.h>
#include <server_common/linklist.h>

#ifdef __cplusplus
extern "C"{
#endif

int send_sign_in_sign_up_struct(const int fd, uint8_t *buffer,
                                const imsg_sign_in_sign_up_t *in_msg);
								 
int send_conversation_struct(const int fd, uint8_t *buffer,
                             const imsg_conversation_t *in_msg);
							  
int send_cmd_struct(const int fd, uint8_t *buffer,
                    const imsg_cmd_t *in_msg);

int send_list_struct(const int fd, uint8_t *buffer,
                     const imsg_list_t *in_msg);

int send_group_info_struct(const int fd, uint8_t *buffer,
                           const imsg_group_info_t *in_msg);

int send_reply_info_struct(const int fd, uint8_t *buffer,
                           const imsg_reply_info_t *in_msg);

void send_online_user(const int fd, const user_node_t *head,
                      uint8_t *buffer, const imsg_reply_info_t *in_msg);

#ifdef __cplusplus
}
#endif
