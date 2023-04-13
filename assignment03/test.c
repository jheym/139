#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

struct entry {
    int data;
    STAILQ_ENTRY(entry) entries;        /* Singly linked tail queue */
};

STAILQ_HEAD(stailhead, entry);

int main(void)
{
    struct entry *n1, *n2, *n3, *np;
    struct stailhead head;                  /* Singly linked tail queue
                                                head */

    STAILQ_INIT(&head);                     /* Initialize the queue */

    n1 = malloc(sizeof(struct entry));      /* Insert at the head */
    STAILQ_INSERT_HEAD(&head, n1, entries);

    n1 = malloc(sizeof(struct entry));      /* Insert at the tail */
    STAILQ_INSERT_TAIL(&head, n1, entries);

    n2 = malloc(sizeof(struct entry));      /* Insert after */
    STAILQ_INSERT_AFTER(&head, n1, n2, entries);

    STAILQ_REMOVE(&head, n2, entry, entries); /* Deletion */
    free(n2);

    n3 = STAILQ_FIRST(&head);
    STAILQ_REMOVE_HEAD(&head, entries);     /* Deletion from the head */
    free(n3);

    n1 = STAILQ_FIRST(&head);
    n1->data = 0;
    int i;
    for (i = 1; i < 5; i++) {
        n1 = malloc(sizeof(struct entry));
        STAILQ_INSERT_HEAD(&head, n1, entries);
        n1->data = i;
    }
                                            /* Forward traversal */
    STAILQ_FOREACH(np, &head, entries)
        printf("%i\n", np->data);
                                            /* TailQ deletion */
    n1 = STAILQ_FIRST(&head);
    while (n1 != NULL) {
        n2 = STAILQ_NEXT(n1, entries);
        free(n1);
        n1 = n2;
    }
    STAILQ_INIT(&head);

    exit(EXIT_SUCCESS);
}