#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


const char* PROGRAM_NAME = "lab5";
const char* WINDOW_NAME = "Lab 5";
const unsigned int MATRIX_SEED = 417;
const int NODES_COUNT = 11;

const int WINDOW_HEIGHT = 200;
const int WINDOW_WIDTH = 200;

const int DRAWING_MARGIN = 30;
const int NODE_RADIUS = 30;
const int NODE_MARGIN = 40;
const int EDGE_MARGIN = 20;
const int NODE_TEXT_HEIGHT_BOUNDARY = 10;
const int NODE_TEXT_WIDTH_BOUNDARY = 10;
const int WEIGHT_TEXT_HEIGHT_BOUNDARY = 10;
const int WEIGHT_TEXT_WIDTH_BOUNDARY = 10;
const double WEIGHT_POSITION_PERCENT = 0.80;


struct coord {
    union {
        double d;
        int i;
    } x;

    union {
        double d;
        int i;
    } y;
};


struct search_step {
    int count;
    int last;
    int nodes_indexes[];
};

struct search_sequence {
    int count;
    struct search_step* steps[];
};


struct node {
    int n;
    struct coord coordinates;
};

struct graph {
   int n;
   unsigned int* matrix;
   unsigned int* weights_matrix;
   struct node* nodes;
   struct search_sequence* sequence;
};

struct edge_shift {
    struct coord start;
    struct coord end;
    int shift;
    int radius;
};


enum ui_state {
    INITIAL,
    MAIN,
    SPANNING_TREE
};

enum drawing_style {
    STANDART,
    ACTIVE,
    INACTIVE,
    HIGHLIGHTED,
    HIDDEN
};

typedef struct list* deque;


struct graph* initialise_graph ();
void render_graph (HDC hdc, struct graph* graph, int height, int width, int sequence_step, enum ui_state screen);

unsigned int graph_get (struct graph* graph, int i, int j);
void graph_set (struct graph* graph, int i, int j, unsigned int value);
struct coord normalize_vector (struct coord vector);
struct search_step* search_step_create (int size, struct search_step* old_step);
void search_step_add (struct search_step* search_step, int value);

deque deque_create ();
void deque_delete (deque deque);
struct search_step* deque_poll (deque deque, boolean as_queue);
void deque_put (deque deque, struct search_step* value);
boolean deque_is_empty (deque deque);

void draw_node (HDC hdc, struct node* node, enum drawing_style style);
void render_edge (HDC hdc, int start_index, int end_index, struct graph* graph, unsigned int weight,
                  enum drawing_style forward_style, enum drawing_style backward_style);

HPEN get_nodes_pen (enum drawing_style style);
HPEN get_edges_pen (enum drawing_style style);

enum drawing_style get_node_style (int index, struct search_step* search_step, enum ui_state screen);
enum drawing_style get_edge_style (int start, int end, int size, struct search_step* search_step,
        enum ui_state screen, const boolean* spanning_tree_drawing_matrix);
enum drawing_style merge_styles (enum drawing_style first_style, enum drawing_style second_style);


boolean use_dfs = 1;