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

typedef struct name
{
    char name[20];
    struct name *next;
}name_t;
/************************************************************
* Function Name: make_node
* Parameters:
*    1: user name or group name
* Return: a pointer at the node
* Date: 2018-08-28
*************************************************************/
static inline name_t *make_name_node(const char *name)
{
    if (name == NULL)
        return NULL;
    name_t *p = (name_t *)malloc(sizeof(name_t));
    if (p == NULL)
    {
        printf("malloc failed\n");
        exit(1);
    }
    memcpy(p->name, name, strlen(name) + 1);
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
static inline int head_insert_name_node(name_t *p, name_t **head)
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
* Date: 2018-08-28
*************************************************************/
static inline int tail_insert_name_node(name_t *p, name_t **head)
{
    if (p == NULL)
        return -1;

    name_t *temp = *head;
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
static inline void traverse_name_list(const name_t *head)
{
    const name_t *p = head;
    while (p != NULL)
    {
        printf(L_BLUE"%s"NONE"|", p->name);
        p = p->next;
    }
    printf("\n");
}
/************************************************************
* Funtion Name: free_node
* Parameters:
*    1: a pointer at the node
* Return: No
* Date: 2018-08-28
*************************************************************/
static inline void free_name_node(name_t **p)
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
static inline void destroy_name_list(name_t **head)
{
    name_t *q = NULL;
    while (*head != NULL)
    {
        q = *head;
        *head = (*head)->next;
        free_name_node(&q);
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
static inline uint16_t get_name_list_size(const name_t *head)
{
    uint16_t count = 0;
    const name_t *p = head;
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
