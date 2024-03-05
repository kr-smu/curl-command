cmake_minimum_required(VERSION 2.6)
project(curl_command)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "-std=c++14 -fPIC ${CMAKE_CXX_FLAGS}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")
endif()
set(KR2_API_LIBS kr2rc_api kr2_io_api kr2_program_api)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
cmake_policy(SET CMP0015 NEW)

link_directories(/usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib $ENV{KR2_INSTALL_FOLDER}/lib $ENV{CBUN_INSTALL_FOLDER}/lib lib)

include_directories(include
                    /opt/kr2/include
                    ${Boost_INCLUDE_DIRS})

# Provide all your CBun source files (*.cpp)
add_library(${PROJECT_NAME} SHARED
            src/curl_command.cpp
)
target_link_libraries(${PROJECT_NAME} ${KR2_API_LIBS} ${Boost_LIBRARIES} curl)
install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{CBUN_INSTALL_FOLDER}/lib)

# Setup your integration test (if available) 
#add_executable(integration_test tests/integration_test.cpp)
#target_link_libraries(integration_test ${PROJECT_NAME} ${KR2_API_LIBS} ${Boost_LIBRARIES})
#install(TARGETS integration_test DESTINATION ${PROJECT_SOURCE_DIR}/tests)

# Build CBun from bundle.xml, headers, APK and CBun lib
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CBUN_BUILD_DIR ${PROJECT_NAME}.cbunbuild)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND rm -rf ${CBUN_BUILD_DIR}
                        COMMAND mkdir ${CBUN_BUILD_DIR}
                        COMMAND cp lib${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX} ${CBUN_BUILD_DIR}
                        COMMAND cp ${PROJECT_SOURCE_DIR}/../bundle.xml ${CBUN_BUILD_DIR}
                        COMMAND cp -r ${PROJECT_SOURCE_DIR}/include ${CBUN_BUILD_DIR}
                        COMMAND cp -r ${PROJECT_SOURCE_DIR}/lib ${CBUN_BUILD_DIR}
                        COMMAND tar -cvzf ${PROJECT_NAME}.cbun -C ${CBUN_BUILD_DIR} .
                    )
endif()

