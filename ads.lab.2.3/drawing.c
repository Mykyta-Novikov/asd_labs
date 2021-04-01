#pragma once

boolean get_edge_shift (struct coordd* vector, struct coord* start,
                        double max_length,  struct coord* node, struct edge_shift* result);

void draw_edge (HDC hdc, struct coord* start, struct coord* end, struct coordd* vector, struct edge_shift shifts[],
                int shifts_count, unsigned int forth, unsigned int back, unsigned int additional_margin);

void draw_edge_segment (HDC hdc, struct edge_shift* shift, struct coordd* vector, int current_edge_margin);

void draw_arrow (HDC hdc, struct coordd *vector, struct coord *arrow_end);

/**********************************************************************************************************************/

void draw_node (HDC hdc, struct node *node) {
    SelectObject(hdc, get_nodes_pen());
    MoveToEx(hdc, node->coordinates.x + NODE_RADIUS, node->coordinates.y, NULL);

    AngleArc(hdc, node->coordinates.x, node->coordinates.y, NODE_RADIUS, 0, 360);

    char text[snprintf(NULL, 0, "%d", node->n)];
    sprintf(text, "%d", node->n);                                       // convert string to text

    RECT text_boundaries = {
            .right = node->coordinates.x + NODE_TEXT_WIDTH_BOUNDARY,
            .left = node->coordinates.x - NODE_TEXT_WIDTH_BOUNDARY,
            .bottom = node->coordinates.y + NODE_TEXT_HEIGHT_BOUNDARY,
            .top = node->coordinates.y - NODE_TEXT_HEIGHT_BOUNDARY
    };
    DrawText(hdc, text, -1, &text_boundaries,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);
}

/**********************************************************************************************************************/
/*
 * Prepares data for driving of an edge between 2 nodes.
 */

void render_edge (HDC hdc, int start_index, int end_index, struct graph* graph) {
    struct coord start = graph->nodes[start_index].coordinates,
            end = graph->nodes[end_index].coordinates;
    struct coordd vector = {
          .x = end.x - start.x,
          .y = end.y - start.y
    };
    double length = sqrt(vector.x * vector.x + vector.y * vector.y);
    vector = normalize_vector(vector);

    struct coord start_point = {
            .x = (int) (start.x + vector.x * NODE_RADIUS),
            .y = (int) (start.y + vector.y * NODE_RADIUS)
    };
    struct coord end_point = {
            .x = (int) (end.x - vector.x * NODE_RADIUS),
            .y = (int) (end.y - vector.y * NODE_RADIUS)
    };


    struct edge_shift shifts[graph->n - 2];
    int shifts_count = 0;
    for (int i = 0; i < graph->n; i++) {
        if (i == start_index || i == end_index)
            continue;

        struct edge_shift shift;
        boolean require_shift = get_edge_shift(&vector, &start, length, &graph->nodes[i].coordinates, &shift);

        if (require_shift) {
            shifts[shifts_count] = shift;
            shifts_count++;
        }
    }


    unsigned int additional_margin = 0;   // determine whether there are more than 2 nodes in line
    if (start_index - end_index == NODES_COUNT / 2 && end_index != 0)           // if nodes are opposite
        additional_margin = max(graph_get(graph, 0, start_index) + graph_get(graph, start_index, 0),
                                graph_get(graph, 0, end_index) + graph_get(graph, end_index, 0));

    draw_edge(hdc, &start_point, &end_point, &vector, shifts, shifts_count,
              directed_graph ? graph_get(graph, start_index, end_index) : 0,
              graph_get(graph, end_index, start_index), additional_margin);
}

/**********************************************************************************************************************/
/*
 * Draws parallel edges between 2 nodes.
 */
