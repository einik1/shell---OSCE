//
// Created by ubuntu on 11/24/19.
//

#ifndef LINKLIST_MK_H
#define LINKLIST_MK_H

typedef struct  BSPS{
    pid_t  pid;
    char* status;
    char* command;

}BSP;

/* A linked list node */
struct node {
    BSP data;
    struct node *next;
};

#endif //LINKLIST_MK_H