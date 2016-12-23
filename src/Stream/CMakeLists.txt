#-----------------------------------------------------------------------------
#
# build
#
#-----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# sender
#----------------------------------------------------------------------------
SET(SRC_SENDER
        StreamSender.h
        StreamSender.c
 )

#----------------------------------------------------------------------------
# receiver
#----------------------------------------------------------------------------
SET(SRC_RECEIVER
        StreamReceiver.h
        StreamReceiver.c
 )

#----------------------------------------------------------------------------
# shared library
#----------------------------------------------------------------------------
ADD_LIBRARY(stream_shared SHARED ${SRC_SENDER} ${SRC_RECEIVER} stream_api.h rtp.c util.c util.h)
SET_TARGET_PROPERTIES(stream_shared PROPERTIES OUTPUT_NAME "stream")
SET_TARGET_PROPERTIES(stream_shared PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(stream_shared ${SRTP_LIB} ${OS_LIB} tiny_static)

#-----------------------------------------------------------------------------
#
# test
#
#-----------------------------------------------------------------------------
ADD_SUBDIRECTORY(test)