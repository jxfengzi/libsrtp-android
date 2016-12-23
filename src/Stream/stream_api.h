/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   stream_api.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __STREAM_API_H__
#define __STREAM_API_H__


#ifdef _MSC_VER
#   if  (defined stream_shared_EXPORTS)
#       define DLLEXPORT _declspec(dllexport)
#   else
#       define DLLEXPORT _declspec(dllimport)
#   endif
#   define DLLLOCAL
#else
#   ifdef __ANDROID__
#       define DLLEXPORT
#       define DLLLOCAL
#   else
#       define DLLEXPORT __attribute__ ((visibility("default")))
#       define DLLLOCAL __attribute__ ((visibility("hidden")))
#   endif /* __ANDROID__ */
#endif /* _MHAP_VER */


#ifdef STREAM_STATIC
#   define STREAM_API
#else
#   define STREAM_API DLLEXPORT
#endif /* STREAM_STATIC */


#endif /* __STREAM_API_H__ */
