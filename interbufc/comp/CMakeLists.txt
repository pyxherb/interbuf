file(GLOB HEADERS *.h *.hh)
file(GLOB SRC *.c *.cc)

target_sources(interbufc PRIVATE ${HEADERS} ${SRC})
target_link_libraries(interbufc PRIVATE interbufc_lexer)

add_subdirectory("cxx")
add_subdirectory("ts")
