#pragma once
#include "math.h"

void position_nodes (struct graph* graph, int height, int width);

void conduct_search (struct graph* graph, struct search_sequence* result);

unsigned int* get_weights_matrix (struct graph* graph,  int result_size, unsigned int* result);


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
            value *= 1.0 - 0 * 0.01 - 7 * 0.005 - 0.05;
            graph_set(graph, i, j, (unsigned  int)value);
        }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (graph_get(graph, i, j))
                graph_set(graph, j, i, 1);
    for (int i = 0; i < n; i++)
        graph_set(graph, i, i, 0);


    unsigned int* weights_matrix = graph->weights_matrix = malloc(n * n * sizeof(unsigned int));
    if (weights_matrix == NULL) exit(2);
    get_weights_matrix(graph, n, weights_matrix);

    struct node* nodes = graph->nodes = malloc(graph->n * sizeof(struct node));
    if (nodes == NULL) exit(2);

    for (int i = 0; i < graph->n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", graph_get(graph, i, j));
        }
        puts("");
    }

    for (int i = 0; i < graph->n; i++) {   // set nodes positions to default
        nodes[i].coordinates.x.i = -1;
        nodes[i].coordinates.y.i = -1;

        nodes[i].n = i;
    }

    struct search_sequence* search = graph->sequence
            = malloc(sizeof(struct search_sequence) + n * sizeof(void*));
    if (search == NULL) exit(2);

    conduct_search(graph, search);

    return graph;
}

/**********************************************************************************************************************/

unsigned int* get_weights_matrix (struct graph* graph, int result_size, unsigned int* result) {
    for (int i = 0; i < result_size; i++) {                // initialise edges structure with random values
        for (int j = 0; j < result_size; j++) {
            double value = (double)rand() * 100 / RAND_MAX;

            unsigned int b1 = !graph_get(graph, i, j);
            unsigned int b2 = !graph_get(graph, j, i);

            unsigned int c = b1 == b2;
            unsigned int d = b1 && b2;

            value *= c  +  d * (j > i);
            result[i * result_size + j] = (unsigned int)value;
        }
    }

    for (int i = 0; i < result_size; i++)
        for (int j = 0; j < i; j++) {
            unsigned int value = max(result[i * result_size + j], result[j * result_size + i]);
            result[i * result_size + j] = value;
            result[j * result_size + i] = value;
        }
    for (int i = 0; i < result_size; i++)
        result[i * result_size + i] = 0;

    return result;
}

/**********************************************************************************************************************/
/*
 * Conducts search and returns the sequence.
 */

void conduct_search (struct graph* graph, struct search_sequence* result) {
    result->count = 0;
    const int n = graph->n;
    unsigned int* weights_matrix = graph->weights_matrix;

    boolean is_visited[graph->n];
    for (int i = 0; i < graph->n; i++)
        is_visited[i] = 0;

    struct search_step* last_step;


    {
        int min_i = -1, min_j = -1;
        unsigned int min_value = UINT_MAX;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < i; j++)
                if (graph_get(graph, i, j) && weights_matrix[i * n + j] <= min_value) {
                    min_i = i;
                    min_j = j;
                    min_value = weights_matrix[i * n + j];
                }
                struct search_step* first_step = last_step =
                        result->steps[result->count] = search_step_create(n, NULL);
                search_step_add(first_step, min_i);
                search_step_add(first_step, min_j);
                result->count++;

                is_visited[min_i] = 1;
                is_visited[min_j] = 1;
    }


    while (1) {
        int min_i = -1, min_j = -1;
        unsigned int min_value = UINT_MAX;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < i; j++)
                if (
                        is_visited[i] ^ is_visited[j] &&
                        graph_get(graph, i, j) &&
                        weights_matrix[i * n + j] <= min_value
                    ) {
                    min_i = i;
                    min_j = j;
                    min_value = weights_matrix[i * n + j];
                }
        if (min_i == -1 || min_j == -1) break;

        struct search_step* step = last_step = result->steps[result->count] = search_step_create(n, last_step);
        search_step_add(step, min_i);
        search_step_add(step, min_j);
        result->count++;

        is_visited[min_i] = 1;
        is_visited[min_j] = 1;
    }

}

/**********************************************************************************************************************/
/*
 * Determines positions of nodes if needed, and paints graph.
 */

void render_graph (HDC hdc, struct graph* graph, int height, int width, int sequence_step, enum ui_state screen) {
    position_nodes(graph, height, width);
    for (int i = 0; i < graph->n; i++) {
        if (graph->nodes[i].coordinates.x.i == -1 || graph->nodes[i].coordinates.y.i == -1) {
            position_nodes(graph, height, width);
            break;
        }
    }

    boolean spanning_tree_drawing_matrix[graph->n][graph->n];
    for (int i = 0; i < graph->n; i++)
        for (int j = 0; j < graph->n; j++) {
            spanning_tree_drawing_matrix[i][j] = 0;
        }
    for (int i = 0; i < graph->sequence->count; i++) {
        int* nodes_indexes = graph->sequence->steps[i]->nodes_indexes;
        for (int j = 1; j < graph->sequence->steps[i]->count; j++) {
            spanning_tree_drawing_matrix[nodes_indexes[j - 1]][nodes_indexes[j]] = 1;
        }
    }

    for (int i = 0; i < graph->n; i++) {
        draw_node(hdc, &graph->nodes[i], get_node_style(i, graph->sequence->steps[sequence_step], screen));

        for (int j = 0; j <= i; j++)
            if (graph_get(graph, i, j) == 1 || graph_get(graph, j, i) == 1) {

                enum drawing_style forward_style = get_edge_style(i, j, graph->n,
                        graph->sequence->steps[sequence_step], screen, (boolean*) &spanning_tree_drawing_matrix);
                enum drawing_style backward_style = get_edge_style(j, i, graph->n,
                        graph->sequence->steps[sequence_step], screen,(boolean*) &spanning_tree_drawing_matrix);

                if (i != j)
                    render_edge(hdc, i, j, graph, graph->weights_matrix[i * graph->n + j],
                                forward_style, backward_style);
            }
    }
}

/**********************************************************************************************************************/
/*
 * Determines coordinates of nodes.
 */

void position_nodes (struct graph* graph, int height, int width) {
    graph->nodes[0].coordinates.x.i = width / 2;           // first node is positioned in center
    graph->nodes[0].coordinates.y.i = height / 2;

    int radius = min(height, width) / 2 - NODE_RADIUS - DRAWING_MARGIN;   // the rest are positioned in circle
    double angle = 0;
    for (int i = 1; i < graph->n; i++) {
        graph->nodes[i].coordinates.x.i = width / 2 + (int)(radius * cos(angle));    // using parametric form
        graph->nodes[i].coordinates.y.i = height / 2 - (int)(radius * sin(angle));   // of equation

        angle += 2 * M_PI / (graph->n - 1);
    }
}