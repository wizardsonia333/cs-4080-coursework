// ch. 1 challenge question 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// list node
struct node {
    char *str;
    struct node *prev;
    struct node *next;
};

struct node *head = NULL;

// insert item
void insert(char *s) {
    struct node *newnode = malloc(sizeof(struct node));

    int length = strlen(s);
    newnode->str = malloc(length + 1);
    strcpy(newnode->str, s);

    newnode->next = NULL;
    newnode->prev = NULL;

    if (head == NULL) {
        head = newnode;
        return;
    }

    struct node *temp = head;

    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = newnode;
    newnode->prev = temp;
}

// find item
struct node *find(char *s) {
    struct node *temp = head;

    while (temp != NULL) {
        if (strcmp(temp->str, s) == 0) {
            return temp;
        }

        temp = temp->next;
    }

    return NULL;
}

// delete item
void delete(char *s) {
    struct node *n = find(s);

    if (n == NULL) {
        return;
    }

    if (n->prev != NULL) {
        n->prev->next = n->next;
    } else {
        head = n->next;
    }

    if (n->next != NULL) {
        n->next->prev = n->prev;
    }

    // free memory
    free(n->str);
    free(n);
}

// test functions
int main() {
    insert("hi");
    insert("hello");
    insert("world");

    struct node *x = find("hi");

    if (x != NULL) {
        printf("found hi\n");
    }

    delete("hello");

    x = find("hello");

    if (x == NULL) {
        printf("hello is deleted\n");
    }

    return 0;
}