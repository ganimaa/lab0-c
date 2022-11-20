#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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
    if (sp)
        node->value = strdup(sp);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp)
        node->value = strdup(sp);
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int size = 0;
    for (struct list_head *ptr = head->next; ptr != head; ptr = ptr->next)
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
void q_reverse(struct list_head *head) {}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}
