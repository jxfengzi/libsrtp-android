#----------------------------------------------------------------------------
# jxfengzi@gmail.com
#
# 2011-6-24
#
#
#----------------------------------------------------------------------------

LOCAL_PATH := $(call my-dir)

INC_SRTP                := ../libsrtp/include
INC_CRYPTO              := ../libsrtp/crypto/include
INC_TINY_BASE           := ../src/Tiny/Base
INC_TINY_LOG            := ../src/Tiny/Log
INC_TINY_MEMORY         := ../src/Tiny/Memory
INC_TINY_CONTAINER      := ../src/Tiny/Container
INC_TINY_THREAD         := ../src/Tiny/Thread
INC_STREAM              := ../src/Stream
INC_ALL                 := $(INC_SRTP)                                      \
                           $(INC_CRYPTO)                                    \
                           $(INC_TINY_BASE)                                 \
                           $(INC_TINY_LOG)                                  \
                           $(INC_TINY_MEMORY)                               \
                           $(INC_TINY_CONTAINER)                            \
                           $(INC_TINY_THREAD)                               \
                           $(INC_STREAM)

MY_CFLAGS               := -D __ANDROID__                                   \
                           -fPIC                                            \
                           -Wno-multichar

#MY_ARCH                := arm64-v8a
MY_ARCH                 := armeabi

#----------------------------------------------------------------------------
# libsrtp.so
#----------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE            := srtp2
LOCAL_SRC_FILES         := ../../libsrtp/libsrtp2.so
include $(PREBUILT_SHARED_LIBRARY)

#----------------------------------------------------------------------------
# test: rtpw
#----------------------------------------------------------------------------
#include $(CLEAR_VARS)
#LOCAL_MODULE            := rtpw
#SRC_DIR                 := ../../libsrtp/test
#LOCAL_SRC_FILES         := $(SRC_DIR)/rtp.c                                 \
                           $(SRC_DIR)/rtpw.c
#LOCAL_C_FLAGS           := $(MY_CFLAGS)
#LOCAL_C_INCLUDES        := $(INC_SRTP) $(INC_CRYPTO)
#LOCAL_STATIC_LIBRARIES  := libsrtp2
#include $(BUILD_EXECUTABLE)

#----------------------------------------------------------------------------
# libtiny.a
#----------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE            := tiny
SRC_DIR                 := ../../src/Tiny
LOCAL_SRC_FILES         := $(SRC_DIR)/Base/tiny_ret.c                       \
                           $(SRC_DIR)/Base/tiny_time.c                      \
                           $(SRC_DIR)/Log/tiny_log_print.c                  \
                           $(SRC_DIR)/Memory/tiny_memory.c                  \
                           $(SRC_DIR)/Container/TinyList.c                  \
                           $(SRC_DIR)/Container/TinyMap.c                   \
                           $(SRC_DIR)/Container/TinyQueue.c                 \
                           $(SRC_DIR)/Thread/TinyCondition.c                \
                           $(SRC_DIR)/Thread/TinyMutex.c                    \
                           $(SRC_DIR)/Thread/TinySemaphore.c                \
                           $(SRC_DIR)/Thread/TinyThread.c
LOCAL_C_FLAGS           := $(MY_CFLAGS)
LOCAL_C_INCLUDES        := $(INC_ALL)
include $(BUILD_STATIC_LIBRARY)

#----------------------------------------------------------------------------
# libstream.so
#----------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE            := stream
SRC_DIR                 := ../../src/Stream
LOCAL_SRC_FILES         := $(SRC_DIR)/rtp.c                                 \
                           $(SRC_DIR)/util.c                                \
                           $(SRC_DIR)/StreamSender.c                        \
                           $(SRC_DIR)/StreamReceiver.c
LOCAL_C_FLAGS           := $(MY_CFLAGS)
LOCAL_C_INCLUDES        := $(INC_ALL)
LOCAL_SHARED_LIBRARIES  := libsrtp2
LOCAL_STATIC_LIBRARIES  := libtiny
LOCAL_LDLIBS            := -llog
include $(BUILD_SHARED_LIBRARY)

#----------------------------------------------------------------------------
# libstream_r_jni.so
#----------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE            := stream_r_jni
SRC_DIR                 := ../../src/Stream
LOCAL_SRC_FILES         := $(SRC_DIR)/com_ouyang_srtp_SrtpStreamReceiver.c
LOCAL_C_FLAGS           := $(MY_CFLAGS)
LOCAL_C_INCLUDES        := $(INC_ALL)
LOCAL_SHARED_LIBRARIES  := stream
LOCAL_LDLIBS            := -llog
include $(BUILD_SHARED_LIBRARY)

#----------------------------------------------------------------------------
# libstream_s_jni.so
#----------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE            := stream_s_jni
SRC_DIR                 := ../../src/Stream
LOCAL_SRC_FILES         := $(SRC_DIR)/com_ouyang_srtp_SrtpStreamSender.c
LOCAL_C_FLAGS           := $(MY_CFLAGS)
LOCAL_C_INCLUDES        := $(INC_ALL)
LOCAL_SHARED_LIBRARIES  := stream
LOCAL_LDLIBS            := -llog
include $(BUILD_SHARED_LIBRARY)

#----------------------------------------------------------------------------
# test: receiver
#----------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE            := receiver
SRC_DIR                 := ../../src/Stream/test
LOCAL_SRC_FILES         := $(SRC_DIR)/receiver.c
LOCAL_C_FLAGS           := $(MY_CFLAGS)
LOCAL_C_INCLUDES        := $(INC_ALL)
LOCAL_SHARED_LIBRARIES  := stream
LOCAL_LDLIBS            := -llog
include $(BUILD_EXECUTABLE)

#----------------------------------------------------------------------------
# test: sender
#----------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE            := sender
SRC_DIR                 := ../../src/Stream/test
LOCAL_SRC_FILES         := $(SRC_DIR)/sender.c
LOCAL_C_FLAGS           := $(MY_CFLAGS)
LOCAL_C_INCLUDES        := $(INC_ALL)
LOCAL_SHARED_LIBRARIES  := stream
LOCAL_LDLIBS            := -llog
include $(BUILD_EXECUTABLE)