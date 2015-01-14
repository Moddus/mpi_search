#ifndef FILE_SEARCHER_H
#define FILE_SEARCHER_H

typedef struct ps_search_task{
    /*Gibt Bereich an, in dem gesucht werden soll, in Byte*/
    unsigned long offset, size;
    /*Laenge des Dateinamens*/
    unsigned int filename_len;
    /*Datei, die durchsucht werden soll*/
    char filename[];
} ps_search_task_t;

int
ps_file_searcher_search(ps_search_task_t *task);

#endif
