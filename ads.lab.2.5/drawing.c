#pragma once

boolean get_edge_shift (struct coord* vector, struct coord* start, double max_length,
        struct coord* node, struct edge_shift* result);

void draw_edge (HDC hdc, struct coord* start, struct coord* end, struct coord* vector, struct edge_shift shifts[],
        int shifts_count, unsigned int forth, unsigned int back, unsigned int additional_margin,
        enum drawing_style forward_style, enum drawing_style backward_style);

void draw_edge_segment (HDC hdc, struct edge_shift* shift, struct coord* vector,
        int current_edge_margin, enum drawing_style style);

void draw_arrow (HDC hdc, struct coord* vector, struct coord* arrow_end, enum drawing_style style);


/**********************************************************************************************************************/

void draw_node (HDC hdc, struct node* node, enum drawing_style style) {
    SelectObject(hdc, get_nodes_pen(style));
    MoveToEx(hdc, node->coordinates.x.i + NODE_RADIUS, node->coordinates.y.i, NULL);

    AngleArc(hdc, node->coordinates.x.i, node->coordinates.y.i, NODE_RADIUS, 0, 360);

    char text[snprintf(NULL, 0, "%d", node->n)];
    sprintf(text, "%d", node->n);                                       // convert string to text

    RECT text_boundaries = {
            .right = node->coordinates.x.i + NODE_TEXT_WIDTH_BOUNDARY,
            .left = node->coordinates.x.i - NODE_TEXT_WIDTH_BOUNDARY,
            .bottom = node->coordinates.y.i + NODE_TEXT_HEIGHT_BOUNDARY,
            .top = node->coordinates.y.i - NODE_TEXT_HEIGHT_BOUNDARY
    };
    DrawText(hdc, text, -1, &text_boundaries,
             DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);
}

/**********************************************************************************************************************/
/*
 * Prepares data for driving of an edge between 2 nodes.
 */

void render_edge (HDC hdc, int start_index, int end_index, struct graph* graph,
        enum drawing_style forward_style, enum drawing_style backward_style) {
    struct coord start = graph->nodes[start_index].coordinates,
            end = graph->nodes[end_index].coordinates;
    struct coord vector = {
          .x.d = end.x.i - start.x.i,
          .y.d = end.y.i - start.y.i
    };
    double length = sqrt(vector.x.d * vector.x.d + vector.y.d * vector.y.d);
    vector = normalize_vector(vector);

    struct coord start_point = {
            .x.i = (int) (start.x.i + vector.x.d * NODE_RADIUS),
            .y.i = (int) (start.y.i + vector.y.d * NODE_RADIUS)
    };
    struct coord end_point = {
            .x.i = (int) (end.x.i - vector.x.d * NODE_RADIUS),
            .y.i = (int) (end.y.i - vector.y.d * NODE_RADIUS)
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
              graph_get(graph, end_index, start_index), additional_margin, forward_style, backward_style);
}

/**********************************************************************************************************************/
/*
 * Draws parallel edges between 2 nodes.
 */

