#TARGET_LINK_LIBRARIES( searcher )

ADD_EXECUTABLE( mpi_searcher 
                main.c )

TARGET_LINK_LIBRARIES( mpi_searcher
                       searcher )
