/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinySemaphore.h
 *
 * @remark
 *
 */

#ifndef __TINY_SEMAPHORE_H__
#define __TINY_SEMAPHORE_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


#if (defined __LINUX__) || (defined __ANDROID__)
#include <semaphore.h>  
#endif

#ifdef __MAC_OSX__
#include <semaphore.h>  
#include <fcntl.h>
#include <limits.h>
#endif

#ifdef _WIN32
#define MAX_SEM_SIZE    128
typedef HANDLE          ct_sem_t;
#endif /* _WIN32 */

#if (defined __LINUX__) || (defined __ANDROID__)
typedef sem_t           ct_sem_t;
#endif

#ifdef __MAC_OSX__
typedef struct _ct_sem_t
{
    char                name[PATH_MAX + 1];
    sem_t *             sem;
} ct_sem_t;
#endif

typedef struct _TinySemaphore
{
    ct_sem_t           sem;
} TinySemaphore;

TinySemaphore * TinySemaphore_New(void);
TinyRet TinySemaphore_Construct(TinySemaphore *thiz);
TinyRet TinySemaphore_Dispose(TinySemaphore *thiz);
void TinySemaphore_Delete(TinySemaphore *thiz);

bool TinySemaphore_Wait(TinySemaphore *thiz);
bool TinySemaphore_Post(TinySemaphore *thiz);


TINY_END_DECLS

#endif /* __TINY_SEMAPHORE_H__ */
