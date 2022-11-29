#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    // Check malloc success or not
    if (!head) {
        free(head);
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;

    list_for_each_entry_safe (entry, safe, l, list) {
        free(entry->value);
        free(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    // Copy string s to node->value
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    // Insert node after head
    node->list.prev = head;
    node->list.next = head->next;
    head->next->prev = &node->list;
    head->next = &node->list;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    node->list.next = head;
    node->list.prev = head->prev;
    head->prev->next = &node->list;
    head->prev = &node->list;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int size = 0;
    struct list_head *ptr;
    list_for_each (ptr, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast;
    for (fast = head->next; fast != head && fast->next != head;
         fast = fast->next->next)
        slow = slow->next;
    list_del(slow);
    element_t *node = list_entry(slow, element_t, list);
    free(node->value);
    free(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *duplist = q_new();
    struct list_head *node, *safe;
    int found_dup = 0;
    list_for_each_safe (node, safe, head) {
        char *cur = list_entry(node, element_t, list)->value;
        char *next = list_entry(safe, element_t, list)->value;
        int res = (node->next != head) && !(strcmp(cur, next));
        if (res || found_dup) {
            list_move(node, duplist);
            found_dup = res;
        }
    }
    q_free(duplist);
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *left, *right;
    for (struct list_head *ptr = head->next; ptr != head && ptr->next != head;
         ptr = ptr->next) {
        left = ptr;
        right = ptr->next;

        left->next = right->next;
        right->prev = left->prev;
        left->prev->next = right;
        right->next->prev = left;
        left->prev = right;
        right->next = left;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *temp = NULL;
    for (struct list_head *ptr = head; temp != head; ptr = temp) {
        temp = ptr->next;
        ptr->next = ptr->prev;
        ptr->prev = temp;
    }
}

struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;
    struct list_head *left = mergesort(head);
    struct list_head *right = mergesort(fast);
    struct list_head *temp_head = head, **ptr = &temp_head, **node;
    for (node = NULL; left && right; *node = (*node)->next) {
        char *l_value = list_entry(left, element_t, list)->value;
        char *r_value = list_entry(right, element_t, list)->value;
        node = strcmp(l_value, r_value) < 0 ? &left : &right;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) left | (uintptr_t) right);
    return temp_head;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || head->next == head->prev)
        return;
    // Break circular linked-list
    struct list_head *end = head->prev;
    end->next = NULL;
    head->next = mergesort(head->next);
    // Rebuild circular doubly linked-list
    for (end = head; end->next != NULL; end = end->next)
        end->next->prev = end;
    head->prev = end;
    end->next = head;
}