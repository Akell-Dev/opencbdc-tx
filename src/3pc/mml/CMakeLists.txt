project(mml)

add_library(mml options.cpp
                server.cpp
                client.cpp
                interface.cpp
                format.cpp
)

add_executable(mmld mmld.cpp)
target_link_libraries(mmld  threepc
                            mml
                            rpc
                            network
                            serialization
                            common 
                            crypto
                            secp256k1
                            ${CMAKE_THREAD_LIBS_INIT}
) 