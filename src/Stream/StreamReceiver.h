/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   StreamReceiver.h
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __STREAM_RECEIVER_H__
#define __STREAM_RECEIVER_H__

#include "tiny_base.h"
#include "stream_api.h"

TINY_BEGIN_DECLS


struct _StreamReceiver;
typedef struct _StreamReceiver StreamReceiver;

STREAM_API StreamReceiver * StreamReceiver_New(void);
STREAM_API TinyRet StreamReceiver_Construct(StreamReceiver *thiz);
STREAM_API void StreamReceiver_Dispose(StreamReceiver *thiz);
STREAM_API void StreamReceiver_Delete(StreamReceiver *thiz);

typedef void (*StreamPacketHandler)(const char *data, int size);

STREAM_API TinyRet StreamReceiver_Start(StreamReceiver *thiz,
                                     const char *ip,
                                     int port,
                                     const char *keyBase64,
                                     uint32_t ssrc,
                                     StreamPacketHandler handler);

STREAM_API TinyRet StreamReceiver_Stop(StreamReceiver *thiz);


TINY_END_DECLS

#endif /* __STREAM_RECEIVER_H__ */


