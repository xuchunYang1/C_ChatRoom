//Single list & No header node
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "color.h"
#ifdef __cplusplus
extern "C"{
#endif
typedef struct
{
    char id[10];
    char nickname[20];
    uint16_t sockfd;
}user_t;

typedef struct user_node
{
    user_t user;
    struct user_node *next;
}user_node_t;
/************************************************************
* Function Name: make_node
* Parameters:
*    1: user id
*    2: user nickname
*    3: user sockfd
* Return: a pointer at the node
* Date: 2018-08-14
*************************************************************/
static inline user_node_t *make_user_node(const char *id,
                                    const char *nickname,
                                    uint16_t sockfd)
{
    if (id == NULL || nickname == NULL)
        return NULL;
    user_node_t *p = (user_node_t*)malloc(sizeof(user_node_t));
    if (p == NULL)
    {
        printf("malloc failed\n");
        exit(1);
    }
    strcpy(p->user.id, id);
    memcpy(p->user.nickname, nickname, strlen(nickname) + 1);
    p->user.sockfd = sockfd;
    p->next = NULL;

    return p;
}
/************************************************************
* Funtion Name: head_insert_node
* Parameters:
*     1: a pointer at the node
*     2: list head pointer
* Return: success return 0, or -1
* Date: 2018-08-14  08-22
*************************************************************/
static inline int head_insert_user_node(user_node_t *p, user_node_t **head)
{
    if (p == NULL)
        return -1;
    p->next = *head;
    *head = p;
    return 0;
}
/************************************************************
* Funtion Name: tail_insert_node
* Parameters:
*     1: a pointer at the node
*     2: list head pointer
* Return: success return 0, or -1
* Date: 2018-08-22
*************************************************************/
static inline int tail_insert_user_node(user_node_t *p, user_node_t **head)
{
    if (p == NULL)
        return -1;

    user_node_t *temp = *head;
    if (temp != NULL)
    {
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = p;
    }
    else
    {
        *head = p;
    }
    return 0;
}
/************************************************************
* Funtion Name: traverse_list
* Parameters: No
* Return: No
* Date: 2018-08-14
*************************************************************/
static inline void traverse_user_list(const user_node_t *head)
{
    const user_node_t *p = head;
    while (p != NULL)
    {
        printf(L_BLUE"%s"NONE"|", p->user.nickname);
        p = p->next;
    }
    printf("\n");
}
/************************************************************
* Funtion Name: free_node
* Parameters:
*    1: a pointer at the node
* Return: No
* Date: 2018-08-14
*************************************************************/
static inline void free_user_node(user_node_t **p)
{
    free(*p);
    *p = NULL;
}
/************************************************************
* Funtion Name: destroy_list
* Parameters:
*     1: a head pointer at the list
* Return: No
* Date: 2018-08-14
*************************************************************/
static inline void destroy_user_list(user_node_t **head)
{
    user_node_t *q = NULL;
    while (*head != NULL)
    {
        q = *head;
        *head = (*head)->next;
        free_user_node(&q);
    }
}
/************************************************************
* Funtion Name: get_list_size
* Parameters:
*     1: list head pointer
* Return: return list size
* Description: Get the current linked list size
* Date: 2018-08-22
*************************************************************/
static inline uint16_t get_user_list_size(const user_node_t *head)
{
    uint16_t count = 0;
    const user_node_t *p = head;
    if (p != NULL)
    {
        while (p != NULL)
        {
            count += 1;
            p = p->next;
        }
    }
    else
    {
        return 0;
    }
    return count;
}

#ifdef __cplusplus
}
#endif
