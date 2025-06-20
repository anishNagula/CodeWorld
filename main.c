#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>
#include "node.h"
#include <pthread.h>
#define MAX_MESSAGES 100

typedef struct {
    node *computers;
    int no_nodes;
} SchedulerArgs;

// messages structue
typedef struct Message {
    int from_id;
    int to_id;
    char type[16];
    char content[256];
    int scheduled_tick;
    bool delivered;
} Message;

Message message_queue[MAX_MESSAGES];
int message_count = 0;

// global tick count
int global_tick = 0;

void handle_message(Message *message, node* computers){

    char *type = message->type;

    if (strcmp(type, "send") == 0) {
        send_message(computers, message->from_id, message->to_id, message->content);
    }

}


void *tick_scheduler(void *arg) {

    SchedulerArgs *args = (SchedulerArgs *)arg;
    node *computers = args->computers;
    int no_nodes = args->no_nodes;


    while (1) {
        usleep(10000);    // 1 tick = 10ms
        global_tick++;

        for (int i = 0; i < message_count; i++) {
            if (!message_queue[i].delivered && message_queue[i].scheduled_tick <= global_tick) {
                // process message
                printf("[Tick %d] Delivering message from %d to %d: %s\n",
                       global_tick, message_queue[i].from_id, message_queue[i].to_id,
                       message_queue[i].content);

                handle_message(&message_queue[i], computers);
                message_queue[i].delivered = true;

                printf(">> ");
                fflush(stdout);
            }
        }
    }
}

void add_to_msg_queue(int from_id, int to_id, const char *type, const char *msg) {

    if (message_count >= MAX_MESSAGES) {
        printf("Message queue full!\n");
        return;
    }

    Message new_message;
    new_message.from_id = from_id;
    new_message.to_id = to_id;
    strncpy(new_message.type, type, sizeof(new_message.type) - 1);
    new_message.type[sizeof(new_message.type) - 1] = '\0';
    if (msg)
        strncpy(new_message.content, msg, sizeof(new_message.content) - 1);
    else
        new_message.content[0] = '\0';  // empty content if msg is NULL
    new_message.content[sizeof(new_message.content) - 1] = '\0';
    new_message.scheduled_tick = global_tick;
    new_message.delivered = false;

    message_queue[message_count++] = new_message;
}

int main() {

    

    int no_nodes;
    printf("Welcome to the simulation\nHow many nodes would you like to spawn: ");
    scanf("%d", &no_nodes);
    getchar();

    node *computers = malloc(no_nodes * sizeof(node));
    if (computers == NULL) {
        perror("Memory Allocation failed.\n ");
        return 1;
    }

    pthread_t tick_thread;
    SchedulerArgs args = {computers, no_nodes};
    pthread_create(&tick_thread, NULL, tick_scheduler, &args);

    for (int i = 0; i < no_nodes; i++) {
        computers[i].node_id = i;
        computers[i].inbox_count = 0;
        computers[i].alive = true;
    }

    char input[512];

    while (1) {
        
        printf(">> ");
        if(!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = 0;

        if (strncmp(input, "send", 4) == 0) {
            int from, to;
            char message[256];
            if (sscanf(input, "send %d %d %[^\n]", &from, &to, message) == 3) {
                add_to_msg_queue(from, to, "send", message);
            } else {
                printf("Usage: send <from_id> <to_id> <message>\n");
            }
        } else if (strncmp(input, "show", 4) == 0) {
            int id;
            if(sscanf(input, "show %d", &id) == 1 && id < no_nodes) {
                add_to_msg_queue(id, -1, "show", NULL);
            } else {
                printf("Usage: show <node_id>\n");
            }
        } else if (strncmp(input, "crash", 5) == 0) {
            int id;
            if(sscanf(input, "crash %d", &id) == 1 && id < no_nodes) {
                add_to_msg_queue(id, -1, "crash", NULL);
            } else {
                printf("Usage: crash <node_id>\n");
            }
        } else if (strncmp(input, "restart", 7) == 0) {
            int id;
            if(sscanf(input, "restart %d", &id) == 1 && id < no_nodes) {
                add_to_msg_queue(id, -1, "restart", NULL);
            } else {
                printf("Usage: restart <node_id>\n");
            }
        } else if (strncmp(input, "clear", 5) == 0) {
            int id;
            if(sscanf(input, "clear %d", &id) == 1 && id < no_nodes) {
                add_to_msg_queue(id, -1, "clear", NULL);
            } else {
                printf("Usage: clear <node_id>\n");
            }
        } else if (strncmp(input, "status", 6) == 0) {
            int id;
            if(sscanf(input, "status %d", &id) == 1 && id < no_nodes) {
                add_to_msg_queue(id, -1, "status", NULL);
            } else {
                printf("Usage: status <node_id>\n");
            }
        } else if (strcmp(input, "exit") == 0) {
            free(computers);
            break;

        } else {
            printf("Unknown command. Try: send, show, crash, restart, status, exit\n");
        }
    }

    return 0;
}