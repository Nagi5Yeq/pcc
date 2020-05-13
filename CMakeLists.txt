cmake_minimum_required(VERSION 3.4)
project(pcc)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(PCC_SOURCE
    Context.cc
    Driver.cc
    Log.cc
    main.cc
    Version.cc)

bison_target(pcc_parser Parser.y ${CMAKE_CURRENT_BINARY_DIR}/Parser.cc)
flex_target(pcc_lexer Lexer.l ${CMAKE_CURRENT_BINARY_DIR}/Lexer.cc)
add_flex_bison_dependency(pcc_lexer pcc_parser)

add_definitions(-DPCC_OSNAME="${CMAKE_SYSTEM}")

list(APPEND PCC_SOURCE
    node/BaseNode.cc
    node/LiteralNode.cc)


list(APPEND PCC_SOURCE
    ${BISON_pcc_parser_OUTPUTS}
    ${FLEX_pcc_lexer_OUTPUTS})

add_executable(pcc ${PCC_SOURCE})
target_include_directories(pcc PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/node
    ${CMAKE_CURRENT_BINARY_DIR})

llvm_map_components_to_libnames(llvm_libs support core)
target_link_libraries(pcc ${llvm_libs})
