//
// Created by ubuntu on 11/24/19.
//

// C program for generic linked list
#ifndef MAXCOM
#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>




#include "LinkList_MK.h"


struct node* head =NULL ;
struct node *current = NULL;
struct node *last=NULL;

struct node* getHead(){
    return head;
}

//display the list
void printList() {
    struct node *ptr = head;
    printf("\n[ ");

    //start from the beginning
    while(ptr != NULL) {
        printf("(%s) ",(ptr->data.command));
        ptr = ptr->next;
    }

    printf(" ]");
}

//insert link at the first location
void insertFirst( BSP data) {
    //create a link
    struct node *link = (struct node*) malloc(sizeof(struct node));


    link->data = data;

    //point it to old first node
    link->next = head;

    //point first to new first node
    head = link;
}


void insertLast( BSP data) {
    //create a link
    struct node *link = (struct node *) malloc(sizeof(struct node));


    link->data = data;

    link->next=NULL;

    //point it to old first node
    if (last){
        last->next = link;
        last = link;
    }
    else{
        last = link;
        head->next=last;
    }
}


//delete first item
struct node* deleteFirst() {

    //save reference to first link
    struct node *tempLink = head;

    //mark next to first link as first
    head = head->next;

    //return the deleted link
    return tempLink;
}

//is list empty
bool isEmpty() {
    return head->next == NULL;
}

int length() {
    int length = 0;
    struct node *current;

    for(current = head->next; current != NULL; current = current->next) {
        length++;
    }

    return length;
}
/*
//find a link with given key
struct node* find(int key) {

    //start from the first link
    struct node* current = head;

    //if list is empty
    if(head == NULL) {
        return NULL;
    }

    //navigate through list
    while(current->key != key) {

        //if it is last node
        if(current->next == NULL) {
            return NULL;
        } else {
            //go to next link
            current = current->next;
        }
    }

    //if data found, return the current Link
    return current;
}
*/
//delete a link with given key
struct node* node_delete(int key_pos_to_del) {

    //start from the first link
    struct node* current = head;
    struct node* previous = NULL;

    //if list is empty
    if(head == NULL) {
        return NULL;
    }

    //navigate through list
    while( key_pos_to_del>0) {
        //if it is last node
        if(current->next == NULL) {
            return NULL;
        } else {
            previous = current;
            current = current->next;
        }
        key_pos_to_del--;
    }

    //found a match, update the link
    if(current == head) {
        //change first to point to next link
        head = head->next;
    }
    if(current == last) {
        //change first to point to next link
        previous->next=NULL;
        last = previous;
    }
    else {
        //bypass the current link
        previous->next = current->next;
    }

    return current;
}


//delete a link with given key
struct node* node_delete_by_pid(pid_t pid_to_del) {

    //start from the first link
    struct node* current = head;
    struct node* previous = NULL;

    //if list is empty
    if(head == NULL) {
        return NULL;
    }

    //navigate through list
    while( current->data.pid!=pid_to_del && current) {
        //if it is last node
        if(current->next == NULL) {
            return NULL;
        } else {
            previous = current;
            current = current->next;
        }
    }

    //found a match, update the link
    if(current == head) {
        //change first to point to next link
        head = head->next;
    }
    if(current == last) {
        //change first to point to next link
        previous->next=NULL;
        last = previous;
    }
    else {
        //bypass the current link
        previous->next = current->next;
    }

    return current;
}



pid_t findPidInList(int location){

    struct node* current = head;

    while( current != NULL && location > 0 ){
        current = current->next;
        location--;
    }
    if (current)
        return current->data.pid;
    return -1;
}

#endif