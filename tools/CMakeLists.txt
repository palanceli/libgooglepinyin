set(DICTBUILDER_SRCS
	pinyinime_dictbuilder.cpp
)

add_executable(dictbuilder ${DICTBUILDER_SRCS})

set(LINKER_LIBS "")
list(APPEND LINKER_LIBS glog::glog)
list(APPEND LINKER_LIBS googlepinyin-static)
list(APPEND LINKER_LIBS  ${GTEST_BOTH_LIBRARIES})
target_link_libraries(dictbuilder ${LINKER_LIBS})

# target_link_libraries(dictbuilder googlepinyin)

# 添加拼音转换工具
add_executable(pinyin_conv pinyin_convertor.cpp)
target_link_libraries(pinyin_conv ${LINKER_LIBS})