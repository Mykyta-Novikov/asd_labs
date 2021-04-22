#pragma once
#include "math.h"

void position_nodes (struct graph* graph, int height, int width);
void print_degrees (struct graph *graph);
void print_paths(const unsigned int* matrix, int size);
void get_reachability_matrix (unsigned int* matrix, int size, unsigned int* result);
void get_components (const unsigned int* reachability_matrix, int size, unsigned int* result, int* result_count);
void print_components (const unsigned int* components, int size, int components_count);
void get_condensation_matrix (const unsigned int* matrix, int size, const unsigned int* components,
                              int components_count, unsigned int* result);

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
            value *= 1 - 2 * 0.01 - 9 * 0.01 - 0.3;
            graph_set(graph, i, j, (unsigned  int)value);
        }
    }

    //matrix_multiply(matrix, n, (unsigned int)(abs((int)(2 * (1 - 2 * 0.01 - 9 * 0.01 - 0.4)))));
    if (!directed_graph)
        matrix_symmetry(matrix, n);


    struct node* nodes = graph->nodes = malloc(n * sizeof(struct node));
    if (nodes == NULL) exit(2);

    puts("Matrix:");
    print_matrix(matrix, graph->n);

    print_degrees(graph);


    unsigned int* modified_matrix = malloc(n * n * sizeof(unsigned int));
    if (modified_matrix == NULL) exit(2);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            modified_matrix[i * n + j] = graph_get(graph, i, j);

    matrix_multiply(modified_matrix, n, 1);

    puts("\nModified matrix:");
    print_matrix(modified_matrix, n);

    if (use_modified_graph) {
        free(graph->matrix);
        graph->matrix = modified_matrix;
    }


    print_paths(modified_matrix, n);

    unsigned int* reachability_matrix = malloc(n * n * sizeof(unsigned int));
    if (reachability_matrix == NULL) exit(2);

    get_reachability_matrix(modified_matrix, n, reachability_matrix);
    puts("\nReachability matrix:");
    print_matrix(reachability_matrix, n);


    unsigned int* components = malloc(n * n * sizeof(unsigned int));
    if (components == NULL) exit(2);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            components[i * n + j] = 0;
    int component_count = 0;

    get_components(reachability_matrix, n, components, &component_count);

    puts("\nComponents:");
    print_components(components, n, component_count);

    unsigned int* condensation_matrix = malloc(n * n * sizeof(unsigned int));
    if (condensation_matrix == NULL) exit(2);

    get_condensation_matrix(modified_matrix, n, components, component_count, condensation_matrix);
    if (use_condensation_graph) {
        graph->n = component_count;

        free(graph->nodes);
        graph->nodes = malloc(component_count * sizeof(struct node));
        if (nodes == NULL) exit(2);

        free(graph->matrix);
        graph->matrix = condensation_matrix;
    }
    puts("\nCondensation matrix:");
    print_matrix(condensation_matrix, component_count);


    for (int i = 0; i < graph->n; i++) {   // set nodes positions to default
        graph->nodes[i].coordinates.x = -1;
        graph->nodes[i].coordinates.y = -1;

        graph->nodes[i].n = i;
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

/**********************************************************************************************************************/
/*
 * Determines and prints degrees of nodes and finds the isolated and leaf nodes. Also checks if the graph is regular.
 */

void print_degrees (struct graph *graph) {
    unsigned int indegrees[graph->n], outdegrees[graph->n];
    boolean is_regular = 1;
    unsigned int regular_indegree, regular_outdegree;
    int isolated_edges[graph->n], isolated_count = 0,
        leaf_edges[graph->n], leaf_count = 0;

    for (int i = 0; i < graph->n; i++) {
        indegrees[i] = 0;
        outdegrees[i] = 0;
        for (int j = 0; j < graph->n; j++) {
            indegrees[i] += graph_get(graph, i, j);
            outdegrees[i] += graph_get(graph, j, i);
        }

        if (i == 0) {
            regular_indegree = indegrees[0];
            regular_outdegree = outdegrees[0];
        } else if (regular_indegree != indegrees[i] || regular_outdegree != outdegrees[i])
            is_regular = 0;

        if (indegrees[i] + outdegrees[i] == 0)
            isolated_edges[isolated_count++] = i;
        else if (indegrees[i] + outdegrees[i] == directed_graph ? 1 : 0)
            leaf_edges[leaf_count++] = i;
    }


    puts("\nDegrees:");
    if (directed_graph) {
        for (int i = 0; i < graph->n; i++) {
            printf("%d ", indegrees[i]);
        }
        puts("");
        for (int i = 0; i < graph->n; i++) {
            printf("%d ", outdegrees[i]);
        }
        puts("");
    } else {
        for (int i = 0; i < graph->n; i++) {
            printf("%d ", indegrees[i] + outdegrees[i]);
        }
        puts("");
    }
    if (is_regular) {
        if (!directed_graph)
            printf("The graph is regular with degree: %d", regular_indegree + regular_outdegree);
        else
            printf("The graph is regular with degrees: %d %d", regular_indegree, regular_outdegree);
    }

    if (isolated_count > 0) {
        puts("\nIsolated nodes:");
        for (int i = 0; i < isolated_count; i++)
            printf("%d ", isolated_edges[i]);
        puts("");
    }
    if (leaf_count > 0) {
        puts("\nLeaf nodes:");
        for (int i = 0; i < leaf_count; i++)
            printf("%d ", leaf_edges[i]);
        puts("");
    }
}

/**********************************************************************************************************************/

void print_paths(const unsigned int* matrix, int size) {
    unsigned int pow2[size][size];
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            pow2[i][j] = 0;
            for (int k = 0; k < size; k++)
                pow2[i][j] += matrix[k * size + j] * matrix[i * size + k];
        }

    puts("\n2-paths:");
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (i == j) continue;
            if (!pow2[i][j]) continue;

            printf("%d ", i);
            for (int k = 0; k < size; k++)
                if (matrix[i * size + k] && matrix[k * size + j]) {
                    printf("%d ", k);
                    break;
                }
            printf("%d\n", j);
        }


    unsigned int pow3[size][size];
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            pow3[i][j] = 0;
            for (int k = 0; k < size; k++)
                pow3[i][j] += pow2[k][j] * matrix[i * size + k];
        }

    puts("\n3-paths:");
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (i == j) continue;
            if (!pow3[i][j])continue;

            printf("%d ", i);
            for (int k = 0; k < size; k++)
                if (pow2[i][k] && matrix[k * size + j]) {
                    printf("%d ", k);
                    break;
                }
            for (int k = 0; k < size; k++)
                if (matrix[i * size + k] && pow2[k][j]) {
                    printf("%d ", k);
                    break;
                }
            printf("%d\n", j);
        }
}

