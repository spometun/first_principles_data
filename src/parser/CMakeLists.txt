project(fp)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11") # Enable C++11

add_definitions(
        -Wall
        -Wextra
)


set(src_files
main.cpp
StringUtils.cpp
XmlSink.cpp
HtmlSource.cpp
SimpleLog.cpp
)

set(hdr_files
Types.h
StringUtils.h
XmlSink.h
HtmlSource.h
SimpleLog.h
)

add_executable(fp_template ${src_files} ${hdr_files})
target_link_libraries(fp_template -lhtmlcxx)

