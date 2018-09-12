#### Linux 聊天室：

这是一个 Linux 下的基于 TCP/IP 协议的一个简易聊天室.

#### 功能：

１．注册

２．登录

３．私聊

４．群聊

５．注册群

６．客户端聊天日志，服务器端操作记录日志

#### 使用方法：

１．代码下载地址

```
git clone git@github.com:xuchunYang1/C_ChatRoom.git

```

２．安装 cmake

```shell
sudo apt-get install cmake
```

３．运行

```
//服务器端
$ cd /home/C_Pros/MyChatRoom/project/server
$ cmake .
$ make -j8
$ ./chatroom_server

//客户端
$ cd /home/C_Pros/MyChatRoom/project/client
$ cmake .
$ make -j8
$ ./chatroom_client
```

#### 问题：

目前还没时间写退出功能，直接＂Ctrl + \＂结束进程，待后期优化，并加入其它功能．

#### 代码维护：

##### 版本 V1.0　2018.09.12
