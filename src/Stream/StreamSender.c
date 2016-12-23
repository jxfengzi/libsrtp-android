/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   StreamSender.c
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "StreamSender.h"
#include <tiny_log.h>
#include <tiny_memory.h>

#define TAG     "StreamSender"

struct _StreamSender
{
    void *x;
};

StreamSender * StreamSender_New(void)
{
    StreamSender *thiz = NULL;

    do
    {
        thiz = (StreamSender *)tiny_malloc(sizeof(StreamSender));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED.");
            break;
        }

        if (RET_FAILED(StreamSender_Construct(thiz)))
        {
            StreamSender_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TinyRet StreamSender_Construct(StreamSender *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(StreamSender));

    } while (false);

    return ret;
}

void StreamSender_Dispose(StreamSender *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(StreamSender));
}

void StreamSender_Delete(StreamSender *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamSender_Dispose(thiz);
    tiny_free(thiz);
}
