#ifndef MPI_FUNCTIONS_H
#define MPI_FUNCTIONS_H 

#define MASTER 0

#define MPI_TAG_FILENAME_LENGTH 0
#define MPI_TAG_SEARCH_TASK 1

typedef struct search_task{
    /*Gibt Bereich an, in dem gesucht werden soll, in Byte*/
    unsigned long start, end;
    /*Laenge des Dateinamens*/
    unsigned int filename_size;
    /*Datei, die durchsucht werden soll*/
    char filename[];
} search_task_t;

#endif
