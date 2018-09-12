#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
//#include <protocol/common.h>
#include <net/common.h>
#include <client_common/common.h>

#define CLS system("clear")
#define SHOW_CHAT_MAIN_INTERFACE\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |              "L_CYAN"Welcome To The Chat Room"NONE"              |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                       "WHITE"Grouping"NONE"                     |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |         "WHITE"Private chat"NONE"            "WHITE"Group chat"NONE"         |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |       "GREEN"press up or left or right or Esc exit"NONE"        |\n");\
        printf("\t    +----------------------------------------------------+\n");

#define SHOW_CHAT_MAIN_INTERFACE_COLOR_GROUPING\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |              "L_CYAN"Welcome To The Chat Room"NONE"              |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                       "L_RED"Grouping"NONE"                     |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |         "WHITE"Private chat"NONE"            "WHITE"Group chat"NONE"         |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |       "GREEN"press up or left or right or Esc exit"NONE"        |\n");\
        printf("\t    +----------------------------------------------------+\n");


#define SHOW_CHAT_MAIN_INTERFACE_COLOR_GROUP\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |              "L_CYAN"Welcome To The Chat Room"NONE"              |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                       "WHITE"Grouping"NONE"                     |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |         "WHITE"Private chat"NONE"            "L_RED"Group chat"NONE"         |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |       "GREEN"press up or left or right or Esc exit"NONE"        |\n");\
        printf("\t    +----------------------------------------------------+\n");


#define SHOW_CHAT_MAIN_INTERFACE_COLOR_PRIVATE\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |              "L_CYAN"Welcome To The Chat Room"NONE"              |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                       "WHITE"Grouping"NONE"                     |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |         "L_RED"Private chat"NONE"            "WHITE"Group chat"NONE"         |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    |                                                    |\n");\
        printf("\t    +----------------------------------------------------+\n");\
        printf("\t    |       "GREEN"press up or left or right or Esc exit"NONE"        |\n");\
        printf("\t    +----------------------------------------------------+\n");


extern char my_nickname[NICKNAMELEN];

#ifdef __cplusplus
extern "C"{
#endif

//ui flag
enum UI_CHAT_FLAG
{
    UI_CHAT_NONE = 0,
    UI_CHAT_GROUPING,
    UI_CHAT_GROUP_CHAT,
    UI_CHAT_PRIVATE_CHAT,
};
static inline void print_main_interface_with_color(uint8_t ui_flag)
{
    if (UI_CHAT_GROUPING == ui_flag)
    {
        CLS; //first clear
        SHOW_CHAT_MAIN_INTERFACE_COLOR_GROUPING;
    }
    else if (UI_CHAT_GROUP_CHAT == ui_flag)
    {
        CLS; //first clear
        SHOW_CHAT_MAIN_INTERFACE_COLOR_GROUP;
    }
    else if (UI_CHAT_PRIVATE_CHAT == ui_flag)
    {
        CLS; //first clear
        SHOW_CHAT_MAIN_INTERFACE_COLOR_PRIVATE;
    }
    else if (UI_CHAT_NONE == ui_flag)
    {
        CLS;
        SHOW_CHAT_MAIN_INTERFACE;
    }
}
//[Blank Tom Lily Yang]
static void get_group_member_from_one_dim(char buffer[][MEMBER_NAME_LEN],
                                          const uint8_t *size, const char *str)
{
    int i = 0;
    int j = 0;
    //First store founder
    memcpy(buffer[0], my_nickname, strlen(my_nickname));
    const char *temp = str;
    while (*temp != ' ')
    {
        buffer[1][j] = *temp;
        j += 1;
        temp++;
    }
    for (i = 2; i < *size; i++)
    {
        j = 0;
        while (1)
        {
            temp++;
            if (*temp != ' ' && *temp != '\0')
            {
                buffer[i][j] = *temp;
                j += 1;
            }
            else
            {
                buffer[i][j] = '\0';
                break;
            }
        }
    }
}

static void input_group_info(char *group_name, uint8_t *group_size,
                             char group_member[][MEMBER_NAME_LEN])
{
    printf(L_YELLOW"Please enter the group name to be registered:"NONE"\n");
    fgets(group_name, 20, stdin);
    group_name[strlen(group_name) - 1] = '\0'; //Replace '\n' with '\0'

    printf(L_YELLOW"Please enter the group size:"NONE"\n");
    scanf("%hhd", group_size);
    getchar(); //get '\n' from buffer

    char member_buffer[256];
    memset(member_buffer, 0, sizeof(member_buffer));
    printf(L_YELLOW"Please enter group member:"NONE"\n");
    fgets(member_buffer, sizeof(member_buffer), stdin);
    member_buffer[strlen(member_buffer) - 1] = '\0'; //Replace '\n' with '\0'

    get_group_member_from_one_dim(group_member, group_size, member_buffer);
}

#ifdef __cplusplus
}
#endif
