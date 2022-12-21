cmake_minimum_required(VERSION 3.12)
project(c22v1)

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# find_package(BISON REQUIRED)
# find_package(FLEX REQUIRED)

# BISON_TARGET(mc_parser
#         ./front/lrparser.y
#         ./front/lrparser.tab.c)

# FLEX_TARGET(mc_lexer
#         ./front/lrlex.l
#         ./front/lrlex.yy.c)

# ADD_FLEX_BISON_DEPENDENCY(mc_lexer mc_parser)


#file(GLOB_RECURSE DIR_TAC "./*.c")
file(GLOB_RECURSE DIR_LIB ./libs/sc_list.c ./libs/sc_map.c ./sc_array.h)
file(GLOB_RECURSE DIR_TAC tacmain.c bblock.c value.c user.c use.c symtab.c instruction.c type.c function.c liveness.c)
file(GLOB_RECURSE DIR_IR ./ir/variable_storage_space.c)
file(GLOB_RECURSE DIR_CONTAINER ./src/hash_map.c ./src/hash_set.c)
include_directories(libs)

add_compile_options(-g -O0)
add_executable(compiler
        # ${FLEX_mc_lexer_OUTPUTS}
        # ${BISON_mc_parser_OUTPUTS}
        ${DIR_MY}
        ${DIR_TAC}
        ${DIR_IR}
        ${DIR_LIB}
        ${DIR_CONTAINER}
        )
set(CMAKE_BUILD_TYPE Debug)
#aux_source_directory(. DIR_SRC)

# file(GLOB_RECURSE DIR_LIB_SRC "third_party/*.cpp")
# add_compile_options(-g -O0)
# include_directories(src)
# include_directories(third_party/antlr4-runtime)
# add_executable(compiler ${DIR_SRC} ${DIR_LIB_SRC})
#add_executable(xiaolin main.cpp)