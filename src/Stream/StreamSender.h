/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   StreamSender.h
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


struct _StreamSender;
typedef struct _StreamSender StreamSender;

STREAM_API StreamSender * StreamSender_New(void);
STREAM_API TinyRet StreamSender_Construct(StreamSender *thiz);
STREAM_API void StreamSender_Dispose(StreamSender *thiz);
STREAM_API void StreamSender_Delete(StreamSender *thiz);

STREAM_API TinyRet StreamSender_Initialize(StreamSender *thiz, const char *ip, int port, const char *key, uint32_t ssrc);
STREAM_API void StreamSender_Finalize(StreamSender *thiz);
STREAM_API TinyRet StreamSender_Sendto(StreamSender *thiz, const char * buf, size_t size);


TINY_END_DECLS

#endif /* __STREAM_RECEIVER_H__ */