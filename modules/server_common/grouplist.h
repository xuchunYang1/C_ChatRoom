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
    char name[20];
    char member[20][20];
    uint8_t size;
}group_t;

typedef struct group_node
{
    group_t group;
    struct group_node *next;
}group_node_t;
/************************************************************
* Function Name: make_node
* Parameters:
*    1: user name or group name
* Return: a pointer at the node
* Date: 2018-08-28
*************************************************************/
static inline group_node_t *make_group_node(char *name, char member[][20], uint16_t size)
{
    if (name == NULL)
        return NULL;
    group_node_t *p = (group_node_t *)malloc(sizeof(group_node_t));
    if (p == NULL)
    {
        printf("malloc failed\n");
        exit(1);
    }
    memcpy(p->group.name, name, strlen(name) + 1);
    memcpy(p->group.member, member, size*20);
    p->group.size = size;
    p->next = NULL;

    return p;
}
/************************************************************
* Funtion Name: tail_insert_node
* Parameters:
*     1: a pointer at the node
*     2: list head pointer
* Return: success return 0, or -1
* Date: 2018-08-28
*************************************************************/
static inline int insert_group_node(group_node_t *p, group_node_t **head)
{
    if (p == NULL)
        return -1;

    group_node_t *temp = *head;
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
* Date: 2018-08-28
*************************************************************/
//static inline void traverse_group_list(const group_node_t *head)
//{
//    const group_node_t *p = head;
//    while (p != NULL)
//    {
////        printf(L_BLUE"%s"NONE"|", p->group.name);
//        printf("Group name: %s\n", p->group.name);
//        show_two_dim_array(p->group.member, p->group.size);
//        printf("\n");
//        p = p->next;
//    }
//    printf("\n");
//}
/************************************************************
* Funtion Name: free_node
* Parameters:
*    1: a pointer at the node
* Return: No
* Date: 2018-08-28
*************************************************************/
static inline void free_group_node(group_node_t **p)
{
    free(*p);
    *p = NULL;
}
/************************************************************
* Funtion Name: destroy_list
* Parameters:
*     1: a head pointer at the list
* Return: No
* Date: 2018-08-28
*************************************************************/
static inline void destroy_group_list(group_node_t **head)
{
    group_node_t *q = NULL;
    while (*head != NULL)
    {
        q = *head;
        *head = (*head)->next;
        free_group_node(&q);
    }
}
/************************************************************
* Funtion Name: get_list_size
* Parameters:
*     1: list head pointer
* Return: return list size
* Description: Get the current linked list size
* Date: 2018-08-28
*************************************************************/
static inline uint16_t get_group_list_size(const group_node_t *head)
{
    uint16_t count = 0;
    const group_node_t *p = head;
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
