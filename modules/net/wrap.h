#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#ifdef __cplusplus
extern "C" {
#endif

void perr_exit(const char *s);

int Socket(int family, int type, int protocol);

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void Listen(int fd, int backlog);

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

int Select(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout);


ssize_t Read(int fd, void *ptr, size_t nbytes);

ssize_t Write(int fd, const void *ptr, size_t nbytes);

void Close(int fd);


ssize_t Readn(int fd, void *vptr, size_t n);

ssize_t Writen(int fd, const void *vptr, size_t n);


static ssize_t my_read(int fd, char *ptr);

ssize_t Readline(int fd, void *vptr, size_t maxlen);

#ifdef __cplusplus
}
#endif
