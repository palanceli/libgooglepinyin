set(PINYIN_TEST_SRCS
	pinyinime_unittest.cpp
)

add_executable(pinyin_test ${PINYIN_TEST_SRCS})

set(LINKER_LIBS "")
list(APPEND LINKER_LIBS glog::glog)
list(APPEND LINKER_LIBS googlepinyin-static)
list(APPEND LINKER_LIBS  ${GTEST_BOTH_LIBRARIES})
target_link_libraries(pinyin_test ${LINKER_LIBS})
