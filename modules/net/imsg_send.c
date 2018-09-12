#include "imsg_send.h"

//发送登录注册数据包
//buffer　一个空的缓冲区
int send_sign_in_sign_up_struct(const int fd, uint8_t *buffer,
                                const imsg_sign_in_sign_up_t *in_msg)
{
	int ret;
	imsg_t msg_send;
    if (buffer == NULL || in_msg == NULL)
		return -1;
	
    imsg_sign_in_sign_up_pack(&msg_send, in_msg);
    ret = imsg_to_put_buffer(buffer, &msg_send);
	return Write(fd, buffer, ret);
}

//发送会话数据包				 
int send_conversation_struct(const int fd, uint8_t *buffer,
                             const imsg_conversation_t *in_msg)
{
	int ret;
	imsg_t msg_send;
    if (buffer == NULL || in_msg == NULL)
		return -1;
	
    imsg_conversation_pack(&msg_send, in_msg);
    ret = imsg_to_put_buffer(buffer, &msg_send);
	return Write(fd, buffer, ret);
}
//发送命令数据包
int send_cmd_struct(const int fd, uint8_t *buffer,
                    const imsg_cmd_t *in_msg)
{
	int ret;
	imsg_t msg_send;
    if (buffer == NULL || in_msg == NULL)
		return -1;
	
    imsg_cmd_pack(&msg_send, in_msg);
    ret = imsg_to_put_buffer(buffer, &msg_send);
	return Write(fd, buffer, ret);
}
//Send list data packet
int send_list_struct(const int fd, uint8_t *buffer,
                     const imsg_list_t *in_msg)
{
    int ret;
    imsg_t msg_send;
    if (buffer == NULL || in_msg == NULL)
        return -1;

    imsg_list_pack(&msg_send, in_msg);
    ret = imsg_to_put_buffer(buffer, &msg_send);
    return Write(fd, buffer, ret);
}
//Send group information data packet
int send_group_info_struct(const int fd, uint8_t *buffer,
                           const imsg_group_info_t *in_msg)
{
    int ret;
    imsg_t msg_send;
    if (buffer == NULL || in_msg == NULL)
        return -1;

    imsg_group_info_pack(&msg_send, in_msg);
    ret = imsg_to_put_buffer(buffer, &msg_send);
    return Write(fd, buffer, ret);
}
//Send server reply information data packet
int send_reply_info_struct(const int fd, uint8_t *buffer,
                           const imsg_reply_info_t *in_msg)
{
    int ret;
    imsg_t msg_send;
    if (buffer == NULL || in_msg == NULL)
        return -1;

    imsg_reply_info_pack(&msg_send, in_msg);
    ret = imsg_to_put_buffer(buffer, &msg_send);
    return Write(fd, buffer, ret);
}
void send_online_user(const int fd, const user_node_t *head,
                      uint8_t *buffer, const imsg_reply_info_t *in_msg)
{
    const user_node_t *p = head;
    while (p != NULL)
    {
        if (p->user.sockfd != fd)
            send_reply_info_struct(p->user.sockfd, buffer, in_msg);
        p = p->next;
    }
}
