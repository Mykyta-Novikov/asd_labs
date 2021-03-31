#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


const int NODES_COUNT = 11;

const char* PROGRAM_NAME = "A";
const char* WINDOW_NAME = "B";
const unsigned int MATRIX_SEED = 123456;

const int WINDOW_HEIGHT = 200;
const int WINDOW_WIDTH = 200;

const int DRAWING_MARGIN = 10;
const int NODE_RADIUS = 20;
const int NODE_MARGIN = 40;
const int EDGE_MARGIN = 10;
const int NODE_TEXT_HEIGHT_BOUNDARY = 10;
const int NODE_TEXT_WIDTH_BOUNDARY = 10;


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

void draw_node (HDC hdc, struct node *node);
void render_edge (HDC hdc, int start_index, int end_index, struct graph* graph);

HPEN get_nodes_pen ();
HPEN get_edges_pen ();