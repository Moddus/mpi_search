cmake_minimum_required(VERSION 2.8)
PROJECT( MPISEACHER )

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES( modules/pcre )

add_subdirectory( src )
add_subdirectory( modules )
add_subdirectory( test )

# run tests with: make test
enable_testing()
add_test(mpi_seacher_test mpi_searcher_test)