void draw_edge (HDC hdc, struct coord* start, struct coord* end, struct coordd* vector, struct edge_shift shifts[],
        int shifts_count, unsigned int forth, unsigned int back, unsigned int additional_margin) {
    SelectObject(hdc, get_edges_pen());

    for (int edge_number = (int)additional_margin; edge_number < forth + back + additional_margin; edge_number++) {
        int current_edge_margin = (int)(EDGE_MARGIN * (edge_number - (forth + back + additional_margin - 1) / 2.0));

        struct coord current_edge_start = {
                .x = (int) (start->x + vector->x * NODE_MARGIN + vector->y * current_edge_margin),
                .y = (int) (start->y + vector->y * NODE_MARGIN - vector->x * current_edge_margin)
        };
        struct coord current_edge_end = {
                .x = (int) (end->x - vector->x * NODE_MARGIN + vector->y * current_edge_margin),
                .y = (int) (end->y - vector->y * NODE_MARGIN - vector->x * current_edge_margin)
        };


        MoveToEx(hdc, start->x, start->y, NULL);
        LineTo(hdc, current_edge_start.x, current_edge_start.y);

        if (shifts_count == 1) {                // if there is only 1 shift segment, draw entire edge as it
            shifts[0].start.x = (int) (start->x + vector->x * NODE_MARGIN);
            shifts[0].start.y = (int) (start->y + vector->y * NODE_MARGIN);

            shifts[0].end.x = (int) (end->x - vector->x * NODE_MARGIN);
            shifts[0].end.y = (int) (end->y - vector->y * NODE_MARGIN);

            draw_edge_segment(hdc, &shifts[0], vector, current_edge_margin);
        }
        else
            for (int i = 0; i < shifts_count; i++)
                draw_edge_segment (hdc, &shifts[i], vector, current_edge_margin);

        LineTo(hdc, current_edge_end.x, current_edge_end.y);
        LineTo(hdc, end->x, end->y);


        if (edge_number < forth) {
            struct coordd arrow_vector = {
                    .x = end->x - current_edge_end.x,
                    .y = end->y - current_edge_end.y
            };
            arrow_vector = normalize_vector(arrow_vector);

            draw_arrow(hdc, &arrow_vector, end);
        }
        else {
            struct coordd arrow_vector = {
                    .x = start->x - current_edge_start.x,
                    .y = start->y - current_edge_start.y
            };
            arrow_vector = normalize_vector(arrow_vector);

            draw_arrow(hdc, &arrow_vector, start);
        }
    }
}

/**********************************************************************************************************************/
/*
 * Draws edge segment corresponding to particular shift.
 */

void draw_edge_segment (HDC hdc, struct edge_shift* shift, struct coordd* vector, int current_edge_margin) {
    LineTo(hdc, (int) (shift->start.x + vector->y * current_edge_margin),
           (int) (shift->start.y - vector->x * current_edge_margin));

    POINT points[3] = {
            {
                    .x = shift->start.x + shift->shift * vector->y + vector->y * current_edge_margin,
                    .y = shift->start.y - shift->shift * vector->x - vector->x * current_edge_margin
            },
            {
                    .x = shift->end.x + shift->shift * vector->y + vector->y * current_edge_margin,
                    .y = shift->end.y - shift->shift * vector->x - vector->x * current_edge_margin
            },
            {
                    .x = shift->end.x + vector->y * current_edge_margin,
                    .y = shift->end.y - vector->x * current_edge_margin
            }
    };
    PolyBezierTo(hdc, points, 3);
    SelectObject(hdc, get_edges_pen());
}

/**********************************************************************************************************************/
/*
 * Draws an arrow directed by vector and with the end in arrow_end point.
 */

void draw_arrow (HDC hdc, struct coordd* vector, struct coord* arrow_end) {
    if (!directed_graph)    // do not draw arrow if graph is not directed
        return;

    SelectObject(hdc, get_edges_pen());
    MoveToEx(hdc, arrow_end->x, arrow_end->y, NULL);

    struct coord arrow_start = {
            .x = (int)(arrow_end->x - vector->x * ARROW_LENGTH),
            .y = (int)(arrow_end->y - vector->y * ARROW_LENGTH)
    };

    LineTo(hdc, (int)(arrow_start.x - vector->y * ARROW_WIDTH), (int)(arrow_start.y + vector->x * ARROW_WIDTH));
    LineTo(hdc, (int)(arrow_start.x + vector->y * ARROW_WIDTH), (int)(arrow_start.y - vector->x * ARROW_WIDTH));
    LineTo(hdc, arrow_end->x, arrow_end->y);

}

