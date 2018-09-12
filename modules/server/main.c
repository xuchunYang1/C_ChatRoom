//This is a server
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <protocol/common.h>
#include <net/common.h>
#include <thread_pool/thread_pool.h>
#include <server_common/common.h>

char chat_oper_log[CHAT_RECORD] = {0};

group_node_t *server_group_head = NULL;
static user_node_t *server_online_head = NULL; //list head pointer

static char *database = "./ChatRoomDatabase/ChatRoom.db"; //database file

static char data_search[DATA_SEARCH_LEN]; //A public buffer to store SQL result
static char data_search_two[MAX_NAME_NUM][NAME]; 

static char current_time[30];
static char sender_name[NAME]; //a temp buffer to store sender

static imsg_t msg_out;  //imsg_t

static imsg_reply_info_t reply_info_send;
static imsg_reply_info_t user_online_send;

static imsg_sign_in_sign_up_t sign_recv;
static imsg_conversation_t conversation_recv;
static imsg_cmd_t cmd_recv;
static imsg_group_info_t group_recv;

static imsg_list_t online_list_send;
static imsg_list_t all_people_send;
static imsg_list_t of_group_send;

//prase process function
static int parse_process(const uint8_t *data_recv, const int count, const int sockfd)
{
    imsg_parse_state_t status = 0; //Record analytic state
    int index_temp = 0; // for parse
    int i,ret = -1;
    if(!data_recv || count < 0)
        return -1;

    memset(&msg_out, 0, sizeof(msg_out));
    for(i = 0; i < count; i++)
    {
//        printf("%c", data_recv[i]);
        //first prase data packet
        ret = imsg_parse_char(&msg_out, &status, data_recv[i], &index_temp);
        if(ret == 0)
        {
            printf("解析一个包成功 msg_id: %d \n",msg_out.msgid);
            switch(msg_out.msgid)
            {
                case IMSG_SIGN_IN_SIGN_UP: //sign in or sign up
                    memset(&sign_recv, 0, sizeof(sign_recv));
                    memset(&reply_info_send, 0, sizeof(reply_info_send));
                    imsg_sign_in_sign_up_unpack(&msg_out, &sign_recv);
                    printf("SIGN message:\t%s\t%s \n",sign_recv.nickname, sign_recv.password);

                    uint8_t buf_sign_up[BUFFER_SEND_LEN];
                    uint8_t buf_sign_in[BUFFER_SEND_LEN];
                    uint8_t buf_cur_list[BUFFER_SEND_LEN];
                    uint8_t buf_one_name[BUFFER_SEND_LEN];


                    /*Analyze sign_flag*/
                    if (sign_recv.sign_flag == SIGN_UP)
                    {
                        /*First search*/
                        //Select the information from database
                        memset(data_search, 0, sizeof(data_search));
                        select_register_name(data_search, sign_recv.nickname);
                        char *temp = strstr(data_search, "nickname");
                        /*If NULL, the nickname does not exist*/
                        if (temp == NULL)
                        {   /*generate random id(0-9)*/
                            uint8_t assigned_id[ID_LEN]; // An id waiting to be assigned
                            memset(assigned_id, 0, ID_LEN);
                            generate_random_str(assigned_id, sizeof(assigned_id));
                            /*get current time*/
                            get_current_time(current_time, sizeof(current_time));
                            /*Insert register information into database*/
                            ret = insert_register_info(assigned_id, sign_recv, current_time);
                            if (ret == 0)
                            {
                                reply_info_send.flag = SIGN_UP_SUCCESS;
                                memcpy(reply_info_send.id, assigned_id, strlen(assigned_id) + 1);
                                strcpy(reply_info_send.info, "Sign up success!!!");

                                memcpy(all_people_send.list[all_people_send.list_size],
                                        sign_recv.nickname, strlen(sign_recv.nickname) + 1);
                                all_people_send.list_size += 1;
                                show_two_dim_array(all_people_send.list, all_people_send.list_size);
                            }
                        }
                        else
                        {
                            reply_info_send.flag = SIGN_UP_FAILED;
                            strcpy(reply_info_send.info, "The nickname has been registered");
                        }
                        char result[20] = {0};
                        get_operation_result(result, reply_info_send.flag);
                        /*Record log*/
                        record_user_operation(chat_oper_log, sign_recv.nickname, "sign up", result);
                        /*Send the result to client*/
                        memset(buf_sign_up, 0, sizeof(buf_sign_up));
                        ret = send_reply_info_struct(sockfd, buf_sign_up, &reply_info_send);
                        if (ret > 0)
                            printf(L_GREEN"Reply sign up success ^_^"NONE"\n");
                    }
                    else if (sign_recv.sign_flag == SIGN_IN)
                    {
                        //First check if this user is logged in(select id from list)
                        if(find_id_from_list(sign_recv.nickname, server_online_head) == 0)
                        {
                            reply_info_send.flag = SIGN_IN_FAILED;
                            strcpy(reply_info_send.info, "The account has been logged in!!!");
                        }
                        else
                        {
                            //Select the information from database
                            memset(data_search, 0, sizeof(data_search));
                            select_register_id(data_search, sign_recv.nickname);
                            char *one_temp = strstr(data_search, "id");
                            /*If NULL, the ID does not exist*/
                            if (one_temp == NULL)
                            {
                                reply_info_send.flag = SIGN_IN_FAILED;
                                strcpy(reply_info_send.info, "The ID you entered does not exist");
                            }
                            else
                            {   /*compare two passwords*/
                                char password_buffer[10]; //temp variable to store password
                                memset(password_buffer, 0, sizeof(password_buffer));
                                search_str_from_data_search(password_buffer, data_search, "password");
                                int two_temp = strcmp(sign_recv.password, password_buffer);
                                if (two_temp == 0)
                                {
                                    reply_info_send.flag = SIGN_IN_SUCCESS;
                                    strcpy(reply_info_send.info, "Sign in success!!!");
                                    /*Tell the user his nickname and id*/
                                    search_str_from_data_search(reply_info_send.name,
                                                                data_search, "nickname");
                                    memcpy(reply_info_send.id, sign_recv.nickname,
                                                    strlen(sign_recv.nickname) + 1);

                                    /*create a new node and put in online list*/
                                    /*Add the user to an online linked list*/
                                    user_node_t *user_node = make_user_node(sign_recv.nickname,
                                                                            reply_info_send.name, sockfd);
                                    tail_insert_user_node(user_node, &server_online_head);
                                    traverse_user_list(server_online_head);

                                    /*Prepare current online list to first sign in user*/
                                    prepare_online_list(&online_list_send, server_online_head);

                                    /*Notify others that I am online*/
                                    memset(&user_online_send, 0, sizeof(user_online_send));
                                    user_online_send.flag = ONLINE_ALONE;
                                    memcpy(user_online_send.name, reply_info_send.name,
                                                        strlen(reply_info_send.name) + 1);

                                    /*Tell me my groups*/
                                    memset(sender_name, 0, sizeof(sender_name));
                                    get_user_of_group(&of_group_send, sender_name,
                                                        &sockfd, server_online_head);
                                }
                                else
                                {
                                    reply_info_send.flag = SIGN_IN_FAILED;
                                    strcpy(reply_info_send.info, "The ID does not match the password");
                                }
                            }
                        }
                        char result[20] = {0};
                        get_operation_result(result, reply_info_send.flag);
                        record_user_operation(chat_oper_log, sender_name, "sign up", result);
                        /*Send the result to client*/
                        memset(buf_sign_in, 0, sizeof(buf_sign_in));
                        ret = send_reply_info_struct(sockfd, buf_sign_in, &reply_info_send);
                        if (ret > 0)
                            printf(L_GREEN"Reply sign in success ^_^"NONE"\n");

                        /*Only success , Sending*/
                        if (reply_info_send.flag == SIGN_IN_SUCCESS)
                        {
                            /*Prepare to send the online user to self*/
                            memset(buf_cur_list, 0, sizeof(buf_cur_list));
                            ret = send_list_struct(sockfd, buf_cur_list, &online_list_send);
                            printf("send cur_list\n");

                            /*Prepare to send the online user to all client*/
                            memset(buf_one_name, 0, sizeof(buf_one_name));
                            send_online_user(sockfd, server_online_head,
                                             buf_one_name, &user_online_send);
                            printf("send other name\n");
                        }
                    }
                    break;
                case IMSG_CONVERSATION: //conversation
                    memset(&conversation_recv, 0, sizeof(conversation_recv));
                    imsg_conversation_unpack(&msg_out, &conversation_recv);
                    printf("谈话数据包 name: %s data: %s \n",conversation_recv.target_name,
                                                            conversation_recv.data);

                    uint8_t buf_people[BUFFER_SEND_LEN];
                    uint8_t buf_group[BUFFER_SEND_LEN];

                    uint16_t send_fd = 0;

                    /*Analyze target_flag*/
                    if (conversation_recv.target_flag == TARGET_IS_PEOPLE)
                    {
                        memset(sender_name, 0, sizeof(sender_name));
                        if(find_sockfd_from_list(&sockfd, sender_name, server_online_head) == 0)
                        {
                            /*First find out if there is this person in the database*/
                            memset(data_search, 0, sizeof(data_search));
                            select_register_name(data_search, conversation_recv.target_name);
                            char *temp = strstr(data_search, "nickname");
                            /*If NULL, the nickname does not exist*/
                            if (temp == NULL)
                            {
                                conversation_recv.target_flag = PEOPLE_NOBODY;
                                /*Send reminder information to the user*/
                                send_fd = sockfd;
                                sprintf(conversation_recv.data, "User [%s] does not exist!!!",
                                                                conversation_recv.target_name);
                            }
                            else
                            {
                                if ((ret = find_name_from_list(conversation_recv.target_name,
                                                                 &send_fd, server_online_head)) == 0)
                                {   /*online*/
                                    conversation_recv.target_flag = PEOPLE_SUCCESS;
                                }
                                else
                                {   /*offline*/
                                    conversation_recv.target_flag = PEOPLE_FAILED;
                                    /*Send reminder information to the user*/
                                    send_fd = sockfd;
                                    sprintf(conversation_recv.data, "[%s] is not online, the message is not sent!!!",
                                            conversation_recv.target_name);
                                }
                            }
                            /*Send to another user or himself*/
                            memset(conversation_recv.target_name, 0, sizeof(conversation_recv.target_name));
                            memcpy(conversation_recv.target_name, sender_name, strlen(sender_name) + 1);
                            memset(buf_people, 0, sizeof(buf_people));
                            ret = send_conversation_struct(send_fd, buf_people, &conversation_recv);
                            if (ret > 0)
                                printf(L_GREEN"Reply conversation --private success ^_^"NONE"\n");
                        }
                    }
                    else if (conversation_recv.target_flag == TARGET_IS_GROUP)
                    {
                        /*First find out group member in the database*/
                        char member[NAME][NAME] = {0};
                        find_group_member(member, conversation_recv.target_name,
                                                              server_group_head);
                        uint8_t act_len = get_two_dim_len(member);
                        printf("Group member: %d\n", act_len);
                        show_two_dim_array(member, act_len);

                        memset(sender_name, 0, sizeof(sender_name));
                        if(find_sockfd_from_list(&sockfd, sender_name, server_online_head) == 0)
                        {
                            conversation_recv.target_flag = GROUP_SUCCESS;
                            strcat(conversation_recv.target_name, " - ");
                            strcat(conversation_recv.target_name, sender_name);

                            int i;
                            for (i = 0; i < act_len; ++i)
                            {
                                if (ret = find_name_from_list(member[i], &send_fd, server_online_head) == 0)
                                {
                                    /*Send to every group member*/
                                    if (send_fd != sockfd)
                                    {
                                        memset(buf_group, 0, sizeof(buf_group));
                                        ret = send_conversation_struct(send_fd, buf_group, &conversation_recv);
                                        if (ret > 0)
                                            printf(L_GREEN"Reply conversation --group success ^_^"NONE"\n");
                                    }
                                }
                            }
                        }
                    }
                    break;
                case IMSG_CMD: //cmd
                    memset(&cmd_recv, 0, sizeof(cmd_recv));
                    imsg_cmd_unpack(&msg_out,&cmd_recv);
                    printf("cmd %d \n",cmd_recv.cmd);

                    uint8_t buf_all_people[BUFFER_SEND_LEN];
                    uint8_t buf_of_group[BUFFER_SEND_LEN]; 

                    switch (cmd_recv.cmd)
                    {
                        case CMD_ALL_PEOPLE:
                            /*Send all people to user */
                            memset(buf_all_people, 0, sizeof(buf_all_people));
                            ret = send_list_struct(sockfd, buf_all_people, &all_people_send);
                            break;
                        case CMD_OF_GROUP:
                            /*Send of group to user*/
                            memset(buf_of_group, 0, sizeof(buf_of_group));
                            ret = send_list_struct(sockfd, buf_of_group, &of_group_send);
                            break;
                    }
                    break;
                case IMSG_GROUP_INFO:
                    memset(&group_recv, 0, sizeof(group_recv));
                    memset(&reply_info_send, 0, sizeof(reply_info_send));
                    imsg_group_info_unpack(&msg_out, &group_recv);

                    uint8_t buf_group_info[BUFFER_SEND_LEN];

                    /*get current time*/
                    get_current_time(current_time, sizeof(current_time));

                    //Select the information from database
                    memset(data_search, 0, sizeof(data_search));
                    select_group_name(data_search, group_recv.name);

                    char *temp = strstr(data_search, "group_name");
                    /*If NULL, the group name does not exist*/
                    if (temp == NULL)
                    {
                        /*Insert group information into database*/
                        ret = insert_group_info(group_recv, current_time);
                        if (ret == 0)
                        {
                            reply_info_send.flag = GROUP_UP_SUCCESS;
                            strcpy(reply_info_send.info, "Register group successfully");
                            memcpy(reply_info_send.name, group_recv.name,
                                                strlen(group_recv.name) + 1);

                            /*Put group information into the linked list*/
                            group_node_t *group_node = make_group_node(group_recv.name,
                                                                           group_recv.member,
                                                                           group_recv.size);
                            insert_group_node(group_node, &server_group_head);
                            traverse_group_list(server_group_head);
                        }
                    }
                    else
                    {
                        reply_info_send.flag = GROUP_UP_FAILED;
                        strcpy(reply_info_send.info, "Register group failed: The group name has been registered");
                    }
                    char result[20] = {0};
                    get_operation_result(result, reply_info_send.flag);
                    /*Record log*/
                    record_user_operation(chat_oper_log, group_recv.founder, "group up", result);
                    /*Send reply*/
                    memset(buf_group_info, 0, sizeof(buf_group_info));
                    ret = send_reply_info_struct(sockfd, buf_group_info, &reply_info_send);
                    if (ret > 0)
                        printf(L_GREEN"Reply group success ^_^"NONE"\n");
                    break;
            }
        }
    }
    printf("\n");
    return 0;
}

