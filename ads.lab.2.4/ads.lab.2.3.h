#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


const char* PROGRAM_NAME = "Lab 2.4";
const char* WINDOW_NAME = "Lab 2.4";
const unsigned int MATRIX_SEED = 417;
const int NODES_COUNT = 11;

const int WINDOW_HEIGHT = 200;
const int WINDOW_WIDTH = 200;

const int DRAWING_MARGIN = 30;
const int NODE_RADIUS = 30;
const int NODE_MARGIN = 40;
const int EDGE_MARGIN = 20;
const int ARROW_LENGTH = 15;
const int ARROW_WIDTH = 5;
const int NODE_TEXT_HEIGHT_BOUNDARY = 10;
const int NODE_TEXT_WIDTH_BOUNDARY = 10;
const int LOOP_CENTER_DISTANCE = 20;
const int LOOP_RADIUS = 25;
const int CENTRAL_LOOP_ANGLE = 135;


struct coord {
    int x;
    int y;
};

struct coordd {
    double x;
    double y;
};


struct node {
    int n;
    struct coord coordinates;
};

struct graph {
   int n;
   unsigned int* matrix;
   struct node* nodes;
};

struct edge_shift {
    struct coord start;
    struct coord end;
    int shift;
    int radius;
};


struct graph* initialise_graph ();
void render_graph (HDC hdc, struct graph* graph, int height, int width);

unsigned int graph_get (struct graph* graph, int i, int j);
void graph_set (struct graph* graph, int i, int j, unsigned int value);
void matrix_multiply (unsigned int* matrix, int size, unsigned int multiplier);
void matrix_symmetry (unsigned int* matrix, int size);
void print_matrix (unsigned int* matrix, int size);
struct coordd normalize_vector (struct coordd vector);

void draw_node (HDC hdc, struct node *node);
void render_edge (HDC hdc, int start_index, int end_index, struct graph* graph);
void draw_loop (HDC hdc, struct node* node, struct graph* graph);

HPEN get_nodes_pen ();
HPEN get_edges_pen ();


boolean directed_graph;
boolean use_modified_graph;
boolean use_condensation_graph;