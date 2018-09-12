#pragma once
/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <protocol/common.h>
#include <net/common.h>
#include "color.h"
#include "onlinelist.h"
#include "conversation_api.h"

/*defines*/
#define SERVER_PORT     8000
#define MAXTHREADS      5
#define MAXREAD         1024*4
#define BUFFER_SEND_LEN 1024*4
#define BUFFER_RECV_LEN 1024*4
#define DATA_RECV_LEN   1024*4
#define CHAT_RECORD     1024*1024*2
#define NAME            20
#define CLS             system("clear")

extern name_t *clien_online_list;
extern char my_nickname[NAME];

extern char chat_record[CHAT_RECORD];

extern int group_up_flag;
extern bool all_people_flag;
extern bool of_group_flag;
extern bool online_list_flag;

#ifdef __cplusplus
extern "C"{
#endif

enum KEY
{ //One key has three char
    DIRECTION_TAG_1 = 0x1b,
    DIRECTION_TAG_2 = 0x5b,
    KEY_UP = 0x41,
    KEY_DOWN = 0x42,
    KEY_RIGHT = 0x43,
    KEY_LEFT = 0x44,
    ENTER = 0x0a,
    ESC = 0x1b,
};
//Package the incoming parameters of the parse process
typedef struct data_in
{
    char data_recv[DATA_RECV_LEN];
    int count;
    int sockfd;
}data_in_t;

static inline void data_in_pack(data_in_t* in_data, const char* data_recv,
                                        const int count, const int sockfd)
{
    memcpy(in_data->data_recv, data_recv, count);
    in_data->count = count;
    in_data->sockfd = sockfd;
}
//Show two-dimensional array
static inline void show_two_dim_array(const char array[][NAME], uint16_t size)
{
    int i;
    for (i = 0; i < size; ++i)
    {
        if (strcmp(my_nickname, array[i]) == 0)
        {
            printf("\t****\t"L_RED"%-11s"NONE"****\n", array[i]);
        }
        else
        {
            printf("\t****\t"L_YELLOW"%-11s"NONE"****\n", array[i]);
        }
    }
}

//Get current time
static void get_file_time(char *current_time, uint8_t size)
{
    //time variable
    time_t now;
    struct tm *timenow;
    /*get current time*/
    time(&now);
    timenow = localtime(&now);
    strftime(current_time, size, "%Y-%m-%d-%H-%M-%S", timenow);
}
//Create chat record file
static FILE *create_log_file()
{
    char cur_time[40] = {0};

    get_file_time(cur_time, 20);
    strcat(cur_time, "_");
    strcat(cur_time, my_nickname);

    char filename[64] = {0};
    sprintf(filename, "./Logs/%s", cur_time);

    FILE *fp;
    fp = fopen(filename, "w");
    if(fp == NULL)
    {
        printf(L_RED"Create chat file failed"NONE"\n");
        exit(1);
    }
    else
    {
        printf(L_GREEN"Create chat file success"NONE"\n");
        return fp;
    }
}
//Get current time
static void get_current_time(char *current_time, uint8_t size)
{
    //time variable
    time_t now;
    struct tm *timenow;
    /*get current time*/
    time(&now);
    timenow = localtime(&now);
    strftime(current_time, size, "%Y-%m-%d %H:%M:%S", timenow);
}
//Record convervation reply
static inline void record_conversation_reply(char *record,  char *name, char *data)
{
    char current_time[30] = {0};
    get_current_time(current_time, sizeof(current_time));

    strcat(record, "|");
    strcat(record, current_time);
    strcat(record, "|");
    strcat(record, name);
    strcat(record, "|");
    strcat(record, data);
}
//Record my words
static inline void record_my_words(char *record, char *data)
{
    char current_time[30] = {0};
    get_current_time(current_time, sizeof(current_time));

    strcat(record, "|");
    strcat(record, current_time);
    strcat(record, "|");
    strcat(record, my_nickname);
    strcat(record, "|");
    strcat(record, data);
}
//Creat current online list
static void make_cur_online_list(char online_list[][NAME], uint16_t size)
{
    name_t *online_user = NULL;
    int i;
    for (i = 0; i < size; ++i)
    {
       online_user =  make_name_node(online_list[i]);
       head_insert_name_node(online_user, &clien_online_list);
    }
}
//Check if the group member has my own name
static int check_group_member_info(const char member[][NAME], uint8_t size)
{
    int total = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        if (strcmp(my_nickname, member[i]) == 0)
            total++;
    }
    return total;
}
static inline void traverse_online_list(const name_t *head)
{
    const name_t *p = head;
    while (p != NULL)
    {
        if (strcmp(my_nickname, p->name) == 0)
        {
            printf(L_RED"%s"NONE"|", p->name);
        }
        else
        {
            printf(L_BLUE"%s"NONE"|", p->name);
        }
        p = p->next;
    }
    printf("\n");
}
//Request critical information from the server
static void request_info_from_server(const int fd)
{
    imsg_cmd_t cmd;
    char buffer_send[BUFFER_SEND_LEN];

    cmd.cmd = CMD_ALL_PEOPLE;
    memset(buffer_send, 0, sizeof(buffer_send));
    send_cmd_struct(fd, buffer_send, &cmd);

    cmd.cmd = CMD_OF_GROUP;
    memset(buffer_send, 0, sizeof(buffer_send));
    send_cmd_struct(fd, buffer_send, &cmd);
}
//Grouping
static inline void chat_grouping(const int fd,
                                 const int group_up_flag,
                                 const imsg_list_t all_people,
                                 imsg_group_info_t *group,
                                 const char *reply)
{
    char buffer_send[BUFFER_SEND_LEN];
    int ret = 0;

    CLS;
    /*Waiting for recive all people info*/
    if (all_people_flag)
    {
        printf(L_GREEN"All people: "L_CYAN"%d"NONE"\n",
                                all_people.list_size);
        show_two_dim_array(all_people.list,
                                all_people.list_size);
    }

    bool exit = false;
    while(!exit)
    {
        memset(group, 0, sizeof(imsg_group_info_t));
        memcpy(group->founder, my_nickname, strlen(my_nickname) + 1);
        /*Start grouping*/
        input_group_info(group->name,
                         &group->size, group->member);
        /*Can't have my name*/
        ret = check_group_member_info(group->member, group->size);
        if (ret == 1)
        {
            /*Send to server*/
            memset(buffer_send, 0, sizeof(buffer_send));
            send_group_info_struct(fd, buffer_send, group);

            /*Waiting for recive group up reply info*/
            printf(L_GREEN"Waiting reply result..."NONE"\n");
            usleep(1000*10);

            if (group_up_flag == 0)
            {
                printf(L_GREEN"\r%s"NONE"\n", reply);
                printf(L_GREEN"Please start group chat"NONE"\n");
                exit = true;
            }
            else if (group_up_flag == 1)
            {
                printf(L_RED"\r%s"NONE"\n", reply);
                printf("hello\n");
                printf(L_GREEN"Please re-register the group"NONE"\n");
            }
        }
        else
        {
            printf(L_RED"Can't enter your own nickname,"
                        " by default you are already in the group"NONE"\n");
            printf(L_GREEN"Please re-register the group"NONE"\n");
        }
    }
}
//Group chat
static inline void group_chat(const int fd,
                              const imsg_list_t of_group,
                              imsg_conversation_t *conversation
                              )
{
    char buffer_send[BUFFER_SEND_LEN];
    CLS;
    /*Waiting for recive of group info*/
    if (of_group_flag)
    {
        //Show all group name
        printf(L_GREEN"Group list:"NONE"\n");
        show_two_dim_array(of_group.list,
                           of_group.list_size);
    }
    //Choose to enter a group
    char group_name[20];
    memset(group_name, 0, sizeof(group_name));
    printf(L_CYAN"Please select a group:\t"NONE);
    fgets(group_name, sizeof(group_name), stdin);
    group_name[strlen(group_name) - 1] = '\0'; //Replace '\n' with '\0'
    while(1)
    {
        memset(conversation, 0, sizeof(imsg_conversation_t));
        printf(L_GREEN"I"NONE" say: ");
        scanf("%128s", conversation->data);
        getchar();
        /*Send to server*/
        conversation->target_flag = TARGET_IS_GROUP;
        memcpy(conversation->target_name, group_name,
                                strlen(group_name) + 1);
        memset(buffer_send, 0, sizeof(buffer_send));
        send_conversation_struct(fd, buffer_send, conversation);

        record_my_words(chat_record, conversation->data);
        usleep(1000*10);
    }
}
//Private chat
static inline void private_chat(const int fd,
                                const name_t *clien_online_list,
                                imsg_conversation_t *conversation)
{
    uint16_t online_list_size = 0;
    char buffer_send[BUFFER_SEND_LEN];

    CLS;
    /*Waiting for recive online list info*/

    if (online_list_flag)
    {
        //Show all group name
        online_list_size = get_name_list_size(clien_online_list);
        printf(L_GREEN"Online list: "L_YELLOW"%d"NONE"\n",
                                        online_list_size);
        traverse_online_list(clien_online_list);
    }

    printf("[ "L_GREEN"Message Format"NONE" ]: < "L_YELLOW UNDERLINE
           "Name"NONE" "L_YELLOW UNDERLINE"Content"NONE" >\n");
    while (1)
    {
        memset(conversation, 0, sizeof(imsg_conversation_t));
        printf(L_GREEN"I"NONE" say: ");
        scanf("%20s %128s", conversation->target_name, conversation->data);
        getchar();
        /*Send to server*/
        conversation->target_flag = TARGET_IS_PEOPLE;
        memset(buffer_send, 0, sizeof(buffer_send));
        send_conversation_struct(fd, buffer_send, conversation);

        record_my_words(chat_record, conversation->data);

        usleep(1000*10);
    }
}
//关闭回显
static inline void enable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}
//Open echo
static inline void disable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
    tcflush(0, TCIFLUSH);
}

static inline bool kbhit2(void)
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

#ifdef __cplusplus
}
#endif
