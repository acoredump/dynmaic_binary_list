//
// Created by phaeton on 2024/3/27.
// A quick search link list , can replace for rb_tree
//

#ifndef UNTITLED3_FAST_LIST_H
#define UNTITLED3_FAST_LIST_H

#include "../base.h"
#include <random>

#define SKIPLIST_MAXLEVEL 64   /* Should be enough for 2^64 elements */
#define SKIPLIST_P 0.25        /* Skiplist P = 1/4 */

typedef struct sk_level {
    struct sk_node *forward;
    unsigned long span;
}sk_level;

typedef struct sk_node {
    void* ele;
    double score;
    struct sk_node *backward;
    sk_level level[];              //Flexible arrays
} sk_node;

/*
 * if left euqal right return 0
 * if left greater than right return 1
 * if left smaller than right return -1
 * */
typedef int (*sk_compare)(sk_node* left, void* right);
typedef void (*sk_print)(sk_node* node);

class SKIP_LIST{
public:
    explicit SKIP_LIST(sk_compare sk_cmp);
    virtual ~SKIP_LIST();

    bool create();
    void destroy();
    bool insert(void *Node, double score = 0);
    bool remove(void *Node, sk_node  **node, double score = 0);
    void unlink_node(sk_node *x, sk_node **update);
    void free_node(sk_node *Node);

    void print_list(sk_print print_node);

private:
    static int RandomLevel();
    static sk_node* alloc_node(uint8_t level ,void *ele, double socre=0.00);

private:
    sk_compare _sk_cmp;
    int _level;
    uint64_t  _length;
    struct sk_node *_header, *_tail;
};
#endif //UNTITLED3_FAST_LIST_H

