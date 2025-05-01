/// @file   tmon.c
/// @brief  RISC-V Virtual Platform - Test Monitor 

#include "tmon.h"

/* Test Monitor Event Queue */

static unsigned long buff[128];

static tmon_queue_t q = {
    .head = buff,
    .tail = buff,
    .buff = buff,
    .size = 128,
};


/* Test Monitor Privilege Modes */

const char *priv_s[8] = {
    "USER",                         // 0
    "SUPERVISOR",                   // 1
    "UNKNOWN PRIVILEGE MODE",       // 2
    "MACHINE",                      // 3
    "VIRTUAL USER",                 // 4
    "VIRTUAL SUPERVISOR",           // 5
    "UNKNOWN PRIVILEGE MODE",       // 6
    "UNKNOWN PRIVILEGE MODE",       // 7
};


/* Test Monitor event queue API */

/// @name 
/// @brief
void queue_append(unsigned long e ) {

    *(q.tail++) = e;
    q.tail = (q.tail < (q.buff + q.size)) ? q.tail : q.buff;

    if (q.tail == q.head) {
        ERROR("queue is full\n");
        exit(-1);
    }  

}


/// @name 
/// @brief
unsigned long queue_pop(void ) {

    register unsigned long head = (q.head == q.tail) ? 0 : *(q.head++);
    q.head = (q.head < (q.buff + q.size)) ? q.head : q.buff;

    return head;
}

/// @name 
/// @brief
unsigned long queue_is_empty(void ) {

    return ((q.head - q.tail) < 0 ) ? q.tail - q.head : q.head - q.tail;

}