/**********************************************************************************************************************/
/*
 * Determines shift of the edge between 2 nodes around third.
 */

boolean get_edge_shift (struct coordd* vector, struct coord* start,
                        double max_length, struct coord* node, struct edge_shift* result) {
    double c = -vector->y * start->x + vector->x * start->y;

    int center_distance = -(int)(
            (vector->y * node->x - vector->x * node->y + c) /
            sqrt(vector->x * vector->x + vector->y * vector->y)
    );
    if (abs(center_distance) >= NODE_RADIUS + NODE_MARGIN)
        return 0;

    result->shift = (center_distance >= 0 ? 1 : -1) * (NODE_RADIUS + NODE_MARGIN)  -  center_distance;


    struct coord intersection = {
            .x = (int)(node->x + vector->y * center_distance),
            .y = (int)(node->y - vector->x * center_distance)
    };

    double length = (intersection.x - start->x) / vector->x;
    if (length <= 0 || length >= max_length)
        return 0;

    double circle_distance = sqrt((NODE_RADIUS + NODE_MARGIN) * (NODE_RADIUS + NODE_MARGIN)
                                  - center_distance * center_distance);
    result->radius = (int)circle_distance;

    result->start.x = (int)(intersection.x - circle_distance * vector->x);
    result->start.y = (int)(intersection.y - circle_distance * vector->y);

    result->end.x = (int)(intersection.x + circle_distance * vector->x);
    result->end.y = (int)(intersection.y + circle_distance * vector->y);

    return 1;
}

/**********************************************************************************************************************/
/*
 * Draws loop edge as an arc of circle.
 */

void draw_loop (HDC hdc, struct node* node, struct graph* graph) {
    struct coordd loop_vector;
    if (node->n == 0) {
        loop_vector.x = cos(CENTRAL_LOOP_ANGLE);
        loop_vector.y = sin(CENTRAL_LOOP_ANGLE);
    }
    else {
        struct coordd raw_loop_vector = {
                .x = node->coordinates.x - graph->nodes[0].coordinates.x,
                .y = node->coordinates.y - graph->nodes[0].coordinates.y
        };

        loop_vector = normalize_vector (raw_loop_vector);
    }

    struct coordd loop_center = {
            .x = node->coordinates.x + loop_vector.x * (NODE_RADIUS + LOOP_CENTER_DISTANCE),
            .y = node->coordinates.y + loop_vector.y * (NODE_RADIUS + LOOP_CENTER_DISTANCE)
    };

    double loop_segment = sqrt(LOOP_RADIUS * LOOP_RADIUS - LOOP_CENTER_DISTANCE * LOOP_CENTER_DISTANCE);
    struct coord loop_start = {
            .x = (int)(node->coordinates.x + loop_vector.x * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    - loop_vector.y * loop_segment),
            .y = (int)(node->coordinates.y + loop_vector.y * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    + loop_vector.x * loop_segment)
    },
    loop_end = {
            .x = (int)(node->coordinates.x + loop_vector.x * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    + loop_vector.y * loop_segment),
            .y = (int)(node->coordinates.y + loop_vector.y * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    - loop_vector.x * loop_segment)
    };

    SelectObject(hdc, get_edges_pen());
    Arc(hdc, (int)(loop_center.x - LOOP_RADIUS), (int)(loop_center.y - LOOP_RADIUS),
        (int)(loop_center.x + LOOP_RADIUS), (int)(loop_center.y + LOOP_RADIUS),
          loop_start.x, loop_start.y, loop_end.x, loop_end.y);

    struct coordd loop_arrow_vector = {
            .x = loop_end.y - loop_center.y,
            .y = -(loop_end.x - loop_center.x)
    };
    loop_arrow_vector = normalize_vector(loop_arrow_vector);

    draw_arrow(hdc, &loop_arrow_vector, &loop_end);
}