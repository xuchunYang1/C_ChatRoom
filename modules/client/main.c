//This is a client
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <protocol/common.h>
#include <net/common.h>
#include <thread_pool/thread_pool.h>
#include <client_common/common.h>
#include "sign_in_sign_up_api.h"

char chat_record[CHAT_RECORD] = {0};

static char *IP = "127.0.0.1";
int sockfd;
static int n_read;
static imsg_t msg_out;  //imsg_t
static imsg_reply_info_t reply_recv;

name_t *clien_online_list = NULL;
char my_nickname[NAME]; //Get my own nickname after successful login
char my_id[ID];

static char buffer_send[BUFFER_SEND_LEN]; //A data packet buffer
static char buffer_recv[BUFFER_RECV_LEN];

static imsg_conversation_t conversation_send;
static imsg_conversation_t conversation_recv;
static imsg_group_info_t group_send;

static uint16_t online_list_size = 0;
static imsg_list_t list_recv;
static imsg_list_t all_people_list;
static imsg_list_t of_group_list;
static imsg_list_t cur_online_list;

int group_up_flag = -1;
bool all_people_flag = false;
bool of_group_flag = false;
bool online_list_flag = false;

static int parse_conversation_process(const uint8_t *data_recv, const int count)
{
    int index_temp = 0; //for parse char
    imsg_parse_state_t status = 0; //for parse
    int i,ret = -1;
    if(!data_recv || count < 0)
        return -1;

    memset(&msg_out, 0, sizeof(msg_out));
    for(i = 0; i < count; ++i)
    {
        /*First prase data packet*/
//        printf("%c", data_recv[i]);
        ret = imsg_parse_char(&msg_out, &status, data_recv[i], &index_temp);
        if(ret == 0)
        {
//            printf("\n解析一个包成功 msg_id %d \n", msg_out.msgid);
            switch(msg_out.msgid)
            {
                case IMSG_CONVERSATION: //conversation
                    memset(&conversation_recv, 0, sizeof(conversation_recv));
                    imsg_conversation_unpack(&msg_out, &conversation_recv);

                    switch(conversation_recv.target_flag)
                    {
                        case PEOPLE_SUCCESS:
                            printf(L_GREEN"\r[%s] "NONE"say: "L_CYAN"%s"NONE"\n",
                                                    conversation_recv.target_name,
                                                    conversation_recv.data);

                            record_conversation_reply(chat_record, conversation_recv.target_name,
                                                      conversation_recv.data);
                            break;
                        case PEOPLE_FAILED:
                            printf(L_RED"\r%s"NONE"\n", conversation_recv.data);
                            break;
                        case PEOPLE_NOBODY:
                            printf(L_RED"\r%s"NONE"\n", conversation_recv.data);
                            break;
                        case GROUP_SUCCESS:
                            printf(L_GREEN"\r[%s] "NONE"say: "L_CYAN"%s"NONE"\n",
                                                    conversation_recv.target_name,
                                                    conversation_recv.data);

                            record_conversation_reply(chat_record, conversation_recv.target_name,
                                                      conversation_recv.data);
                            break;

                    }
                    break;
                case IMSG_CMD: //cmd
                    break;
                case IMSG_LIST: //online/group/all people list
                    memset(&list_recv, 0, sizeof(list_recv));
                    imsg_list_unpack(&msg_out, &list_recv);
                    switch(list_recv.list_flag)
                    {
                        case LIST_ALL_PEOPLE:
                            memcpy(&all_people_list, &list_recv, sizeof(list_recv));
                            all_people_flag = true;
                            break;
                        case LIST_OF_GROUP:
                            memcpy(&of_group_list, &list_recv, sizeof(list_recv));
                            of_group_flag = true;
                            break;
                        case LIST_CUR_ONLINE:
                            memcpy(&cur_online_list, &list_recv, sizeof(list_recv));
                            make_cur_online_list(cur_online_list.list, cur_online_list.list_size);
                            online_list_flag = true;
                            break;
                    }
                    break;
                case IMSG_REPLY:
                    memset(&reply_recv, 0, sizeof(reply_recv));
                    imsg_reply_info_unpack(&msg_out, &reply_recv);
                    printf("reply_recv.flag: %d\n", reply_recv.flag);
//                    printf("group_up_flag1: %d\n", group_up_flag);
                    name_t *online_user = NULL;
                    switch (reply_recv.flag)
                    {
                        case ONLINE_ALONE:
                            online_user = make_name_node(reply_recv.name);
                            head_insert_name_node(online_user, &clien_online_list);

                            online_list_size = get_name_list_size(clien_online_list);
                            printf(L_GREEN"\nOnline list: "L_YELLOW"%d"NONE"\n", online_list_size);
                            traverse_online_list(clien_online_list);
                            break;
                        case GROUP_UP_SUCCESS:
                            memcpy(of_group_list.list[of_group_list.list_size],
                                    reply_recv.name, strlen(reply_recv.name) + 1);
                            of_group_list.list_size += 1;
                            group_up_flag = 0;
//                            printf(L_GREEN"\r%s"NONE"\n", reply_recv.info);
                            break;
                        case GROUP_UP_FAILED:
                            group_up_flag = 1;
//                            printf("group_up_flag: %d\n", group_up_flag);
                            printf(L_RED"\r%s"NONE"\n", reply_recv.info);
                            break;
                    }
                    break;
            }
        }
    }
}

