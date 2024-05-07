//
// Created by phaeton on 2024/3/28.
//

#include "simple_queue.h"

simple_queue::simple_queue(int max_ele_num, bool thread_safe) {
    _thread_safe = thread_safe;
    if (_thread_safe)
        pthread_spin_init(&_lock, 0);
    _max_ele_num = max_ele_num;
    _cur_ele_num = 0;
    _p_header = nullptr;
    _p_tailer = nullptr;
}

simple_queue::~simple_queue() = default;

elem_base * simple_queue::alloc_elem(int buff_len)
{
    auto * ptr = (elem_base *)malloc(sizeof(elem_base) + buff_len);
    ptr->buff_len = buff_len;
    ptr->continuity = UN_CONTINUITY;
    return ptr;
}

void simple_queue::release_elem(elem_base *elem)
{
    if(elem != nullptr)
        free(elem);
}
//具备 内存数据连续性的多个elem 入队列
bool simple_queue::push_elems(elem_base **elems, int num)
{
    int cnt = 0;
    bool rc = true;
    if (_thread_safe)
        pthread_spin_lock(&_lock);

    do{
        if (_cur_ele_num >= _max_ele_num)
        {
            rc = false;
            break;
        }
        if (num > 1)
        {
            elems[cnt]->continuity = cnt;
        }else
        {
            elems[cnt]->continuity = UN_CONTINUITY;
        }
        if (_p_header == _p_tailer && _p_header == nullptr)
        {
            //空队列
            _p_header = elems[cnt];
            _p_tailer = elems[cnt];
            auto *base = (elem_base *)elems[cnt];
            base->p_next = nullptr;
        }else
        {
            //非空队列，尾部插入
            auto *base = (elem_base *)_p_tailer;
            base->p_next = elems[cnt];
            _p_tailer = elems[cnt];
            base = (elem_base *)_p_tailer;
            base->p_next = nullptr;
        }
        cnt += 1;
    }while(cnt < num);

    if (rc)
    {
        _cur_ele_num ++;
    }

    if (_thread_safe)
        pthread_spin_unlock(&_lock);

    return rc;
}

//具备内存数据连续性的elems从队列中提取
bool simple_queue::fitch_elems(elem_base **elems, int &num)
{
    int cnt = 0;
    if (_thread_safe)
        pthread_spin_lock(&_lock);

    if (_cur_ele_num == 0)
    {
        num = 0;
        return false;
    }

    do{
        elems[cnt] = _p_header;
        //非空队列，从头开始提取
        auto *base = (elem_base *)elems[cnt];
        if (base->continuity == UN_CONTINUITY || base->continuity != cnt)
        {
            break;
        }
        _p_header = base->p_next;
        base->p_next = nullptr;
        cnt += 1;
    }while(true);

    if (cnt > 1)    //element元素内存buff之间 没有 连续性的元素
        num = cnt;
    else if (cnt == 0)
    {
        num = 1;
        _p_header = _p_header->p_next;
        ((elem_base *)elems[cnt])->p_next = nullptr;
    }


    _cur_ele_num --;
    if (_cur_ele_num == 0)
    {
        _p_header = nullptr;
        _p_tailer = nullptr;
    }

    if (_thread_safe)
        pthread_spin_unlock(&_lock);

    return true;
}

bool simple_queue::push_elem(elem_base *elem)
{
    bool rc = true;
    if (_thread_safe)
        pthread_spin_lock(&_lock);

    do{
        if (_cur_ele_num >= _max_ele_num)
        {
            rc = false;
            break;
        }
        if (_p_header == _p_tailer && _p_header == nullptr)
        {
            //空队列
            _p_header = elem;
            _p_tailer = elem;
            auto *base = (elem_base *)elem;
            base->p_next = nullptr;
        }else
        {
            //非空队列，尾部插入
            auto *base = (elem_base *)_p_tailer;
            base->p_next = elem;
            _p_tailer = elem;
            base = (elem_base *)_p_tailer;
            base->p_next = nullptr;
        }
        _cur_ele_num ++;
    }while(false);

    if (_thread_safe)
        pthread_spin_unlock(&_lock);

    return rc;
}

elem_base * simple_queue::fitch_elem()
{
    elem_base *p_elem = nullptr;
    if (_thread_safe)
        pthread_spin_lock(&_lock);

    do{
        if (_cur_ele_num == 0)
            break;

        p_elem = _p_header;
        auto *base = (elem_base *)p_elem;

        if (base->continuity != UN_CONTINUITY)
            break;

        if (_cur_ele_num == 1)
        {
            //取完数据即为空队列
            _p_header = nullptr;
            _p_tailer = nullptr;
        }else
        {
            _p_header = base->p_next;
        }
        base->p_next = nullptr;
        _cur_ele_num --;
    }while(false);

    if (_thread_safe)
        pthread_spin_unlock(&_lock);

    return p_elem;
}

uint32_t simple_queue::get_queue_max_size()
{
    return _max_ele_num;
}

uint32_t simple_queue::get_queue_cur_size()
{
    return _cur_ele_num;
}