project(mml)

#add_library()

add_executable(mmld mmld.cpp)
target_link_libraries(mmld  threepc
                            rpc
                            network
                            common
                            crypto
                            ${CMAKE_THREAD_LIBS_INIT}
) 