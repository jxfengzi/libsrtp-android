#-----------------------------------------------------------------------------
#
# build
#
#-----------------------------------------------------------------------------
ADD_EXECUTABLE(test_receiver receiver.c)
SET_TARGET_PROPERTIES(test_receiver PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(test_receiver stream_shared ${OS_LIB})

ADD_EXECUTABLE(test_sender sender.c)
SET_TARGET_PROPERTIES(test_sender PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(test_sender stream_shared ${OS_LIB})
