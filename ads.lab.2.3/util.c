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

struct coordd normalize_vector (struct coordd vector) {
    double length = sqrt(vector.x * vector.x + vector.y * vector.y);
    struct coordd result = {
            .x = vector.x / length,
            .y = vector.y / length
    };
    return result;
}