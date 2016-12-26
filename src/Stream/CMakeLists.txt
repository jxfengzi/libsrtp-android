#-----------------------------------------------------------------------------
#
# build
#
#-----------------------------------------------------------------------------

SET(SRC
        stream_api.h
        util.h
        StreamSender.h
        StreamReceiver.h
        rtp.c
        util.c
        StreamSender.c
        StreamReceiver.c
 )

#----------------------

#----------------------------------------------------------------------------
# shared library
#----------------------------------------------------------------------------
ADD_LIBRARY(stream_shared SHARED ${SRC})
SET_TARGET_PROPERTIES(stream_shared PROPERTIES OUTPUT_NAME "stream")
SET_TARGET_PROPERTIES(stream_shared PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(stream_shared ${SRTP_LIB} ${OS_LIB} tiny_static)

#-----------------------------------------------------------------------------
#
# test
#
#-----------------------------------------------------------------------------
ADD_SUBDIRECTORY(test)