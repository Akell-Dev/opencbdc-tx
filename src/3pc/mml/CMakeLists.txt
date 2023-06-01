project(mml)

add_library(mml options.cpp
                server.cpp
                client.cpp
                interface.cpp
)

add_executable(mmld mmld.cpp)
target_link_libraries(mmld  mml
                            threepc
                            rpc
                            network
                            common
                            crypto
                            ${CMAKE_THREAD_LIBS_INIT}
) 