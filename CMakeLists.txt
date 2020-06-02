cmake_minimum_required(VERSION 3.4)
project(pcc)

set(CMAKE_CXX_STANDARD 14)
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(PCC_SOURCE
    Color.cc
    Context.cc
    Driver.cc
    Log.cc
    main.cc
    Type.cc
    TypeIdentifier.cc
    TypeManager.cc
    VariableList.cc
    Version.cc)

list(APPEND PCC_SOURCE
    node/BaseNode.cc
    node/DeclNode.cc
    node/ExprNode.cc
    node/FunctionNode.cc
    node/LiteralNode.cc
    node/Node.cc
    node/StatementNode.cc)

list(APPEND PCC_SOURCE
    traveler/AsciiTraveler.cc
    traveler/Traveler.cc)

set(PCC_INCLUDE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/node
    ${CMAKE_SOURCE_DIR}/traveler
    ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
    add_compile_options(/Zc:__cplusplus)
    list(APPEND PCC_SOURCE
        win32/getopt.c)
    list(APPEND PCC_INCLUDE
        ${CMAKE_SOURCE_DIR}/win32)
endif()

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(pcc_parser Parser.y ${CMAKE_CURRENT_BINARY_DIR}/Parser.cc)
flex_target(pcc_lexer Lexer.l ${CMAKE_CURRENT_BINARY_DIR}/Lexer.cc)
add_flex_bison_dependency(pcc_lexer pcc_parser)

list(APPEND PCC_SOURCE
    ${BISON_pcc_parser_OUTPUTS}
    ${FLEX_pcc_lexer_OUTPUTS})

add_definitions(-DPCC_OSNAME="${CMAKE_SYSTEM}")

add_executable(pcc ${PCC_SOURCE})
target_include_directories(pcc PRIVATE ${PCC_INCLUDE})

llvm_map_components_to_libnames(llvm_libs support core x86asmparser x86codegen)
target_link_libraries(pcc ${llvm_libs})
