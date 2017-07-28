cmake_minimum_required(VERSION 2.8)

project(Golomb C)

execute_process(COMMAND gengetopt --input=${PROJECT_SOURCE_DIR}/golomb.ggo --include-getopt --output-dir=${PROJECT_SOURCE_DIR})

add_executable(test main.c cmdline.c cmdline.h)

# `m` is libm.a
target_link_libraries(test m)
