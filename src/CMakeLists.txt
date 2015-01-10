#TARGET_LINK_LIBRARIES( searcher )

ADD_EXECUTABLE( mpi_searcher 
                main.c
                file_searcher.c
                regexw.c
                util.c
                log.c )

TARGET_LINK_LIBRARIES( mpi_searcher
                       pcre )
