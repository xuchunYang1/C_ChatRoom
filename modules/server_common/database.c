#include "database.h"

static sqlite3 *db = NULL; 	//database pointer
/*****************************************************************************
/*名称： open_sqlite3
/*描述： 打开数据库
/*作成日期：2018/07/03
/*参数： A pointer to db
/*返回值：
/*作者：yang
/*******************************************************************************/
int open_sqlite3(char* database)
{
    //return value
    int ret = 0;

    //open database
    ret = sqlite3_open(database, &db);

    //if open failed
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open db: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    //if open succeed
//    printf("Open database\n");
    return 0;
}
/*****************************************************************************
/*名称： close_sqlite3
/*描述： 关闭数据库
/*作成日期：2018/07/03
/*参数：No
/*返回值：
/*作者：yang
/*******************************************************************************/
void close_sqlite3()
{
    sqlite3_close(db);

    //printf("Close database\n");
}
/*****************************************************************************
/*名称： exec_sqlite3
/*描述： 执行sql
/*作成日期：2018/07/03
/*参数：
/*返回值：No
/*作者：yang
/*******************************************************************************/
int exec_sqlite3(char *sql, sqlite3_callback callback, void *para)
{
    //printf("exec_sqlite3\n");
    int ret = 0;
    //error
    char *errmsg = 0;
    //执行SQL
    ret = sqlite3_exec(db, sql, callback, para, &errmsg);
    // if failed, record error log
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, RED"ERROR: %s"NONE"\n", errmsg);
        return -1;
    }
//        printf(L_GREEN"sqlite success"NONE"\n");
    return 0;
}
