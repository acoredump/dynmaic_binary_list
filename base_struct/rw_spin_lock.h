//
// Created by phaeton on 2024/4/30.
//

#ifndef PHAETON_RW_SPIN_LOCK_H
#define PHAETON_RW_SPIN_LOCK_H
#include "../base.h"

typedef struct {
    /*
     * cnt > 0   : have cnt readers
     * cnt == -1 : have a writer
     * cnt == 0  : no readers and no writer
     * write_status == 1 : a writer is doing now
     * */
    volatile int32_t cnt;
    volatile int32_t write_status;
} rwlock_status;

class RW_SPIN_LOCK{
public:
    RW_SPIN_LOCK();
    ~RW_SPIN_LOCK();
    void inline rlock();
    void inline unrlock();
    void inline wlock();
    void inline unwlock();

private:
    rwlock_status _lock_info;
};

#endif //PHAETON_RW_SPIN_LOCK_H
