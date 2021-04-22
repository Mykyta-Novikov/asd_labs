#pragma once


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
 * Arithmetic operations with matrices;
 */

void matrix_multiply (unsigned int* matrix, int size, unsigned int multiplier) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i * size + j] *= multiplier;
}

void matrix_symmetry (unsigned int* matrix, int size) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (matrix[i * size + j])
                matrix[i * size + j] = 1;
}

/**********************************************************************************************************************/

void print_matrix (unsigned int* matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", matrix[i * size + j]);
        }
        puts("");
    }
}

/**********************************************************************************************************************/
/*
 * Singleton getters of pens.
 */

HPEN nodes_pen;
HPEN get_nodes_pen () {
    if (nodes_pen == NULL)
        nodes_pen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
    return nodes_pen;
}

HPEN edges_pen;
HPEN get_edges_pen() {
    if (edges_pen == NULL)
        edges_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    return edges_pen;
}

/**********************************************************************************************************************/
/*
 * Function for normalizing a vector (dividing by its length).
 */

struct coordd normalize_vector (struct coordd vector) {
    double length = sqrt(vector.x * vector.x + vector.y * vector.y);
    struct coordd result = {
            .x = vector.x / length,
            .y = vector.y / length
    };
    return result;
}