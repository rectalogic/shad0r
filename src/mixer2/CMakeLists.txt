find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(FREI0R REQUIRED frei0r)

include_directories(${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS} ${FREI0R_INCLUDE_DIRS})
link_directories(${GLFW_LIBRARY_DIRS})

add_library(shad0r MODULE shad0r.c)

target_link_libraries(shad0r ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(shad0r LINK_INTERFACE_LIBRARIES "")

set_target_properties(shad0r PROPERTIES PREFIX "")

install(TARGETS shad0r
  LIBRARY DESTINATION lib COMPONENT libraries
)
