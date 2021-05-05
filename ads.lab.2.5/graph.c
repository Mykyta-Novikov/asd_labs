#pragma once
#include "math.h"

void position_nodes (struct graph* graph, int height, int width);

void conduct_search (struct graph* graph, struct search_sequence* result);


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
            value *= 1.0 - 0 * 0.01 - 7 * 0.005 - 0.15;
            graph_set(graph, i, j, (unsigned  int)value);
        }
    }

    if (!directed_graph)
        for (int i = 0; i < graph->n; i++)
            for (int j = 0; j < n; j++)
                if (graph_get(graph, i, j))
                    graph_set(graph, j, i, 1);


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
/*
 * Conducts search and returns the sequence.
 */

void conduct_search (struct graph* graph, struct search_sequence* result) {
    result->count = 0;

    boolean is_visited[graph->n];
    for (int i = 0; i < graph->n; i++)
        is_visited[i] = 0;
    deque deque = deque_create();

    for (int i = 0; i < graph->n; i++) {
        if (is_visited[i]) continue;

        struct search_step* current = search_step_create(graph->n, NULL);
        search_step_add(current, i);
        deque_put(deque, current);
        do {
            struct search_step* current_step = deque_poll(deque, !use_dfs);

            if (is_visited[current_step->last]) {
                free(current_step);
                continue;
            }
            is_visited[current_step->last] = 1;

            result->steps[result->count] = current_step;
            result->count++;

            for (int j = 0; j < graph->n; j++)
                if (!is_visited[j] && graph_get(graph, current_step->last, j)) {
                    struct search_step* next_step = search_step_create(graph->n, current_step);
                    search_step_add(next_step, j);
                    deque_put(deque, next_step);
                }
        } while (!deque_is_empty(deque));
    }
    deque_delete(deque);
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
                        graph->sequence->steps[sequence_step], screen, (boolean*) &spanning_tree_drawing_matrix);
                if (!directed_graph)
                    forward_style = backward_style = merge_styles(forward_style, backward_style);

                if (i == j)
                    draw_loop(hdc, &graph->nodes[i], graph, forward_style);
                else
                    render_edge(hdc, i, j, graph, forward_style, backward_style);
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