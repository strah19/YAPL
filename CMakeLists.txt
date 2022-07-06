cmake_minimum_required(VERSION 3.10)

project(YAPL VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_VM "Use YAPL's implemented virtual machine" ON)

configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in ${PROJECT_SOURCE_DIR}/include/config.h)

include_directories(include)
include_directories(include/interpreter)
file(GLOB SOURCES "src/*.cpp" "src/interpreter/*.cpp")

if (USE_VM)
    add_subdirectory(VM)
    list(APPEND EXTRA_LIBS VM)
endif()

add_executable(YAPL ${SOURCES})

target_link_libraries(YAPL PUBLIC ${EXTRA_LIBS})

target_include_directories(YAPL PUBLIC "${PROJECT_BINARY_DIR}/include" 
                                       "${PROJECT_BINARY_DIR}/include/interpreter")

install(TARGETS YAPL DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/include/config.h"
        DESTINATION include)

enable_testing()

# does the application run
add_test(NAME Runs COMMAND YAPL "../tests/basic.yapl")

add_test(NAME Usage COMMAND YAPL)
set_tests_properties(Usage PROPERTIES PASS_REGULAR_EXPRESSION "no input file found")

# define a function to simplify adding tests
function(do_test target arg result)
  add_test(NAME Comp${arg} COMMAND ${target} ${arg})
  set_tests_properties(Comp${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction()