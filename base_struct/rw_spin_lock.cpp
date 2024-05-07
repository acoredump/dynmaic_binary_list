//
// Created by phaeton on 2024/4/30.
//
#include "rw_spin_lock.h"

RW_SPIN_LOCK::RW_SPIN_LOCK() {
    _lock_info.cnt = 0;
    _lock_info.write_status = 0;
}

RW_SPIN_LOCK::~RW_SPIN_LOCK()
{
}

void inline RW_SPIN_LOCK::rlock() {
    int32_t x;
    int success = 0;

    while (success == 0) {
        if(_lock_info.write_status == 1)
        {
            usleep(100);
        }
        x = _lock_info.cnt;
        /* write lock is held */
        if (x < 0) {
            continue;
        }
        success = __sync_bool_compare_and_swap(&_lock_info.cnt,x, x + 1);
    }
}

void inline RW_SPIN_LOCK::unrlock() {
    __sync_fetch_and_sub(&_lock_info.cnt, 1);
}

void inline RW_SPIN_LOCK::wlock() {
    int32_t x;
    int success = 0;

    while (success == 0) {
        _lock_info.write_status = 1; //the writer is writing now, block reader

        //then wait all readers and writer release..... and writers write_status each other
        //only a single writer can get the lock

        x = _lock_info.cnt;
        /* a lock is held (maybe many reader or a single writer) ,
         * only all readers and writer released , then can do ... */
        if (x != 0) {
            continue;
        }

        success = __sync_bool_compare_and_swap(&_lock_info.cnt, 0, -1);
    }
    _lock_info.write_status = 0;
}

void inline RW_SPIN_LOCK::unwlock() {
    __sync_fetch_and_add(&_lock_info.cnt, 1);
}

