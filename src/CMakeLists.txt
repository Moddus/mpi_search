#TARGET_LINK_LIBRARIES( searcher )

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

ADD_EXECUTABLE( mpi_searcher 
    main.c
    file_searcher.c
    regexw.c
    util.c
    log.c
    file_util.c)

TARGET_LINK_LIBRARIES( mpi_searcher pcre )

target_link_libraries(mpi_searcher ${MPI_LIBRARIES})

IF(MPI_COMPILE_FLAGS)
    SET_TARGET_PROPERTIES(mpi_searcher PROPERTIES
        COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
ENDIF()

IF(MPI_LINK_FLAGS)
    SET_TARGET_PROPERTIES(mpi_searcher PROPERTIES
        LINK_FLAGS "${MPI_LINK_FLAGS}")
ENDIF()