static void *callback_parse_process(void *p)
{
    data_in_t *para = (data_in_t *)p;
    int ret = 0;
    bool work1_exit_flag = false;
    while(!work1_exit_flag)
    {
        ret = parse_process(para->data_recv, para->count, para->sockfd);
        if (ret == 0)
            work1_exit_flag = true;
    }
}

int main(int argc, char* argv[])
{
    pool_init(MAXTHREADS);          // Init thread pool
    open_sqlite3(database);         // Open the database

    /*Get the name of everyone from the database*/
    load_all_people(&all_people_send);

    /*Get all groups from the database*/
    load_all_groups();

    /*Create log file*/
    FILE *fp = create_log_file();

    /*Server init*/
    int i, maxi;
	int maxfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];                 //存放connfd
	fd_set readable_set, allset;
	
    ssize_t n_read = 0;
    uint8_t buffer_recv[BUFFER_RECV_LEN];           //An empty buffer
    char str[INET_ADDRSTRLEN];                      //转换地址需要

    struct sockaddr_in cliaddr;                     //定义全局的客户端地址
	socklen_t cliaddr_len;

    int listenfd;                                   //监听描述符
    listenfd = Socket(PF_INET, SOCK_STREAM, 0);     //流服务 即TCP/IP协议
                                                    //AF_INET地址族对应PF_INET协议族(TCP/IPv4协议)
    /*避免bind error*/
    int reuse = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
        return -1;
    }
	
    /*初始化服务器地址参数*/
    struct sockaddr_in servaddr;                    //服务器地址
    bzero(&servaddr, sizeof(servaddr));	
    servaddr.sin_family  		= AF_INET;  
    servaddr.sin_addr.s_addr 	= htonl(INADDR_ANY);
    servaddr.sin_port 			= htons(SERVER_PORT);
	
    Bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    Listen(listenfd, 20);
    
    /*初始化变量*/
    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; ++i)
    {
    	client[i] = -1;
    }
    
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for ( ; ; )
    {
        readable_set = allset;      //赋值
        /*nready为就绪的描述符个数*/
    	nready = Select(maxfd + 1, &readable_set, NULL, NULL, NULL);
		
        /*处理客户端连接*/
    	if (FD_ISSET(listenfd, &readable_set))
    	{
    		cliaddr_len = sizeof(cliaddr);
    		connfd = Accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddr_len);
    		printf("received form %s at PORT %d\n",
    				inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
    				ntohs(cliaddr.sin_port));

    		for (i = 0; i < FD_SETSIZE; ++i)
            {	/*单纯地控制i在1024之内*/
    			if (client[i] < 0)
    			{
    				client[i] = connfd;
    				break;
    			}
    		}
            if (i == FD_SETSIZE)         //检测客户端的连接个数是否超过1024
    		{
    			fputs("too much clients\n", stderr);
    			exit(1);
    		}
			if (i > maxi)
    		{
                maxi = i;               //更新client[i]最大索引
    		}
    		
            FD_SET(connfd, &allset);    //添加描述符到集合里
    		if (connfd > maxfd)
    		{
                maxfd = connfd;         //更新最大的描述符
    		}
    		
            if (--nready == 0)          //处理一个连接则减一
    		{
    			continue;
    		}
    	}
		
        /*处理客户端读写*/
    	for (i = 0; i <= maxi; ++i)
    	{
            if ((sockfd = client[i]) < 0) //Assign connfd to sockfd
    		{
    			continue;
    		} 

    		if (FD_ISSET(sockfd, &readable_set))
            {
                memset(buffer_recv, 0, sizeof(buffer_recv));
                n_read = Read(sockfd, buffer_recv, MAXREAD);
    			if (n_read == 0)
    			{
    				Close(sockfd);
    				FD_CLR(sockfd, &allset);
    				client[i] = -1;
    			}
    			else
    			{
                    data_in_t in_data;
                    memset(&in_data, 0, sizeof(in_data));
                    data_in_pack(&in_data, buffer_recv, n_read, sockfd);

                    /*Add the parsing task to the thread pool*/
                    pool_add_worker(callback_parse_process, &in_data);
    			}

    			if (--nready == 0)
    			{
    				break;
    			}
    		}
    	}
    }

    destroy_user_list(&server_online_head);     //Destroy online list
    destroy_group_list(&server_group_head);     //Destroy group list
    close_sqlite3();                            //Close the database
    pool_destroy();                             //Destroy thread pool

    int total = fwrite(chat_oper_log, sizeof(char),
                       strlen(chat_oper_log), fp);
    printf("write = %d\n", total);
    fclose(fp);

    return 0;
}
