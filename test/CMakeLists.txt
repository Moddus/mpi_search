cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(MPI REQUIRED)

INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})

INCLUDE_DIRECTORIES( ../modules/cutest-1.5
                     ../src/
                     ../modules/pcre )

ADD_EXECUTABLE( mpi_searcher_test
                t_regexw.c
                t_csv.c
                t_file_searcher.c
                ../modules/cutest-1.5/CuTest.c
                test_all.c )

TARGET_LINK_LIBRARIES( mpi_searcher_test
                       mpi_searcher_lib
                       ${MPI_LIBRARIES} )

# Run test after executable is build
ADD_CUSTOM_COMMAND( TARGET mpi_searcher_test
                    POST_BUILD
                    COMMAND ${PROJECT_BINARY_DIR}/bin/mpi_searcher_test )

IF(MPI_COMPILE_FLAGS)
    SET_TARGET_PROPERTIES(mpi_searcher PROPERTIES
        COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
ENDIF()

IF(MPI_LINK_FLAGS)
    SET_TARGET_PROPERTIES(mpi_searcher PROPERTIES
        LINK_FLAGS "${MPI_LINK_FLAGS}")
ENDIF()
