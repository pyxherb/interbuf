file(GLOB HEADERS *.h *.hh)
file(GLOB SRC *.c *.cc)

target_sources(interbufc PRIVATE ${HEADERS} ${SRC})
