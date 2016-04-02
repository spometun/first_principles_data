project(fp)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11") # Enable C++11

add_definitions(
        -Wall
        -Wextra
)


set(src_files
tools/Types.cpp
tools/StringUtils.cpp
tools/XmlSink.cpp
tools/HtmlParser.cpp
tools/Json.cpp
tools/HtmlFP.cpp
tools/SimpleLog.cpp
tools/Commands.cpp
)

set(hdr_files
tools/Types.h
tools/StringUtils.h
tools/XmlSink.h
tools/HtmlParser.h
tools/Json.h
tools/HtmlFP.h
tools/SimpleLog.h
tools/Commands.h
)

find_package(Boost REQUIRED COMPONENTS filesystem system)
add_library(fp_lib ${SHARED_LIBRARY} ${src_files} ${hdr_files})

add_executable(create_template main.cpp)
target_link_libraries(create_template fp_lib ${Boost_LIBRARIES} -lhtmlcxx )

add_executable(link_id_creator link_id_creator.cpp)
target_link_libraries(link_id_creator fp_lib ${Boost_LIBRARIES} -lhtmlcxx )

add_executable(link_http_creator link_http_creator.cpp)
target_link_libraries(link_http_creator fp_lib ${Boost_LIBRARIES} -lhtmlcxx )

