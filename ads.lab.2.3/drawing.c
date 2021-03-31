#pragma once

boolean get_edge_shift (struct coordd* vector, struct coord* start,
                        double max_length,  struct coord* node, struct edge_shift* result);

void draw_edge (HDC hdc, struct coord* start, struct coord* end, struct coordd* vector,
                struct edge_shift* shifts, int shifts_count, unsigned int forth, unsigned int back);

void draw_edge_segment (HDC hdc, struct edge_shift* shift, struct coordd* vector, int current_edge_margin);


void draw_node (HDC hdc, struct node *node) {
    SelectObject(hdc, get_nodes_pen());
    MoveToEx(hdc, node->coordinates.x + NODE_RADIUS, node->coordinates.y, NULL);

    AngleArc(hdc, node->coordinates.x, node->coordinates.y, NODE_RADIUS, 0, 360);

    char text[snprintf(NULL, 0, "%d", node->n)];
    sprintf(text, "%d", node->n);

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

void render_edge (HDC hdc, int start_index, int end_index, struct graph* graph) {
    struct coord start = graph->nodes[start_index].coordinates,
            end = graph->nodes[end_index].coordinates;
    struct coordd vector;
    double length;
    {
        int x = end.x - start.x,
                y = end.y - start.y;
        length = sqrt(x * x + y * y);

        vector.x = x / length;
        vector.y = y / length;
    }

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

    draw_edge(hdc, &start_point, &end_point, &vector, shifts, shifts_count,
              graph_get(graph, start_index, end_index), graph_get(graph, end_index, start_index));
}

/**********************************************************************************************************************/

void draw_edge (HDC hdc, struct coord* start, struct coord* end, struct coordd* vector,
                struct edge_shift shifts[], int shifts_count, unsigned int forth, unsigned int back) {
    SelectObject(hdc, get_edges_pen());

    for (int edge_number = 0; edge_number < forth + back; edge_number++) {
        int current_edge_margin = (int)(EDGE_MARGIN * (edge_number - (forth + back - 1) / 2.0));

        struct coord current_edge_start = {
                .x = (int) (start->x + vector->x * NODE_MARGIN + vector->y * current_edge_margin),
                .y = (int) (start->y + vector->y * NODE_MARGIN - vector->x * current_edge_margin)
        };

        if (edge_number <= forth) {
            struct coord arrow_vector = {

            };
            draw_arrow(arrow_vector, end);
        }
        else {
            struct coord arrow_vector = {

            };
            draw_arrow(arrow_vector, start);
        }


        MoveToEx(hdc, start->x, start->y, NULL);
        LineTo(hdc, (int) (start->x + vector->x * NODE_MARGIN + vector->y * current_edge_margin),
        );

        if (shifts_count == 1) {
            shifts[0].start.x = (int) (start->x + vector->x * NODE_MARGIN);
            shifts[0].start.y = (int) (start->y + vector->y * NODE_MARGIN);

            shifts[0].end.x = (int) (end->x - vector->x * NODE_MARGIN);
            shifts[0].end.y = (int) (end->y - vector->y * NODE_MARGIN);

            draw_edge_segment(hdc, &shifts[0], vector, current_edge_margin);
        }
        else
            for (int i = 0; i < shifts_count; i++)
                draw_edge_segment (hdc, &shifts[i], vector, current_edge_margin);

        LineTo(hdc, (int) (end->x - vector->x * NODE_MARGIN + vector->y * current_edge_margin),
               (int) (end->y - vector->y * NODE_MARGIN - vector->x * current_edge_margin));
        LineTo(hdc, end->x, end->y);
    }
}

/**********************************************************************************************************************/

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
