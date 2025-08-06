find_package(peff REQUIRED)

file(GLOB HEADERS *.h)
file(GLOB SRC *.cc)

add_executable(interbufc)
set_target_properties(interbufc PROPERTIES CXX_STANDARD 17)
target_sources(interbufc PRIVATE ${HEADERS} ${SRC})
target_link_libraries(interbufc PUBLIC peff_advutils_static peff_containers_static)
target_include_directories(interbufc PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/> $<INSTALL_INTERFACE:include/>)
set_target_properties(interbufc PROPERTIES
    PUBLIC_HEADER "${HEADERS}"
    VERSION ${PROJECT_VERSION}
)

add_subdirectory("ast")

install(TARGETS interbufc
    EXPORT interbufTargets
    LIBRARY DESTINATION lib/
    ARCHIVE DESTINATION lib/
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/interbuf/
)
