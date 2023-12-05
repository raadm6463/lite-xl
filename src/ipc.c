#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "ipc.h"

#if defined(__linux__)
  #define SOCKET_PATH "/tmp/lxl.sock"
  #include <sys/stat.h>
  #include <sys/socket.h>
  #include <sys/un.h>
#endif

static int fd;

int check_socket(void) {
#if defined(__linux__)
  struct stat sb = {0};
  stat(SOCKET_PATH, &sb);

  return S_ISSOCK(sb.st_mode);
#endif
  return 0;
}

void open_socket(void) {
    assert(!fd);

    struct sockaddr_un ipcsock;
    ipcsock.sun_family = AF_UNIX;
    strncpy(ipcsock.sun_path, (char *)SOCKET_PATH, sizeof(ipcsock.sun_path));

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        fd = 0;
        fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
        return;
    }

    bind(fd, (struct sockaddr *) &ipcsock, strlen(ipcsock.sun_path) + sizeof(ipcsock.sun_family));
}

void close_socket(void) {
    if (fd) {
        close(fd);
    }   
}

void destroy_socket(void)
{
    if (fd)
    {
        close_socket();
        unlink(SOCKET_PATH);
    }

}
static int validate_message(Message* msg) {
    if (!msg)
        return 0;

    if (!msg->type_length || !msg->type)
        return 0;

    if (msg->data_length && !msg->data)
        return 0;

    return 1;
}

void send_message(Message* msg) {
    if (!fd || !validate_message(msg)) {
        return;
    }

    send(fd, &msg->type_length, sizeof(msg->type_length), 0);
    if (msg->type_length)
    {
        assert(msg->type);
        send(fd, msg->type, strlen(msg->type), 0);
    }

    send(fd, &msg->data_length, sizeof(msg->data_length), 0);
    if (msg->data_length)
    {
        assert(msg->data);
        send(fd, msg->data, strlen(msg->data), 0);
    }

    close(fd);
}

Message* receive_message(void) {
    if (!fd)
        return NULL;

    Message* msg = malloc(sizeof(Message));
    memset(msg, 0, sizeof(Message));

    recv(fd, &msg->type_length, sizeof(msg->type_length), 0);

    if (msg->type_length)
        printf("%li\n", msg->type_length);

    if (!validate_message(msg))
    {
        free(msg);
        return NULL;
    }

    return msg;
}