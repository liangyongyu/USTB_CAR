#add_subdirectory(stctrlServer)
# json
include_directories(${PROJECT_SOURCE_DIR}/libs/nlohmann/include)
#  websocket 库
include_directories(${PROJECT_SOURCE_DIR}/libs/websocketpp/include)

# 需要 boost 和 openssl
set (WEBSOCKETPP_BOOST_LIBS system thread)
list (APPEND WEBSOCKETPP_BOOST_LIBS random)
list (APPEND WEBSOCKETPP_BOOST_LIBS unit_test_framework)
find_package (Boost  COMPONENTS "${WEBSOCKETPP_BOOST_LIBS}")
include_directories (${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_libraries(${Boost_LIBRARIES})
find_package(ZLIB)
link_libraries (${ZLIB_LIBRARIES})
include_directories(${ZLIB_INCLUDE_DIR})


#添加python.h的头文件路径
include_directories(/usr/include/python2.7)

# 添加python动态链接库文件目录
link_directories(${PROJECT_SOURCE_DIR}/usr/lib/python2.7/config-x86_64-  linux-gnu)
set(SOURCE_FILES ../stCtrl.cc)


# 依赖库文件
link_libraries(libpython2.7.so)




find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})
link_libraries(${OPENSSL_LIBRARIES})
message (STATUS "OPENSSL_VERSION = ${OPENSSL_VERSION}")


set(BIN_NAME stcarCtrlServer)

file(GLOB project_src *.cpp *.cc *.ui)

include_directories(${PROJECT_SOURCE_DIR}/libs/msg/)
include_directories(${PROJECT_SOURCE_DIR}/libs/agv/)
include_directories(${PROJECT_SOURCE_DIR}/libs/ini/)

set(LIB_SRC
        ${PROJECT_SOURCE_DIR}/libs/agv/agv.cc
        )

set(can_src ${PROJECT_SOURCE_DIR}/libs/can)
include_directories(${can_src})
link_directories(${can_src})
link_libraries(libcontrolcan.so)

add_executable(${BIN_NAME} ${LIB_SRC} ${project_src})