/**********************************************************************************************************************/

void get_reachability_matrix (unsigned int* matrix, int size, unsigned int* result) {
    unsigned int pows[size - 1][size][size];
    for (int pow = 1; pow < size; pow++) {
        unsigned int* prev, * curr;
        if (pow == 1) {
            prev = matrix;
            curr = (unsigned int *) pows[0];
        }
        else {
            prev = (unsigned int *) pows[pow - 2];
            curr = (unsigned int *) pows[pow - 1];
        }

        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++) {
                curr[i * size + j] = 0;
                for (int k = 0; k < size; k++)
                    curr[i * size + j] += prev[k * size + j] * matrix[i * size + k];
            }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            unsigned int value = matrix[i * size + j];
            for (int k = 0; k < size - 1; k++) {
                value += pows[k][i][j];
            }
            result[i * size + j] = value ? 1 : 0;
        }
    }

    for (int i = 0; i < size; i++)      // set all cells on main diagonal to 1
        result[i * size + i] = 1;
}

/**********************************************************************************************************************/

void get_components (const unsigned int* reachability_matrix, int size, unsigned int* result, int* result_count) {
    unsigned int components_matrix[size][size];
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            components_matrix[i][j] = reachability_matrix[i * size + j] * reachability_matrix[j * size + i] ? 1 : 0;

    boolean is_included[size];
    for (int i = 0; i < size; i++)
        is_included[i] = 0;

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            result[i * size + j] = 0;

    for (int i = 0; i < size; i++) {
        if (is_included[i]) continue;
        is_included[i] = 1;
        result[*result_count * size + i] = 1;

        for (int j = 0; j < size; j++) {
            if (is_included[j]) continue;
            if (components_matrix[i][j]) {
                is_included[j] = 1;
                result[*result_count * size + j] = 1;
            }
        }
        (*result_count)++;
    }
}

/**********************************************************************************************************************/

void print_components (const unsigned int* components, int size, int components_count) {
    for (int i = 0; i < components_count; i++) {
        printf("%d:  ", i);
        for (int j = 0; j < size; j++)
            if (components[i * size + j])
                printf("%d ", j);
        puts("");
    }
}

/**********************************************************************************************************************/

void get_condensation_matrix (const unsigned int* matrix, int size, const unsigned int* components,
                              int components_count, unsigned int* result) {

    for (int i = 0; i < components_count; i++) {
        for (int j = 0; j < components_count; j++) {
            if (i == j) {
                result[i * components_count + j] = 0;
                continue;
            }
            boolean has_connection = 0;

            for (int k = 0; k < size; k++) {
                if (!components[i * size + k]) continue;
                for (int l = 0; l < size; l++) {
                    if (!components[j * size + l]) continue;

                    if (matrix[k * size + l]) {
                        has_connection = 1;
                        goto main_loop;
                    }
                }
            }
            main_loop:
            result[i * components_count + j] = has_connection;
        }
    }
}