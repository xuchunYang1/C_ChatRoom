#pragma once
/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <protocol/common.h>
#include "color.h"
#include "database.h"
#include "grouplist.h"
#include "linklist.h"

/*defines*/
#define SERVER_PORT     8000
#define ID_LEN          7
#define SQL_LEN         1024
#define MAXTHREADS      18
#define MAXREAD         1024*4
#define MAX_BUFFER      20
#define BUFFER_SEND_LEN 1024*4
#define BUFFER_RECV_LEN 1024*4
#define DATA_RECV_LEN   1024*4
#define DATA_SEARCH_LEN 1024*4
#define MAX_NAME_NUM    1024
#define NAME            20
#define CHAT_RECORD     1024*1024*2

extern group_node_t *server_group_head;
#ifdef __cplusplus
extern "C"{
#endif

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
/********************************************
* 功能：产生长度为ｎ的 0-9 之间的字符串
* 参数：
*	1: 一个非空的pointer
*	2: length
* 返回值：No
*********************************************/
static inline void generate_random_str(char * buff, int n)
{
    char metachar[] = "0123456789";
    srand(time(NULL));
    for (int i = 0; i < n - 1; ++i)
    {
        buff[i] = metachar[rand() % 10];
    }
    buff[n - 1] = '\0';
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
    memset(current_time, 0, size);
    //time variable
    time_t now;
    struct tm *timenow;
    /*get current time*/
    time(&now);
    timenow = localtime(&now);
    strftime(current_time, size, "%Y-%m-%d %H:%M:%S", timenow);
}
//Record user operation
static inline void record_user_operation(char *record,
                                         char *name,
                                         char *operarion,
                                         char *result)
{
    char current_time[30] = {0};
    get_current_time(current_time, sizeof(current_time));

    strcat(record, "|");
    strcat(record, current_time);
    strcat(record, "|");
    strcat(record, name);
    strcat(record, "|");
    strcat(record, operarion);
    strcat(record, "|");
    strcat(record, result);
}
//Get operation result
static inline void get_operation_result(char *result, uint8_t flag)
{
    if (flag == SIGN_UP_SUCCESS || flag == SIGN_IN_SUCCESS ||
            flag == GROUP_UP_SUCCESS)
    {
        strcpy(result, "Success");
    }
    else if (flag == SIGN_UP_FAILED || flag == SIGN_IN_FAILED ||
             flag == GROUP_UP_FAILED)
    {
        strcpy(result, "Failed");
    }
}

//Get two dim array len
static inline int get_two_dim_len(const char buffer[][NAME])
{
    int i;
    for (i = 0; i < MAX_NAME_NUM && *buffer[i]; ++i);

    return i;
}
//Show two-dimensional array
static inline void show_two_dim_array(const char array[][NAME], uint16_t size)
{
    int i;
    for (i = 0; i < size; ++i)
    {
        printf("\t****\t"L_YELLOW"%-11s"NONE"****\n", array[i]);
    }
}
//count name from the database information
//The content: [Alice|Tom|blank|枫叶子|]
static inline uint16_t count_name(const char *src)
{
    int i;
    uint16_t count = 0;
    for (i = 0; i < strlen(src) && src[i] != '\0'; ++i)
    {
        if (src[i] == '|')
            count += 1;
    }
    return count;
}
/************************************************************
* Funtion Name: find_name_from_list
* Parameters:
*     1: the receiver's nickname to find
*     2: receiver's sockfd
*     3: list head pointer
* Return: success return user 0, or -1
* Description: find the fd corresponding to the receiver
* Date: 2018-08-14
*************************************************************/
static inline int find_name_from_list(const char *receiver_name,
                                        uint16_t *receiver_sockfd,
                                        const user_node_t *head)
{
    if (receiver_name == NULL || head == NULL)
        return -1;
    const user_node_t *p = head;
    while (p != NULL)
    {
        if (strcmp(p->user.nickname, receiver_name) == 0)
        {
            *receiver_sockfd = p->user.sockfd;
            return 0;
        }
        p = p->next;
    }
    return -1;
}
/************************************************************
* Funtion Name: find_sockfd_from_list
* Parameters:
*     1: the sender's sockfd to find
*     2: sender's name
*     3: list head pointer
* Return: success return 0, or -1
* Description: find the sender corresponding to fd
* Date: 2018-08-14
*************************************************************/
static inline int find_sockfd_from_list(const int *sender_sockfd,
                                        char *sender_name,
                                        const user_node_t *head)
{
    if (sender_name == NULL || head == NULL)
        return -1;
    const user_node_t *p = head;
    while (p != NULL)
    {
        if (p->user.sockfd == *sender_sockfd)
        {
            memcpy(sender_name, p->user.nickname,
                   strlen(p->user.nickname) + 1);
            return 0;
        }
        p = p->next;
    }
    return -1;
}
/************************************************************
* Funtion Name: find_id_from_list
* Parameters:
*     1: the user id to find
*     2: list head pointer
* Return: success return user 0, or -1
* Description: avoid double logins
* Date: 2018-08-14
*************************************************************/
static inline int find_id_from_list(const char *user_id, const user_node_t *head)
{
    if (user_id == NULL || head == NULL)
        return -1;

    const user_node_t *p = head;
    while (p != NULL)
    {
        if (strcmp(p->user.id, user_id) == 0)
        {
            return 0;
        }
        p = p->next;
    }
    return -1;
}

/************************************************************
* Funtion Name: delete_user_from_list
* Parameters:
*     1: the user nickname to delete
*     2: list head pointer
* Return: success return user 0, or -1
* Description: delete offline users
* Date: 2018-08-14
*************************************************************/
static inline int delete_user_from_list(const uint16_t sockfd, user_node_t **head)
{
    user_node_t *p = *head;
    user_node_t *q = NULL;
    if (*head == NULL)
        return -1;
    //first node
    if (p->user.sockfd == sockfd)
    {
        *head = p->next;
        return 0;
    }
    else
    {   //from second node
        while (p != NULL)
        {
            q = p;
            p = p->next;
            //last node
            if (p->next == NULL && p->user.sockfd == sockfd)
            {
                q->next = NULL;
                return 0;
            }
            //middle node
            if (p->next != NULL && p->user.sockfd == sockfd)
            {
                q->next = p->next;
                return 0;
            }
        }
    }
}
//Separate the str from the database information
//The content: [id|000003|nickname|Alice|password|123|reg_date|2018-08-11 17:33:14|]
static inline void search_str_from_data_search(char *buffer,
                                               const char *data_search,
                                               const char *str)
{
    int i = 0;
    bool exit_flag = false;
    char *temp = strstr(data_search, str);
    //[ nickname|Alice|password|123|reg_date|2018-08-11 17:33:14| ]
    while(!exit_flag)
    {
        temp++;
        if (*temp == '|')
        {
            while(1)
            {
                temp++;
                if (*temp != '|')
                {
                    buffer[i] = *temp;
                    i += 1;
                }
                else
                {
                    buffer[i] = '\0';
                    exit_flag = true;
                    break;
                }
            }
        }
    }
}
//Separate the str from the database information
//The content: [nickname|Alice|nickname|Tom|nickname|blank|nickname|枫叶子|]
//The content: [group_name|Friends|group_name|LaLa|group_name|Spring|group_name|Summer|]
static inline void split_name_from_data_search(char buffer[][20], uint16_t size,
                                                    const char *data_search)
{
    int i;
    int j;
    int k;
    int temp = 0;
    for (i = 0; i < size; ++i)
    {
        k = 0;
        for (j = temp; data_search[j] != '|'; ++j)
        {
            buffer[i][k++] = data_search[j];
        }
        temp = ++j; //skip '|'
    }
}

//Online list assign
static inline void online_list_assign(const user_node_t *head,
                                      char online_list[][NAME],
                                      uint16_t list_size)
{
    const user_node_t *p = head;
    int i;
    for (i = 0; i < list_size; ++i)
    {
        if (p != NULL)
        {
            memcpy(online_list[i], p->user.nickname, strlen(p->user.nickname) + 1);
        }
        p = p->next;
    }
}
//Insert register information into database
static inline int insert_register_info(const char *id,
                                       const imsg_sign_in_sign_up_t sign,
                                       const char *cur_time)
{
    int ret = 0;
    char sql[SQL_LEN] = {0};

    snprintf(sql, SQL_LEN, "INSERT INTO RegisterInfo VALUES('%s', '%s', '%s', '%s');",
                            id, sign.nickname, sign.password, cur_time);
    ret = exec_sqlite3(sql, NULL, NULL);
    return ret;
}
//Insert group information into database
static inline int insert_group_info(const imsg_group_info_t group, const char *cur_time)
{
    int ret = 0;
    char sql[SQL_LEN] = {0};

    snprintf(sql, SQL_LEN, "INSERT INTO GroupInfo VALUES('%s', '%d', '%s', '%s');",
                group.name, group.size, group.founder, cur_time);
    ret = exec_sqlite3(sql, NULL, NULL);

    int i;
    for (i = 0; i < group.size; ++i)
    {
        memset(sql, 0, SQL_LEN);
        snprintf(sql, SQL_LEN, "INSERT INTO UserGroup VALUES('%s', '%s','%s', 'NULL');",
                    group.member[i], group.name, cur_time);
        exec_sqlite3(sql, NULL, NULL);
    }
    return ret;
}
//Create group list
static inline void make_group_list(char name[][NAME], uint16_t size)
{
    group_node_t *group_node = NULL;
    char sql[SQL_LEN];
    char data_search[DATA_SEARCH_LEN];
    char data_search_two[MAX_NAME_NUM][NAME];
    uint16_t name_num = 0;
    int i;

    for (i = 0; i < size; ++i)
    {
       memset(sql, 0, SQL_LEN);
       snprintf(sql, SQL_LEN,
                "SELECT nickname FROM UserGroup WHERE group_name = '%s';",
                name[i]);
       memset(data_search, 0, DATA_SEARCH_LEN);
       exec_sqlite3(sql, print_name_result, data_search);

       name_num = count_name(data_search);

       memset(data_search_two, 0, MAX_NAME_NUM*NAME);
       split_name_from_data_search(data_search_two, name_num, data_search);

       group_node =  make_group_node(name[i], data_search_two, name_num);
       insert_group_node(group_node, &server_group_head);
    }
}
//Traverse group list
static inline void traverse_group_list(const group_node_t *head)
{
    const group_node_t *p = head;
    printf("------------------------------------------\n");
    while (p != NULL)
    {
        printf(L_BLUE"Group name:\t"NONE L_CYAN"%s"NONE"\n", p->group.name);
        printf(L_BLUE"Group size:\t"NONE L_CYAN"%d"NONE"\n", p->group.size);
        printf(L_BLUE"Group member:\t"NONE"\n");
        show_two_dim_array(p->group.member, p->group.size);
        printf("------------------------------------------\n");

        p = p->next;
    }
    printf("\n");
}
/************************************************************
* Funtion Name: find_group_member
* Parameters:
*     1: the group name to find
*     2: list head pointer
* Return: success return user 0, or -1
* Description: find the members corresponding to group name
* Date: 2018-08-14
*************************************************************/
static inline int find_group_member(char member[][20],
                                      const char *group_name,
                                      const group_node_t *head)
{
    uint16_t member_size = 0;
    if (group_name == NULL || head == NULL)
        return -1;
    const group_node_t *p = head;
    while (p != NULL)
    {
        if (strcmp(p->group.name, group_name) == 0)
        {
            member_size = p->group.size;
            memcpy(member, p->group.member, member_size*NAME);
            return 0;
        }
        p = p->next;
    }
    return -1;
}
/************************************************************
* Funtion Name: select_of_group
* Parameters:
*     1: the user name to find
*     2: list head pointer
*     3: two dim array to store group name
* Return: success return user 0, or -1
* Description: find the members corresponding to group name
* Date: 2018-08-14
*************************************************************/
static inline int select_of_group(char of_group[][NAME],
                                  const char *name,
                                  const group_node_t *head)
{
    int i;
    int j = 0;
    if (name == NULL || head == NULL)
        return -1;
    const group_node_t *p = head;
    while (p != NULL)
    {
        for (i = 0; i < p->group.size; ++i)
        {
            if (strcmp(p->group.member[i], name) == 0)
            {
                memcpy(of_group[j], p->group.name, strlen(p->group.name) + 1);
                j += 1;
                break;
            }
        }
        p = p->next;
    }
    return -1;
}
//Get every user's of groups
static inline void get_user_of_group(imsg_list_t* of_group, char *sender_name,
                                     const int *fd, const user_node_t *head)
{
    find_sockfd_from_list(fd, sender_name, head);

    memset(of_group, 0, sizeof(imsg_list_t));
    of_group->list_flag = LIST_OF_GROUP;
    select_of_group(of_group->list, sender_name, server_group_head);
    of_group->list_size = get_two_dim_len(of_group->list);
}
//Get the name of everyone from the database
static void load_all_people(imsg_list_t *all_people)
{
    uint16_t people_num = 0;
    char sql[SQL_LEN] = {0};
    char data_search[DATA_SEARCH_LEN] = {0};
    char data_search_two[MAX_NAME_NUM][NAME] = {0};

    strcpy(sql, "SELECT nickname FROM RegisterInfo");

    exec_sqlite3(sql, print_name_result, data_search);

    people_num = count_name(data_search);

    split_name_from_data_search(data_search_two,
                                 people_num, data_search);

    memset(all_people, 0, sizeof(imsg_list_t));
    all_people->list_flag = LIST_ALL_PEOPLE;
    memcpy(all_people->list, data_search_two, people_num*NAME);
    all_people->list_size = people_num;

}
//Get all groups from the database
static void load_all_groups()
{
    uint16_t group_num = 0;
    char sql[SQL_LEN] = {0};
    char data_search[DATA_SEARCH_LEN] = {0};
    char data_search_two[MAX_NAME_NUM][NAME] = {0};

    strcpy(sql, "SELECT group_name FROM GroupInfo");

    exec_sqlite3(sql, print_name_result, data_search);

    group_num = count_name(data_search);

    split_name_from_data_search(data_search_two,
                                 group_num, data_search);

    make_group_list(data_search_two, group_num);
}
//Select the information from database
static inline int select_register_name(char *data_search, const char *nickname)
{
    int ret = 0;
    char sql[SQL_LEN] = {0};

    snprintf(sql, SQL_LEN,
                        "SELECT * FROM RegisterInfo WHERE nickname = '%s';",
                        nickname);
    ret = exec_sqlite3(sql, print_result, data_search);
}
//Select the information from database
static inline int select_register_id(char *data_search, const char *id)
{
    int ret = 0;
    char sql[SQL_LEN] = {0};

    snprintf(sql, SQL_LEN, "SELECT * FROM RegisterInfo WHERE id = '%s';",
                                id);
    ret = exec_sqlite3(sql, print_result, data_search);
}
//Select the information from database
static inline int select_group_name(char *data_search, const char *group_name)
{
    int ret = 0;
    char sql[SQL_LEN] = {0};

    snprintf(sql, SQL_LEN,
                        "SELECT * FROM GroupInfo WHERE group_name = '%s';",
                        group_name);
    ret = exec_sqlite3(sql, print_result, data_search);
}
//Prepare online list to send
static inline void prepare_online_list(imsg_list_t *online_list,
                                       const user_node_t *head)
{
    memset(online_list, 0, sizeof(imsg_list_t));
    online_list->list_flag = LIST_CUR_ONLINE;
    online_list->list_size = get_user_list_size(head);
    online_list_assign(head, online_list->list,
                             online_list->list_size);
}

#ifdef __cplusplus
}
#endif
