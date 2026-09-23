#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TOTAL_TICKETS 20
#define BUYER_COUNT 4

/* Shared resources: ticket inventory and the owner of each numbered seat. */
int tickets_left = TOTAL_TICKETS;
int seat_owners[TOTAL_TICKETS] = {0};
pthread_mutex_t ticket_mutex = PTHREAD_MUTEX_INITIALIZER;

void *buy_tickets(void *argument) {
    int buyer_id = *(int *)argument;
    struct timespec delay = {0, 10000000}; /* 10 milliseconds */

    while (1) {
        /* Critical section: check availability and reserve a seat together. */
        pthread_mutex_lock(&ticket_mutex);
        if (tickets_left == 0) {
            pthread_mutex_unlock(&ticket_mutex);
            break;
        }

        int seat = TOTAL_TICKETS - tickets_left;
        seat_owners[seat] = buyer_id;
        --tickets_left;
        printf("Buyer %d bought seat %d | Tickets left: %d\n",
               buyer_id, seat + 1, tickets_left);
        pthread_mutex_unlock(&ticket_mutex);

        /* Wait outside the critical section so other buyers can purchase. */
        nanosleep(&delay, NULL);
    }
    return NULL;
}

int main(void) {
    pthread_t buyers[BUYER_COUNT];
    int buyer_ids[BUYER_COUNT];
    int started = 0;
    int failed = 0;

    printf("Concert Ticket Simulator (C / pthreads)\n");
    printf("%d tickets, %d buyer threads\n\n", TOTAL_TICKETS, BUYER_COUNT);

    for (int i = 0; i < BUYER_COUNT; ++i) {
        buyer_ids[i] = i + 1;
        int error = pthread_create(&buyers[i], NULL, buy_tickets, &buyer_ids[i]);
        if (error != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            failed = 1;
            break;
        }
        ++started;
    }
    for (int i = 0; i < started; ++i) {
        int error = pthread_join(buyers[i], NULL);
        if (error != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(error));
            return 1;
        }
    }

    /* All started threads have finished, so these reads need no lock. */
    int seats_sold = 0;
    for (int i = 0; i < TOTAL_TICKETS; ++i) {
        if (seat_owners[i] >= 1 && seat_owners[i] <= BUYER_COUNT) {
            ++seats_sold;
        }
    }
    int passed = !failed && tickets_left == 0 && seats_sold == TOTAL_TICKETS;
    printf("\nTickets sold: %d\nTickets remaining: %d\nVerification: %s\n",
           seats_sold, tickets_left, passed ? "PASS" : "FAIL");
    pthread_mutex_destroy(&ticket_mutex);
    return passed ? 0 : 1;
}
