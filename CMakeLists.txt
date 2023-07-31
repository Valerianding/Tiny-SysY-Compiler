cmake_minimum_required(VERSION 3.10)

project(c22v1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER clang-10)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

#调用flex将auto_lex.l  在cmake-build-debug目录下面生成对应文件
FLEX_TARGET(mc_lexer ./front_end/auto_lex.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
#同理
BISON_TARGET(mc_parser ./front_end/lrparser.y ${CMAKE_CURRENT_BINARY_DIR}/lrparser.tab.c)

ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)

file(GLOB_RECURSE DIR_LIB ./libs/*.c)
file(GLOB_RECURSE DIR_IR ./ir/*.c)
file(GLOB_RECURSE DIR_OPT ./ir/opt/*.c)
file(GLOB_RECURSE DIR_CONTAINER ./src/*.c)
file(GLOB_RECURSE DIR_FRONT ./front_end/*.c)
file(GLOB_RECURSE DIR_BACK ./back_end/*.c)

link_libraries(m)

#可以不写绝对路径从而只包含文件名就可以了
include_directories(.)
include_directories(libs)
include_directories(math)
include_directories(src)
include_directories(ir)
include_directories(ir/opt)
include_directories(front_end)
include_directories(back_end)
include_directories(container)
#debug必备
add_compile_options(-Wall -g -O0)
#生成可以执行文件
add_executable(compiler
        ${FLEX_mc_lexer_OUTPUTS}
        ${BISON_mc_parser_OUTPUTS}
        ${DIR_FRONT}
        ${DIR_TAC}
        ${DIR_IR}
        ${DIR_OPT}
        ${DIR_LIB}
        ${DIR_CONTAINER}
        ${DIR_BACK}
        tacmain.c ir/opt/loopnorm.c ir/opt/loopnorm.h)





