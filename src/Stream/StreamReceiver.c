/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   StreamReceiver.c
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "StreamReceiver.h"
#include <tiny_log.h>
#include <tiny_memory.h>
#include <TinyThread.h>
#include <config.h>
#include <rtp.h>

#define TAG                 "StreamReceiver"
#define PACKET_LEN          1378

static void loop(void *param);

struct _StreamReceiver
{
    rtp_receiver_t          receiver;
    TinyThread              thread;
    StreamPacketHandler     handler;
    bool                    running;
};

StreamReceiver * StreamReceiver_New(void)
{
    StreamReceiver *thiz = NULL;

    do
    {
        thiz = (StreamReceiver *)tiny_malloc(sizeof(StreamReceiver));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED.");
            break;
        }

        if (RET_FAILED(StreamReceiver_Construct(thiz)))
        {
            StreamReceiver_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TinyRet StreamReceiver_Construct(StreamReceiver *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(StreamReceiver));
        thiz->handler = NULL;
        thiz->running = false;

        TinyThread_Construct(&thiz->thread);
        TinyThread_Initialize(&thiz->thread, loop, thiz, "StreamReceiver");

    } while (false);

    return ret;
}

void StreamReceiver_Dispose(StreamReceiver *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(StreamReceiver));
}

void StreamReceiver_Delete(StreamReceiver *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamReceiver_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet StreamReceiver_Start(StreamReceiver *thiz,
                          const char *ip,
                          int port,
                          const char *key,
                          uint32_t ssrc,
                          StreamPacketHandler handler)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ip, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(port, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ssrc, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    thiz->handler = handler;

    TinyThread_Start(&thiz->thread);

    return TINY_RET_OK;
}

TinyRet StreamReceiver_Stop(StreamReceiver *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

static void loop(void *param)
{
    StreamReceiver *thiz = (StreamReceiver *)param;

    thiz->running = true;

    while (thiz->running)
    {
        char packet[PACKET_LEN];
        int packet_len = PACKET_LEN;
        int size = 0;

        size = rtp_recvfrom(thiz->receiver, packet, &packet_len);
        if (size < 0)
        {
            break;
        }

        thiz->handler(packet, size);
    }
}
