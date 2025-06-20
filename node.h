#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct Node {
    int node_id;
    char inbox[10][256];
    int inbox_count;
    bool alive;
} node;

void send_message(node *nodes, int from_id, int to_id, const char *message);
void show_inbox(node *nodes, int node_id);
void crash_node(node *nodes, int node_id);
void restart_node(node *nodes, int node_id);
void clear_node(node *nodes, int node_id);
void status_node(node *nodes, int node_id);

#endif