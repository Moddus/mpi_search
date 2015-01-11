cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES( ../modules/cutest-1.5 )

ADD_EXECUTABLE( mpi_searcher_test
                StrUtil.c
                ../modules/cutest-1.5/CuTest.c
                test_all.c )

# Run test after executable is build
ADD_CUSTOM_COMMAND( TARGET mpi_searcher_test
                    POST_BUILD
                    COMMAND ${PROJECT_BINARY_DIR}/bin/mpi_searcher_test )