static void handle_connection(int sockfd, char* buffer_recv)
{
    fd_set readfds;
    int ret = 0;
    int maxfd;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfd = sockfd;

        /*Listening I/O events*/
        ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0)
            perr_exit("select error");
        if (ret == 0)
        {
            printf("client timeout.\n");
            continue;
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            /*Read from server*/
            memset(buffer_recv, 0, MAXREAD);
            n_read = Read(sockfd, buffer_recv, MAXREAD);
            if (n_read <= 0)
            {
                fprintf(stderr, "client: server is closed.\n");
                close(sockfd);
                FD_CLR(sockfd, &readfds);
                return;
            }
            else
            {   /*handle_recv_msg*/
                /*Add the parsing task to the thread pool*/
                parse_conversation_process(buffer_recv, n_read);
            }
        }
    }
}

static void *callback_handle_connection(void *p)
{
    data_in_t *para = (data_in_t *)p;

    bool work1_exit_flag = false;
    while(!work1_exit_flag)
    {
        handle_connection(para->sockfd, para->data_recv);
        work1_exit_flag = true;
    }
}

int main(int argc, char* argv[])
{
    pool_init(MAXTHREADS);
    sockfd = Socket(PF_INET, SOCK_STREAM, 0);
    /*初始化服务器地址*/
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port   = htons(SERVER_PORT);

    Connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    /*Listening readable I/O*/

    data_in_t in_data;
    memset(&in_data, 0, sizeof(in_data));
    data_in_pack(&in_data, buffer_recv, 0, sockfd);
    pool_add_worker(callback_handle_connection, &in_data);

    /*Sign in or Sign up*/
    sign_in_sign_up();

    /*create chat record file*/
    FILE *fp = create_log_file();

    /*Request critical information from the server*/
    request_info_from_server(sockfd);

    /*Start chatting with other people*/
    /*Enter the main interface*/
    bool exit_flag = false;
    uint8_t ui_chat_flag = UI_CHAT_NONE;
    uint8_t option = 0;
    int stdin_count = 0;
    uint8_t in_buffer[1024] = {0};
    int in_buffer_index = 0;
    int i;

    //clear and print interface
//    CLS;
//    SHOW_CHAT_MAIN_INTERFACE;
    enable_raw_mode(); //close echo
    stdin_count = kbhit2();
    if (stdin_count > 0)
    {
//        printf("stdin: %d\n", stdin_count);
        int i;
        for (i = 0; i < stdin_count; ++i)
        {
            getchar();
        }
    }
    while (!exit_flag)
    {
        print_main_interface_with_color(ui_chat_flag);
        while ((stdin_count = kbhit2()) == 0)
        {
            usleep(1000*5); //sleep 5ms
        }
        option = getchar();
        switch (option)
        {
            case DIRECTION_TAG_1:
//                if (stdin_count == 1)
//                {   /*The ESC key is the same as tag1*/
//                    //Send Sign out data packet
//                    printf("ESC\n");
//                    exit_flag = true;
//                }
                option = getchar();                     //get second char
                if (DIRECTION_TAG_2 == option)
                {
                    option = getchar();                 //get third char
                    switch (option)
                    {
                        case KEY_UP:
                            ui_chat_flag = UI_CHAT_GROUPING;
                            break;
                        case KEY_LEFT:
                            ui_chat_flag = UI_CHAT_PRIVATE_CHAT;
                            break;
                        case KEY_RIGHT:
                            ui_chat_flag = UI_CHAT_GROUP_CHAT;
                            break;
                    }
                }
                break;
            case ENTER:
                if (ui_chat_flag != UI_CHAT_NONE)
                {
                    disable_raw_mode();                         //open echo
                    if (ui_chat_flag == UI_CHAT_GROUPING)
                    {   /*Group up*/
                        chat_grouping(sockfd, group_up_flag, all_people_list,
                                                &group_send, reply_recv.info);
                    }
                    else if (ui_chat_flag == UI_CHAT_GROUP_CHAT)
                    {   /*Group chat*/
                        group_chat(sockfd, of_group_list, &conversation_send);
                    }
                    else if (ui_chat_flag == UI_CHAT_PRIVATE_CHAT)
                    {   /*Private chat*/
                        private_chat(sockfd, clien_online_list, &conversation_recv);
                    }
                    enable_raw_mode();                          //close echo
                    ui_chat_flag = UI_CHAT_NONE;
                }
                break;
            default:
                in_buffer[in_buffer_index++] = option;
                stdin_count--;
                for(i = 0; i < stdin_count; ++i)
                {
                    option = getchar();
                    in_buffer[in_buffer_index++] = option;
                }
                break;
        }
    }

    disable_raw_mode();                     //open echo
    printf("exit while\n");

    destroy_name_list(&clien_online_list);
    Close(sockfd);                          //关闭socket
    pool_destroy();                         //destroy the pool

    int total = fwrite(chat_record, sizeof(char), strlen(chat_record), fp);
    printf("write = %d\n", total);
    fclose(fp);

    return 0;
}
