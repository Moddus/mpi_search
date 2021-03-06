FIND_PACKAGE(MPI REQUIRED)

INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})

SET(source file_searcher.c
           regexw.c
           util.c
           log.c
           mpi_functions.c
           file_util.c
           csv.c
           ps_mpi.c
           timeutil.c )

ADD_EXECUTABLE( mpi_searcher
    main.c
    ${source} )

ADD_LIBRARY( mpi_searcher_lib STATIC
    ${source} )

TARGET_LINK_LIBRARIES( mpi_searcher_lib
    pcre
    ${MPI_LIBRARIES} )

TARGET_LINK_LIBRARIES( mpi_searcher
    pcre
    ${MPI_LIBRARIES} )

IF(MPI_COMPILE_FLAGS)
    SET_TARGET_PROPERTIES(mpi_searcher PROPERTIES
        COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
ENDIF()

IF(MPI_LINK_FLAGS)
    SET_TARGET_PROPERTIES(mpi_searcher PROPERTIES
        LINK_FLAGS "${MPI_LINK_FLAGS}")
ENDIF()

add_custom_target(
    MPIRUN mpirun -n 4 ${EXECUTABLE_OUTPUT_PATH}/mpi_searcher -s 23.75.241.41 -f
    ${CMAKE_SOURCE_DIR}/testfile
    DEPENDS mpi_searcher
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT Run a search with 4 mpi machines
)

add_custom_target(
    MPIRUN_CLION mpirun -n 4 ${CMAKE_CURRENT_BINARY_DIR}/mpi_searcher -s 23.75.241.41 -f
    ${CMAKE_SOURCE_DIR}/testfile
    DEPENDS mpi_searcher
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT Run a search with 4 mpi machines
)
