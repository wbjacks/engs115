/******************************************************************************/
/*                                                                            */
/* File: shop.c                                                               */
/* Author: Will Jackson '14                                                   */
/* Date: Sep 26, 2013                                                         */
/*                                                                            */
/* Uses a generic queue to make a user-interactive shopping list              */
/*                                                                            */
/******************************************************************************/

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./queue.h"
#include "./shop.h"

public int main(int argc, char *argv[]) {
    int c;
    char *pt;
    Shopping_List_t *shopping_list;
    List_Entry_t *entry;
    char input[INPUT_BUFFER_LENGTH];

    /* Open queue */
    shopping_list = (Shopping_List_t *)malloc(sizeof(Shopping_List_t));
    shopping_list->qp = qopen();
    shopping_list->entry_count = 0;

    /* LOOP... */
    for (;;) {
        /* Accept entries */
        entry = (List_Entry_t *)malloc(sizeof(List_Entry_t));

        /* Get item name */
        memset(entry->item_name, 0, INPUT_BUFFER_LENGTH);
        printf("Please input item name without spaces ('q' to quit): ");
        fgets(entry->item_name, INPUT_BUFFER_LENGTH, stdin);

        /* Check for exit request */
        if (entry->item_name[0] == 'q' && entry->item_name[1] == '\n') break;

        /* Remove trailing newline from input */
        pt = entry->item_name;
        while (*(++pt) != 0) {}
        *(pt-1) = 0;

        /* Get item count */
        memset(input, 0, INPUT_BUFFER_LENGTH);
        printf("Please input item count: ");
        scanf("%d", &(entry->count));

        /* Eat extra newline chars in stdin */
        while ((c = fgetc(stdin)) != EOF && c != '\n') {}

        /* Add to queue */
        qput(shopping_list->qp, (void *)entry);
        shopping_list->entry_count++;

        if (argc > 1 && atoi(argv[1]) == shopping_list->entry_count)
            printList(shopping_list);

    }
    /* Print entries */
    printList(shopping_list);
    
    /* Remove entries */
    clearList(shopping_list);
    free(entry);

    /* Close queue */
    qclose(shopping_list->qp);
    free(shopping_list);

    return EXIT_SUCCESS;
}

/* Used to print out the list. This could be refactored with qapply */
void printList(Shopping_List_t *sp) {
    int i;
    List_Entry_t *entry;

    printf("Shopping List:\n");
    printf("----------------------------\n");
    for (i = sp->entry_count - 1; i >= 0; i--) {
        entry = (List_Entry_t *)qget(sp->qp);
        printf("%20s | %5d\n", entry->item_name, entry->count);

        /* Return entry to list... */
        qput(sp->qp, (void *)entry);

        /* Yo there is a leak here ^^ because qget removes from the list without
            freeing */
    }
}

/* Used to deallocate the shopping list items */
void clearList(Shopping_List_t *sp) {
    /* qget returns NULL at empty list, which means Shopping_List_t isn't
       necessary And yes, I am keeping it because I am lazy. */
    List_Entry_t *entry;

    while ((entry = (List_Entry_t *)qget(sp->qp)) != NULL) {
        free(entry);

    }
} 
