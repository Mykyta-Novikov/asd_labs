#include <stdio.h>
#include <float.h>
#include <stdlib.h>

struct list_element {
    double value;
    struct list_element* next;
    struct list_element* previous;
};

struct list {
    struct list_element* first;
    struct list_element* last;
};

struct list* create_list();

void add_element (struct list* list, double value);

void delete_list(struct list* list);

double compute_sum(struct list* list);

/**********************************************************************************************************************/

int main() {
    int n;

    printf("Input n: ");
    scanf("%d", &n);

    struct list* list = create_list();

    printf("Input elements:\n");
    for (int i = 0; i < n; i++) {
        double element;
        scanf("%lg", &element);
        add_element(list, element);
    }

    double result = compute_sum(list);

    delete_list(list);

    printf("\nResult is: %.*lg", DBL_DECIMAL_DIG, result);

    return 0;
}

/**********************************************************************************************************************/

struct list* create_list () {
    struct list* new_list = malloc(sizeof(struct list));

    new_list->first = NULL;
    new_list->last = NULL;

    return new_list;
}

/**********************************************************************************************************************/

void add_element (struct list* list, double value) {
    struct list_element* new_element = malloc(sizeof(struct list_element));

    new_element->value = value;
    new_element->next = NULL;

    if (list->last == NULL) {
        new_element->previous = NULL;

        list->first = list->last = new_element;
    } else {
        new_element->previous = list->last;
        list->last->next = new_element;
        list->last = new_element;
    }
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

/**********************************************************************************************************************/

double compute_sum (struct list* list) {
    struct list_element* forward_iterator = list->first;
    struct list_element* backward_iterator = list->last;

    double sum = 0;

    while (forward_iterator != NULL && backward_iterator != NULL) {
        sum += forward_iterator->value * backward_iterator->value;

        forward_iterator = forward_iterator->next;
        backward_iterator = backward_iterator->previous;
    }

    return sum;
}