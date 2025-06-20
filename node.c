#include "node.h"

void send_message(node *nodes, int from_id, int to_id, const char *message) {
    
    if (!nodes[to_id].alive && !nodes[from_id].alive) {
        printf("Both Node %d and Node %d are not alive.\n", from_id, to_id);
        return;
    } else if (!nodes[to_id].alive) {
        printf("Node %d is not alive.\n", to_id);
        return;
    } else if (!nodes[from_id].alive) {
        printf("Node %d is not alive.\n", from_id);
        return;
    }

    if (nodes[to_id].inbox_count >= 10) {
        printf("Node %d's inbox is full. Message not sent.\n", to_id);
        return;
    }

    strncpy(nodes[to_id].inbox[nodes[to_id].inbox_count], message, 255);
    nodes[to_id].inbox[nodes[to_id].inbox_count][255] = '\0';
    nodes[to_id].inbox_count++;

    printf("Message sent from Node %d to Node %d: \"%s\"\n", from_id, to_id, message);

}

void show_inbox(node *nodes, int node_id) {

    if (!nodes[node_id].alive) {
        printf("Node %d is not alive. Cannot show inbox.\n", node_id);
        return;
    }

    printf("Inbox of Node %d\n", node_id);

    if (nodes[node_id].inbox_count == 0) {
        printf("(empty)\n");
        return;
    }

    for (int i = 0; i < nodes[node_id].inbox_count; i++) {
        printf("[%d] %s\n", i + 1, nodes[node_id].inbox[i]);
    }

}

void crash_node(node *nodes, int node_id) {

    nodes[node_id].alive = false;
    printf("Node %d has been crashed\n", node_id);

}

void restart_node(node *nodes, int node_id) {

    nodes[node_id].alive = true;
    printf("Node %d has been restarted\n", node_id);

}

void clear_node(node *nodes, int node_id) {
    for (int i = 0; i < 10; i++) {
        nodes[node_id].inbox[i][0] = '\0';
    }
    nodes[node_id].inbox_count = 0;
    printf("Inbox of Node %d has been cleared.\n", node_id);
}

bool status_node(node *nodes, int node_id) {
    return nodes[node_id].alive;
}