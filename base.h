//
// Created by phaeton on 2024/3/27.
//

#ifndef UNTITLED3_BASE_H
#define UNTITLED3_BASE_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <cstdint>
#include <assert.h>
#include <unistd.h>

typedef struct node
{
    struct node *next;		// Points at the next node in the list
    struct node *prev;	    // Points at the previous node in the list
    struct node *index;
    struct node *middle;
}BASE_NODE;

#endif //UNTITLED3_BASE_H
