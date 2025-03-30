#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *node =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!node)
        return NULL;
    node->next = node;
    node->prev = node;
    return node;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *cur = NULL, *node = NULL;
    list_for_each_entry_safe (cur, node, head, list) {
        free(cur->value);
        free(cur);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add(&node->list, head->prev);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->next, element_t, list);
    list_del(&node->list);
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
    list_del(&node->list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    struct list_head *temp = head;
    if (!head)
        return 0;
    int size = 0;
    while (temp && temp->next != head) {
        size++;
        temp = temp->next;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    element_t *node = list_entry(slow, element_t, list);
    list_del(slow);
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
    bool isdup = false;
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        element_t *cur = list_entry(node, element_t, list);
        if (safe != head &&
            strcmp(cur->value, list_entry(safe, element_t, list)->value) == 0) {
            list_del(node);
            free(cur->value);
            free(cur);
            isdup = true;
        } else if (isdup) {
            list_del(node);
            free(cur->value);
            free(cur);
            isdup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *node = head->next;
    while (node && node->next != head) {
        list_del(node);
        list_add(node, node->next);
        node = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        list_del(node);
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL, *safe = NULL, *start = head;
    int count = 0;
    list_for_each_safe (node, safe, head) {
        if (count < k - 1) {
            count++;
            printf("%d\n", count);
            continue;
        }
        LIST_HEAD(tmp);
        list_cut_position(&tmp, start, node);
        q_reverse(&tmp);
        list_splice(&tmp, start);
        start = safe->prev;
        count = 0;
    }
}

struct list_head *merge(struct list_head *left,
                        struct list_head *right,
                        bool descend)
{
    LIST_HEAD(head);
    INIT_LIST_HEAD(&head);
    struct list_head *node = &head;

    while (left && right) {
        const element_t *l = list_entry(left, element_t, list);
        const element_t *r = list_entry(right, element_t, list);
        if ((strcmp(l->value, r->value) <= 0)) {
            if (!descend) {
                node->next = left;
                left->prev = node;
                left = left->next;
            } else {
                node->next = right;
                right->prev = node;
                right = right->next;
            }
        } else {
            if (!descend) {
                node->next = right;
                right->prev = node;
                right = right->next;
            } else {
                node->next = left;
                left->prev = node;
                left = left->next;
            }
        }
        node = node->next;
    }
    node->next = left ? left : right;

    if (node->next)
        node->next->prev = node;

    return head.next;
}

struct list_head *merge_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head *mid = slow->next;

    slow->next = NULL;
    struct list_head *left = merge_sort(head, descend);
    struct list_head *right = merge_sort(mid, descend);
    return merge(left, right, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *start = head->next, *end = head->prev;
    start->prev = NULL;
    end->next = NULL;

    struct list_head *after_sort = merge_sort(start, descend);
    head->next = after_sort;
    after_sort->prev = head;
    end = after_sort;
    while (end->next)
        end = end->next;
    end->next = head;
    head->prev = end;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;

    struct list_head *tail = head->prev;
    const element_t *tail_elem = list_entry(tail, element_t, list);
    while (tail != head && tail->prev != head) {
        struct list_head *node = tail->prev;
        element_t *node_elem = list_entry(node, element_t, list);
        if (strcmp(node_elem->value, tail_elem->value) > 0) {
            list_del(node);
            free(node_elem->value);
            free(node_elem);
        } else {
            tail = node;
            tail_elem = node_elem;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    struct list_head *tail = head->prev;
    const element_t *tail_elem = list_entry(tail, element_t, list);
    while (tail != head && tail->prev != head) {
        struct list_head *node = tail->prev;
        element_t *node_elem = list_entry(node, element_t, list);
        if (strcmp(node_elem->value, tail_elem->value) < 0) {
            list_del(node);
            free(node_elem->value);
            free(node_elem);
        } else {
            tail = node;
            tail_elem = node_elem;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *new_head = list_entry(head->next, queue_contex_t, chain);
    struct list_head *node = head->next->next, *safe = node->next;
    while (node && node != head) {
        queue_contex_t *tmp = list_entry(node, queue_contex_t, chain);
        list_splice_tail_init(tmp->q, new_head->q);
        node = safe;
        safe = node->next;
    }
    q_sort(new_head->q, descend);
    return q_size(new_head->q);
}
