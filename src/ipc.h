#ifndef IPC_H
#define IPC_H

#include <stddef.h>

typedef struct {
    size_t type_length;
    char* type;
    size_t data_length;
    char* data;
} Message;

int check_socket(void);
void open_socket(void);
void closet_socket(void);

void send_message(Message* message);
Message* receive_message(void);

#endif