#pragma once
#include "list.c"


/*
 * Setter and getter for values of graph's matrix.
 */

unsigned int graph_get (struct graph *graph, int i, int j) {
    return graph->matrix[i * graph->n + j];
}

void graph_set (struct graph *graph, int i, int j, unsigned int value) {
    graph->matrix[i * graph->n + j] = value;
}



/**********************************************************************************************************************/
/*
 * Singleton getters of pens.
 */

HPEN nodes_pen;
HPEN active_nodes_pen;
HPEN inactive_nodes_pen;
HPEN highlighted_nodes_pen;
HPEN get_nodes_pen (enum drawing_style style) {
    HPEN pen;
    switch (style) {
        case STANDART:
            if (nodes_pen == NULL)
                nodes_pen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
            pen = nodes_pen;
            break;
        case ACTIVE:
            if (active_nodes_pen == NULL)
                active_nodes_pen = CreatePen(PS_SOLID, 3, RGB(50, 0, 255));
            pen = active_nodes_pen;
            break;
        case INACTIVE:
            if (inactive_nodes_pen == NULL)
                inactive_nodes_pen = CreatePen(PS_SOLID, 1, RGB(90, 90, 150));
            pen = inactive_nodes_pen;
            break;
        case HIGHLIGHTED:
            if (highlighted_nodes_pen == NULL)
                highlighted_nodes_pen = CreatePen(PS_SOLID, 4, RGB(180, 180, 50));
            pen = highlighted_nodes_pen;
            break;
        default:
            exit(3);
    }
    return pen;
}

HPEN edges_pen;
HPEN active_edges_pen;
HPEN inactive_edges_pen;
HPEN get_edges_pen (enum drawing_style style) {
    HPEN pen;
    switch (style) {
        case STANDART:
            if (edges_pen == NULL)
                edges_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            pen = edges_pen;
            break;
        case ACTIVE:
            if (active_edges_pen == NULL)
                active_edges_pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            pen = active_edges_pen;
            break;
        case INACTIVE:
            if (inactive_edges_pen == NULL)
                inactive_edges_pen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
            pen = inactive_edges_pen;
            break;
        default:
            exit(3);
    }
    return pen;
}

/**********************************************************************************************************************/
/*
 * Function for normalizing a vector (dividing by its length).
 */

struct coord normalize_vector (struct coord vector) {
    double length = sqrt(vector.x.d * vector.x.d + vector.y.d * vector.y.d);
    struct coord result = {
            .x.d = vector.x.d / length,
            .y.d = vector.y.d / length
    };
    return result;
}

/**********************************************************************************************************************/
/*
 * Functions for manipulations with stacks and queues
 */

deque deque_create () {
    return create_list();
}

void deque_delete (deque deque) {
    delete_list(deque);
}

struct search_step* deque_poll (deque deque, boolean as_queue) {
    if (as_queue)
        return poll_last_element(deque);
    else
        return poll_first_element(deque);
}

void deque_put (deque deque, struct search_step* value) {
    put_element(deque, value);
}

boolean deque_is_empty (deque deque) {
    return deque->length == 0;
}

/**********************************************************************************************************************/

struct search_step* search_step_create (int size, struct search_step* old_step) {
    struct search_step* new_step = malloc(sizeof(struct search_step) + size * sizeof(int));
    if (new_step == NULL) exit(2);

    if (old_step != NULL) {
        int count = new_step->count = old_step->count;
        for (int i = 0; i < count; i++)
            new_step->nodes_indexes[i] = old_step->nodes_indexes[i];
        new_step->last = old_step->last;
    }
    else
        new_step->count = 0;
}

void search_step_add (struct search_step* search_step, int value) {
    search_step->nodes_indexes[search_step->count] = value;
    search_step->count++;
    search_step->last = value;
}

/**********************************************************************************************************************/

enum drawing_style get_node_style (int index, struct search_step* search_step, enum ui_state screen) {
    enum drawing_style node_style;
    if (screen == INITIAL || screen == SPANNING_TREE)
        node_style = STANDART;
    else if (index == search_step->last)
        node_style = HIGHLIGHTED;
    else {
        node_style = INACTIVE;
        for (int j = 0; j < search_step->count; j++)
            if (index == search_step->nodes_indexes[j])
                node_style = ACTIVE;
    }
    return node_style;
}

/**********************************************************************************************************************/

enum drawing_style get_edge_style (int start, int end, int size, struct search_step* search_step,
        enum ui_state screen, const boolean* spanning_tree_drawing_matrix) {
    enum drawing_style edge_style;
    switch (screen) {
        case INITIAL:
            edge_style = STANDART;
            break;
        case SPANNING_TREE:
            edge_style = spanning_tree_drawing_matrix[start * size + end] ? STANDART : HIDDEN;
            break;
        case MAIN: {
            edge_style = INACTIVE;
            for (int i = 1; i < search_step->count; i++)
                if (start == search_step->nodes_indexes[i - 1] && end == search_step->nodes_indexes[i])
                    edge_style = ACTIVE;
        }
    }
    return edge_style;
}

/**********************************************************************************************************************/

enum drawing_style merge_styles (enum drawing_style first_style, enum drawing_style second_style) {
    if (first_style == HIGHLIGHTED || second_style == HIGHLIGHTED)
        return HIGHLIGHTED;
    else if (first_style == ACTIVE || second_style == ACTIVE)
        return ACTIVE;
    else if (first_style == STANDART || second_style == STANDART)
        return STANDART;
    else if (first_style == INACTIVE || second_style == INACTIVE)
        return INACTIVE;
    else
        return HIDDEN;
}