/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_time.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_TIME_H__
#define __TINY_TIME_H__

#include "tiny_typedef.h"

#ifdef _WIN32
#include <time.h>
#else /* Linux */
#include <unistd.h>
#include <sys/time.h>
#endif /* _WIN32 */

TINY_BEGIN_DECLS


#ifdef _WIN32
int gettimeofday(struct timeval *tv, void *tz);
#endif /* _WIN32 */

void tiny_sleep(int second);
void tiny_usleep(int usecond);
int tiny_getstrtime(char buf[], int len);

static TINY_INLINE uint64_t tiny_getusec(void)
{
    struct timeval __tv__ = {0};
    return (gettimeofday(&__tv__, NULL) ? 0LL : 1000000LL * __tv__.tv_sec + __tv__.tv_usec); 
}


TINY_END_DECLS

#endif /* __TINY_TIME_H__ */