void draw_edge (HDC hdc, struct coord* start, struct coord* end, struct coord* vector, struct edge_shift shifts[],
        int shifts_count, unsigned int forth, unsigned int back, unsigned int additional_margin,
        enum drawing_style forward_style, enum drawing_style backward_style) {
    if (forward_style == HIDDEN && backward_style == HIDDEN) return;


    for (int edge_number = (int)additional_margin; edge_number < forth + back + additional_margin; edge_number++) {
        int current_edge_margin = (int)(EDGE_MARGIN * (edge_number - (forth + back + additional_margin - 1) / 2.0));

        enum drawing_style style = edge_number < forth ? forward_style : backward_style;
        if (style == HIDDEN) continue;
        SelectObject(hdc, get_edges_pen(style));

        struct coord current_edge_start = {
                .x.i = (int) (start->x.i + vector->x.d * NODE_MARGIN + vector->y.d * current_edge_margin),
                .y.i = (int) (start->y.i + vector->y.d * NODE_MARGIN - vector->x.d * current_edge_margin)
        };
        struct coord current_edge_end = {
                .x.i = (int) (end->x.i - vector->x.d * NODE_MARGIN + vector->y.d * current_edge_margin),
                .y.i = (int) (end->y.i - vector->y.d * NODE_MARGIN - vector->x.d * current_edge_margin)
        };


        MoveToEx(hdc, start->x.i, start->y.i, NULL);
        LineTo(hdc, current_edge_start.x.i, current_edge_start.y.i);

        if (shifts_count == 1) {                // if there is only 1 shift segment, draw entire edge as it
            shifts[0].start.x.i = (int) (start->x.i + vector->x.d * NODE_MARGIN);
            shifts[0].start.y.i = (int) (start->y.i + vector->y.d * NODE_MARGIN);

            shifts[0].end.x.i = (int) (end->x.i - vector->x.d * NODE_MARGIN);
            shifts[0].end.y.i = (int) (end->y.i - vector->y.d * NODE_MARGIN);

            draw_edge_segment(hdc, &shifts[0], vector, current_edge_margin, style);
        }
        else
            for (int i = 0; i < shifts_count; i++)
                draw_edge_segment(hdc, &shifts[i], vector, current_edge_margin, style);

        LineTo(hdc, current_edge_end.x.i, current_edge_end.y.i);
        LineTo(hdc, end->x.i, end->y.i);


        if (edge_number < forth) {
            struct coord arrow_vector = {
                    .x.d = end->x.i - current_edge_end.x.i,
                    .y.d = end->y.i - current_edge_end.y.i
            };
            arrow_vector = normalize_vector(arrow_vector);

            draw_arrow(hdc, &arrow_vector, end, forward_style);
        }
        else {
            struct coord arrow_vector = {
                    .x.d = start->x.i - current_edge_start.x.i,
                    .y.d = start->y.i - current_edge_start.y.i
            };
            arrow_vector = normalize_vector(arrow_vector);

            draw_arrow(hdc, &arrow_vector, start, backward_style);
        }
    }
}

/**********************************************************************************************************************/
/*
 * Draws edge segment corresponding to particular shift.
 */

void draw_edge_segment (HDC hdc, struct edge_shift* shift, struct coord* vector,
        int current_edge_margin, enum drawing_style style) {
    if (style == HIDDEN) return;
    SelectObject(hdc, get_edges_pen(style));

    LineTo(hdc, (int) (shift->start.x.i + vector->y.d * current_edge_margin),
           (int) (shift->start.y.i - vector->x.d * current_edge_margin));

    POINT points[3] = {
            {
                    .x = shift->start.x.i + shift->shift * vector->y.d + vector->y.d * current_edge_margin,
                    .y = shift->start.y.i - shift->shift * vector->x.d - vector->x.d * current_edge_margin
            },
            {
                    .x = shift->end.x.i + shift->shift * vector->y.d + vector->y.d * current_edge_margin,
                    .y = shift->end.y.i - shift->shift * vector->x.d - vector->x.d * current_edge_margin
            },
            {
                    .x = shift->end.x.i + vector->y.d * current_edge_margin,
                    .y = shift->end.y.i - vector->x.d * current_edge_margin
            }
    };
    PolyBezierTo(hdc, points, 3);
}

/**********************************************************************************************************************/
/*
 * Draws an arrow directed by vector and with the end in arrow_end point.
 */

void draw_arrow (HDC hdc, struct coord* vector, struct coord* arrow_end, enum drawing_style style) {
    if (!directed_graph)    // do not draw arrow if graph is not directed
        return;

    if (style == HIDDEN) return;
    SelectObject(hdc, get_edges_pen(style));

    MoveToEx(hdc, arrow_end->x.i, arrow_end->y.i, NULL);

    struct coord arrow_start = {
            .x.i = (int)(arrow_end->x.i - vector->x.d * ARROW_LENGTH),
            .y.i = (int)(arrow_end->y.i - vector->y.d * ARROW_LENGTH)
    };

    LineTo(hdc, (int)(arrow_start.x.i - vector->y.d * ARROW_WIDTH), (int)(arrow_start.y.i + vector->x.d * ARROW_WIDTH));
    LineTo(hdc, (int)(arrow_start.x.i + vector->y.d * ARROW_WIDTH), (int)(arrow_start.y.i - vector->x.d * ARROW_WIDTH));
    LineTo(hdc, arrow_end->x.i, arrow_end->y.i);

}

/**********************************************************************************************************************/
/*
 * Determines shift of the edge between 2 nodes around third.
 */

