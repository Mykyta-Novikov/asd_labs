#pragma once
#include "math.h"

void position_nodes (struct graph* graph, int height, int width);

/**********************************************************************************************************************/
/*
 * Creates and fills graph structure.
 */

struct graph* initialise_graph () {
    struct graph* graph = malloc(sizeof(struct graph));
    if (graph == NULL) exit(2);

    int n = graph->n = NODES_COUNT;

    unsigned int* matrix = graph->matrix = malloc(n * n * sizeof(unsigned int));
    if (matrix == NULL) exit(2);

    srand(MATRIX_SEED); // NOLINT(cert-msc51-cpp)
    for (int i = 0; i < n; i++) {                // initialise edges structure with random values
        for (int j = 0; j < n; j++) {
            double value = (double)rand() * 2 / RAND_MAX;
            value *= 1 - 0 * 0.2 - 7 * 0.005 - 0.25;
            graph_set(graph, i, j, (unsigned  int)value);

            printf("%d ", graph_get(graph, i, j));
        }
        puts("");
    }

    if (!directed_graph)
        for (int i = 0; i < graph->n; i++)
            for (int j = 0; j < n; j++)
                if (graph_get(graph, i, j))
                    graph_set(graph, j, i, 1);


    struct node* nodes = graph->nodes = malloc(graph->n * sizeof(struct node));
    if (nodes == NULL) exit(2);

    for (int i = 0; i < graph->n; i++) {   // set nodes positions to default
        nodes[i].coordinates.x = -1;
        nodes[i].coordinates.y = -1;

        nodes[i].n = i;
    }

    return graph;
}

/**********************************************************************************************************************/
/*
 * Determines positions of nodes if needed, and paints graph.
 */

void render_graph (HDC hdc, struct graph* graph, int height, int width) {
    position_nodes(graph, height, width);
    for (int i = 0; i < graph->n; i++) {
        if (graph->nodes[i].coordinates.x == -1 || graph->nodes[i].coordinates.y == -1) {
            position_nodes(graph, height, width);
            break;
        }
    }


    for (int i = 0; i < graph->n; i++) {
        draw_node(hdc, &graph->nodes[i]);

        for (int j = 0; j <= i; j++) {
            if (graph_get(graph, i, j) == 1 || graph_get(graph, j, i) == 1) {
                if (i == j)
                    draw_loop(hdc, &graph->nodes[i], graph);
                else
                    render_edge(hdc, i, j, graph);
            }
        }
    }
}

/**********************************************************************************************************************/
/*
 * Determines coordinates of nodes.
 */

void position_nodes (struct graph* graph, int height, int width) {
    graph->nodes[0].coordinates.x = width / 2;           // first node is positioned in center
    graph->nodes[0].coordinates.y = height / 2;

    int radius = min(height, width) / 2 - NODE_RADIUS - DRAWING_MARGIN;   // the rest are positioned in circle
    double angle = 0;
    for (int i = 1; i < graph->n; i++) {
        graph->nodes[i].coordinates.x = width / 2 + (int)(radius * cos(angle));    // using parametric form
        graph->nodes[i].coordinates.y = height / 2 - (int)(radius * sin(angle));   // of equation

        angle += 2 * M_PI / (graph->n - 1);
    }
}