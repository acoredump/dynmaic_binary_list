//
// Created by phaeton on 2024/4/7.
//

#ifndef PHAETON_BINARY_LIST_H
#define PHAETON_BINARY_LIST_H
#include "../base.h"
#include "rw_spin_lock.h"

#define DYNAMIC_ROW_MIN_LENGTH 512
#define DYNAMIC_ROW_MAX_LENGTH 1024
#define DYNAMIC_ROW_UPGRADE_UINT 64

#define DYNAMIC_ROW_INDEX_UNIT 17
#define DYNAMIC_MAX_ROW_INDEX_NUM 4096


/*
 * if left euqal right return 0
 * if left greater than right return 1
 * if left smaller than right return -1
 * */
typedef int (*binary_compare)(void* left, void* right);
typedef void (*binary_print)(void* node);

typedef struct row_header{
    uint16_t row_size;
    uint16_t used_size;
}row_header;

typedef struct dynamic_row {
    dynamic_row * prev;
    dynamic_row * next;
    row_header header;
    void *buf[]; //Flexible arrays
}dynamic_row;

typedef struct dynamic_row_index {
    dynamic_row * start;
    dynamic_row * middle;
}dynamic_row_index;

typedef struct dynamic_row_pos {
    dynamic_row * row;
    int pos;
}dynamic_row_pos;

class DYN_BINARY_LIST{
public:
    explicit DYN_BINARY_LIST(binary_compare bin_cmp, binary_print bin_print);
    virtual  ~DYN_BINARY_LIST();

    void create();
    bool insert(void *node);
    bool remove(void *node);
    bool search(void *node, dynamic_row_pos *at_row);
    bool search_row_index(void *node, dynamic_row_pos *at_row);

    int binary_row_search(dynamic_row *row, int low, int high, void *target);
    bool binary_row_insert(dynamic_row *row, int position, int length, void *target);
    bool binary_row_delete(dynamic_row *row, int position);

    bool dynamic_row_search_and_insert(dynamic_row *row,void *target);
    void build_row_index_points(dynamic_row *row , dynamic_row *new_row);

    void print_list_info();
    void print_detail_list();
    void check_list_continuity();

private:

    uint32_t _rows_num{};
    uint32_t _row_index_no{};
    uint32_t _rebuild_index_count{};

    dynamic_row *_header{};
    binary_compare _bin_cmp;
    binary_print _bin_print;

    void *_tmp[DYNAMIC_ROW_MAX_LENGTH*2]{};
    dynamic_row_index _row_index_pro[DYNAMIC_MAX_ROW_INDEX_NUM]{};

    //lock the whole list
    RW_SPIN_LOCK _global_lock;
    //lock the single dynamic_row
    RW_SPIN_LOCK _partial_lock;
};


#endif //PHAETON_BINARY_LIST_H
