#include "sign_in_sign_up_api.h"


static int n_read;
static imsg_t msg_out;  //imsg_t
static imsg_reply_info_t reply_recv;
static char buffer_send[BUFFER_SEND_LEN];
static char buffer_recv[BUFFER_RECV_LEN];

static imsg_sign_in_sign_up_t sign_send;
static uint8_t ui_sign_flag = UI_SIGN_NONE;

static bool sign_in_flag = false;
static uint8_t sign_up_flag = -1;

//prase process for sign_in_sign_up
int parse_sign_process(const uint8_t *data_recv, const int count)
{
    imsg_parse_state_t status = 0; //for parse
    int index_temp = 0; //for parse char
    bool success_flag = false;  //sign in success
    int i,ret = -1;
    if(!data_recv || count < 0)
        return -1;

//    printf("read: %d\n", count);
    memset(&msg_out, 0, sizeof(msg_out));
    for(i = 0; i < count; i++)
    {
        //first prase data packet
//        printf("%c", data_recv[i]);
        ret = imsg_parse_char(&msg_out, &status, data_recv[i], &index_temp);
        if(ret == 0)
        {
            switch (msg_out.msgid)
            {
                case IMSG_REPLY:
                    memset(&reply_recv, 0, sizeof(reply_recv));
                    imsg_reply_info_unpack(&msg_out, &reply_recv);
                    /*Analyze sign_flag*/
                    switch (reply_recv.flag)
                    {
                        case SIGN_UP_SUCCESS:
                            sign_up_flag = 1;
                            break;
                        case SIGN_IN_SUCCESS:
                            printf(L_GREEN"%s"NONE"\n", reply_recv.info);
                            memset(my_id, 0, sizeof(my_id));
                            memcpy(my_id, reply_recv.id, strlen(reply_recv.id) + 1);
                            memset(my_nickname, 0, sizeof(my_nickname));
                            memcpy(my_nickname, reply_recv.name,
                                                strlen(reply_recv.name) + 1);

                            success_flag = true;
                            printf(L_GREEN"Please start chatting"NONE"\n");
                            //show main interface
                            //Start chat
                            break;
                        case SIGN_UP_FAILED:
                            sign_up_flag = 2;
                            break;
                        case SIGN_IN_FAILED:
                            sign_in_flag = true;
                            break;
                    }
                    break;
            }
        }
    }
    printf("\n");
    //sign in success
    if (success_flag)
        return 0;
}

void sign_in_sign_up()
{
    uint8_t option = 0;
    bool exit_flag = false;

    enable_raw_mode(); //close echo
    while (!exit_flag)
    {
        print_interface_with_color(ui_sign_flag);
        if (sign_in_flag)
        {
            printf(RED"Sign in failed:%s"NONE"\n", reply_recv.info);
            printf(L_GREEN"Please sign in again"NONE"\n");
        }
        if (sign_up_flag == 1)
        {
            printf(L_GREEN"%s"NONE"\n", reply_recv.info);
            printf("Your ID is "L_CYAN"%s"NONE"\n", reply_recv.id);
            printf(L_GREEN"Please start sign in"NONE"\n");

        }
        else if (sign_up_flag == 2)
        {
            printf(RED"Sign up failed: %s"NONE"\n", reply_recv.info);
            printf(L_GREEN"Please sign up again"NONE"\n"NONE);
        }

        while (!kbhit2())
        {
            usleep(1000*5); //sleep 5ms
        }
        option = getchar();
        switch (option)
        {
            case DIRECTION_TAG_1:
                option = getchar(); //get second char
                if (DIRECTION_TAG_2 == option)
                {
                    option = getchar(); //get third char
                    switch (option)
                    {
                        case KEY_LEFT:
                            ui_sign_flag = UI_SIGN_IN;
                            break;
                        case KEY_RIGHT:
                            ui_sign_flag = UI_SIGN_UP;
                            break;
                        default:
                            break;
                    }
                }
                break;
            case ENTER:
                if (ui_sign_flag != UI_SIGN_NONE)
                {
                    disable_raw_mode(); //open echo
                    memset(&sign_send, 0, sizeof(sign_send));
                    if (ui_sign_flag == UI_SIGN_UP)
                    {
                        sign_send.sign_flag = SIGN_UP; //sign up flag
                        input_sign_up_info(sign_send.nickname, sign_send.password);
                    }
                    else if (ui_sign_flag == UI_SIGN_IN)
                    {
                        sign_send.sign_flag = SIGN_IN; //sign in flag
                        input_sign_in_info(sign_send.nickname, sign_send.password);
                    }
                    enable_raw_mode(); //close echo
                    ui_sign_flag = UI_SIGN_NONE;

                    /*Send to server*/
                    memset(buffer_send, 0, sizeof(buffer_send));
                    int ret = send_sign_in_sign_up_struct(sockfd, buffer_send, &sign_send);

                    /*Read from server*/
                    memset(buffer_recv, 0, sizeof(buffer_recv));
                    n_read = Read(sockfd, buffer_recv, MAXREAD);
                    /*Start to parse*/
                    if (parse_sign_process(buffer_recv, n_read) == 0)
                        exit_flag = true; //If Sign in success, Can quit
                }                
                break;
            default:
                break;
        }
    }
    disable_raw_mode(); //open echo
}
