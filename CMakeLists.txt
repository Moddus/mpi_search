cmake_minimum_required(VERSION 2.8)
PROJECT( MPISEACHER )

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES( ./libs/the_silver_searcher )

add_subdirectory( src )
