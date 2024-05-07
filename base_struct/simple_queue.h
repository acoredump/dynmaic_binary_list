//
// Created by phaeton on 2024/3/28.
//

#ifndef PHAETON_SIMPLE_QUEUE_H
#define PHAETON_SIMPLE_QUEUE_H

#include "../base.h"

#define UN_CONTINUITY (-1)

struct elem_base
{
    elem_base   *p_next;
    int      continuity;  //-1代表非连续 elem; 否则从0开始编号
    uint32_t   buff_len;
};

struct queue_elem
{
    elem_base base;
    char  buff[0];
};

class simple_queue {
public:

    explicit simple_queue(int max_ele_num, bool thread_safe = true);
    virtual ~simple_queue();

    static elem_base * alloc_elem(int buff_len);
    static void release_elem(elem_base *elem);

    bool push_elems(elem_base **elems, int num);
    bool fitch_elems(elem_base **elems, int &num);

    bool push_elem(elem_base *elem);
    elem_base * fitch_elem();

    uint32_t get_queue_max_size();
    uint32_t get_queue_cur_size();

private:

    pthread_spinlock_t _lock{};
    bool _thread_safe;

    elem_base  *_p_header;
    elem_base  *_p_tailer;
    uint32_t _max_ele_num;
    uint32_t _cur_ele_num;
};

#endif //PHAETON_SIMPLE_QUEUE_H
