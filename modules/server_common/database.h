#pragma once
/* include heads */
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <server_common/color.h>
/* defines */


#ifdef __cplusplus
extern "C"{
#endif
/*define sqlite3 callback*/
typedef int (*sqlite3_callback)(void*, int, char**, char**);

/*****************************************************************************
/*名称： open_sqlite3
/*描述： 打开数据库
/*作成日期：2018/07/03
/*参数： A pointer to db
/*返回值：
/*作者：yang
/*******************************************************************************/
int open_sqlite3(char* database);
/*****************************************************************************
/*名称： close_sqlite3
/*描述： 关闭数据库
/*作成日期：2018/07/03
/*参数：No
/*返回值：
/*作者：yang
/*******************************************************************************/
void close_sqlite3();
/*****************************************************************************
/*名称： exec_sqlite3
/*描述： 执行sql
/*作成日期：2018/07/03
/*参数：
/*返回值：No
/*作者：yang
/*******************************************************************************/
int exec_sqlite3(char *sql, sqlite3_callback callback, void *para);
/*****************************************************************************
/*名称： load_song_name
/*描述： 回调函数 for select
/*作成日期：2018/07/04
/*参数：
/*para 是一个一维数组，放字段名和值
/*返回值：
/*作者：yang
/*******************************************************************************/
//[id|000003|nickname|Alice|password|123|reg_date|2018-08-11 17:33:14|]
static inline int print_result(void* para, int n_col,
                               char** col_value, char** col_name)
{
    char *temp = (char *)para; //强制转换 (or para = (char *)para;)
    int i;
    for (i = 0; i < n_col; ++i)
    {
        strcat(temp, col_name[i]);
        strcat(temp, "|"); //add separator
        strcat(temp, col_value[i]);
        strcat(temp, "|");
    }
    return 0;
}
//[Alice|Tom|Blank]
//nickname|Alice|nickname|Tom|nickname|blank|nickname|枫叶子|
static inline int print_name_result(void* para, int n_col,
                                    char** col_value, char** col_name)
{
    char *temp = (char *)para; //强制转换 (or para = (char *)para;)
    int i;
    for (i = 0; i < n_col; ++i)
    {
        strcat(temp, col_value[i]);
        strcat(temp, "|");
    }
    return 0;
}
#ifdef __cplusplus
}
#endif
