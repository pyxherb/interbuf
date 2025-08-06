find_package(re2c REQUIRED)

add_re2c_target(interbufc_lexer lexer.in.cpp)

file(GLOB HEADERS *.h *.hh)
file(GLOB SRC *.c *.cc)

target_sources(interbufc PRIVATE ${HEADERS} ${SRC})
target_link_libraries(interbufc PRIVATE interbufc_lexer)

add_subdirectory("parser")
