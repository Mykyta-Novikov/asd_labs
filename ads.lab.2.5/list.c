#pragma once
#include <stdio.h>
#include <float.h>
#include <stdlib.h>

#define LIST_TYPE struct search_step*


struct list_element {
    LIST_TYPE value;
    struct list_element* next;
    struct list_element* previous;
};

struct list {
    struct list_element* first;
    struct list_element* last;
    int length;
};

struct list* create_list();
void delete_list(struct list* list);

void put_element (struct list* list, LIST_TYPE value);
LIST_TYPE poll_first_element (struct list* list);
LIST_TYPE poll_last_element (struct list* list);



/**********************************************************************************************************************/

struct list* create_list () {
    struct list* new_list = malloc(sizeof(struct list));
    if (new_list == NULL) exit(2);

    new_list->first = NULL;
    new_list->last = NULL;
    new_list->length = 0;

    return new_list;
}

/**********************************************************************************************************************/

void put_element (struct list* list, LIST_TYPE value) {
    struct list_element* new_element = malloc(sizeof(struct list_element));
    if (new_element == NULL) exit(2);

    new_element->value = value;
    list->length++;

    new_element->previous = NULL;

    if (list->first == NULL) {
        new_element->next = NULL;
        list->first = list->last = new_element;
    }
    else {
        new_element->next = list->first;
        list->first->previous = new_element;
        list->first = new_element;
    }
}

/**********************************************************************************************************************/

LIST_TYPE poll_first_element (struct list* list) {
    struct list_element* element = list->first;
    list->length--;

    if(list->first->next != NULL)
        list->first->next->previous = NULL;
    list->first = list->first->next;

    if (list->length == 0)
        list->last = NULL;

    LIST_TYPE value = element->value;
    free(element);
    return value;
}

LIST_TYPE poll_last_element (struct list* list) {
    struct list_element* element = list->last;
    list->length--;

    if(list->last->previous != NULL)
        list->last->previous->next = NULL;
    list->last = list->last->previous;

    if (list->length == 0)
        list->first = NULL;

    LIST_TYPE value = element->value;
    free(element);
    return value;
}

/**********************************************************************************************************************/

void delete_list (struct list* list) {
    struct list_element* iterator = list->first;

    while (iterator != NULL) {
        struct list_element* current_element = iterator;
        iterator = current_element->next;
        free(current_element);
    }

    free(list);
}
