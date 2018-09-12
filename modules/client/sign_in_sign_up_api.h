#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <client_common/common.h>
#include <protocol/common.h>
#include <net/common.h>

#define SHOW_SIGN_INTERFACE \
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|     "L_YELLOW"Sign in or Sign up Chat Room"NONE"     |\n");\
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|                                      |\n");\
        printf("\t\t|       "WHITE"Sign in          Sign up"NONE"       |\n");\
        printf("\t\t|                                      |\n");\
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|         "GREEN"press left or right"NONE"          |\n");\
        printf("\t\t+--------------------------------------+\n");

#define SHOW_COLOR_SIGN_IN_INTERFACE \
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|     "L_YELLOW"Sign in or Sign up Chat Room"NONE"     |\n");\
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|                                      |\n");\
        printf("\t\t|       "L_RED"Sign in"NONE"          "WHITE"Sign up"NONE"       |\n");\
        printf("\t\t|                                      |\n");\
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|         "GREEN"press left or right"NONE"          |\n");\
        printf("\t\t+--------------------------------------+\n");

#define SHOW_COLOR_SIGN_UP_INTERFACE \
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|     "L_YELLOW"Sign in or Sign up Chat Room"NONE"     |\n");\
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|                                      |\n");\
        printf("\t\t|       "WHITE"Sign in"NONE"          "L_RED"Sign up"NONE"       |\n");\
        printf("\t\t|                                      |\n");\
        printf("\t\t+--------------------------------------+\n");\
        printf("\t\t|         "GREEN"press left or right"NONE"          |\n");\
        printf("\t\t+--------------------------------------+\n");

//外部声明
extern int sockfd;
extern char my_nickname[NICKNAMELEN];
extern char my_id[ID];

#ifdef __cplusplus
extern "C"{
#endif
//ui flag
enum UI_SIGN_FLAG
{
    UI_SIGN_NONE = 0,
    UI_SIGN_IN,
    UI_SIGN_UP,
};

static inline void print_interface_with_color(uint8_t ui_flag)
{
    if (UI_SIGN_IN == ui_flag)
    {
        CLS; //first clear
        SHOW_COLOR_SIGN_IN_INTERFACE;
    }
    else if (UI_SIGN_UP == ui_flag)
    {
        CLS; //first clear
        SHOW_COLOR_SIGN_UP_INTERFACE;
    }
    else if (UI_SIGN_NONE == ui_flag)
    {
        CLS; //first clear
        SHOW_SIGN_INTERFACE;
    }
}

int parse_sign_process(const uint8_t *data_recv, const int count);

void sign_in_sign_up();

static void input_sign_up_info(char *nickname, char *password)
{
    char password_temp[10];
    printf(L_YELLOW"Please enter your nickname:\t"NONE);
    fgets(nickname, 20, stdin);
    nickname[strlen(nickname) - 1] = '\0'; //Replace '\n' with '\0'

    printf(L_YELLOW"Please enter your password:\t"NONE);
    fgets(password, 6, stdin);
    password[strlen(password) - 1] = '\0'; //Replace '\n' with '\0'

    printf(L_YELLOW"Enter your password again:\t"NONE);
    fgets(password_temp, sizeof(password_temp), stdin);
    password_temp[strlen(password_temp) - 1] = '\0'; //Replace '\n' with '\0'

    if (strcmp(password_temp, password) == 0)
    {
//        printf(WHITE"Sending it to the server...\n"NONE);
    }
    else
    {
        printf(L_RED"Two passwords are not match"NONE"\n");
        printf(L_RED"Please sign up again"NONE"\n");
    }
}

static void input_sign_in_info(char *nickname, char *password)
{
    printf(L_CYAN"Please enter your ID:\t"NONE);
    fgets(nickname, 20, stdin);
    nickname[strlen(nickname) - 1] = '\0'; //Replace '\n' with '\0'

    printf(L_CYAN"Please enter your password:\t"NONE);
    fgets(password, 6, stdin);
    password[strlen(password) - 1] = '\0'; //Replace '\n' with '\0'

}

#ifdef __cplusplus
}
#endif