boolean get_edge_shift (struct coord* vector, struct coord* start,
        double max_length, struct coord* node, struct edge_shift* result) {
    double c = -vector->y.d * start->x.i + vector->x.d * start->y.i;

    int center_distance = -(int)(
            (vector->y.d * node->x.i - vector->x.d * node->y.i + c) /
            sqrt(vector->x.d * vector->x.d + vector->y.d * vector->y.d)
    );
    if (abs(center_distance) >= NODE_RADIUS + NODE_MARGIN)
        return 0;

    result->shift = (center_distance >= 0 ? 1 : -1) * (NODE_RADIUS + NODE_MARGIN)  -  center_distance;


    struct coord intersection = {
            .x.i = (int)(node->x.i + vector->y.d * center_distance),
            .y.i = (int)(node->y.i - vector->x.d * center_distance)
    };

    double length = (intersection.x.i - start->x.i) / vector->x.d;
    if (length <= 0 || length >= max_length)
        return 0;

    double circle_distance = sqrt((NODE_RADIUS + NODE_MARGIN) * (NODE_RADIUS + NODE_MARGIN)
                                  - center_distance * center_distance);
    result->radius = (int)circle_distance;

    result->start.x.i = (int)(intersection.x.i - circle_distance * vector->x.d);
    result->start.y.i = (int)(intersection.y.i - circle_distance * vector->y.d);

    result->end.x.i = (int)(intersection.x.i + circle_distance * vector->x.d);
    result->end.y.i = (int)(intersection.y.i + circle_distance * vector->y.d);

    return 1;
}

/**********************************************************************************************************************/
/*
 * Draws loop edge as an arc of circle.
 */

void draw_loop (HDC hdc, struct node* node, struct graph* graph, enum drawing_style style) {
    struct coord loop_vector;
    if (node->n == 0) {
        loop_vector.x.d = cos(CENTRAL_LOOP_ANGLE);
        loop_vector.y.d = sin(CENTRAL_LOOP_ANGLE);
    }
    else {
        struct coord raw_loop_vector = {
                .x.d = node->coordinates.x.i - graph->nodes[0].coordinates.x.i,
                .y.d = node->coordinates.y.i - graph->nodes[0].coordinates.y.i
        };

        loop_vector = normalize_vector (raw_loop_vector);
    }

    struct coord loop_center = {
            .x.d = node->coordinates.x.i + loop_vector.x.d * (NODE_RADIUS + LOOP_CENTER_DISTANCE),
            .y.d = node->coordinates.y.i + loop_vector.y.d * (NODE_RADIUS + LOOP_CENTER_DISTANCE)
    };

    double loop_segment = sqrt(LOOP_RADIUS * LOOP_RADIUS - LOOP_CENTER_DISTANCE * LOOP_CENTER_DISTANCE);
    struct coord loop_start = {
            .x.i = (int)(node->coordinates.x.i + loop_vector.x.d * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    - loop_vector.y.d * loop_segment),
            .y.i = (int)(node->coordinates.y.i + loop_vector.y.d * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    + loop_vector.x.d * loop_segment)
    },
    loop_end = {
            .x.i = (int)(node->coordinates.x.i + loop_vector.x.d * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    + loop_vector.y.d * loop_segment),
            .y.i = (int)(node->coordinates.y.i + loop_vector.y.d * (NODE_RADIUS + LOOP_RADIUS - LOOP_CENTER_DISTANCE)
                    - loop_vector.x.d * loop_segment)
    };

    if (style == HIDDEN) return;
    SelectObject(hdc, get_edges_pen(style));

    Arc(hdc, (int)(loop_center.x.d - LOOP_RADIUS), (int)(loop_center.y.d - LOOP_RADIUS),
        (int)(loop_center.x.d + LOOP_RADIUS), (int)(loop_center.y.d + LOOP_RADIUS),
          loop_start.x.i, loop_start.y.i, loop_end.x.i, loop_end.y.i);

    struct coord loop_arrow_vector = {
            .x.d = loop_end.y.i - loop_center.y.d,
            .y.d = -(loop_end.x.i - loop_center.x.d)
    };
    loop_arrow_vector = normalize_vector(loop_arrow_vector);

    draw_arrow(hdc, &loop_arrow_vector, &loop_end, style);
}