#ifndef CSV_H
#define CSV_H

#include "util.h"

#define PS_CSV_SPACER (";\n")

#define PS_CSV_ALL_COL (-1)

ps_status_t
ps_csv_get_column(const char* content,
                  size_t content_len,
                  char** val,
                  int* val_len,
                  int column);

#endif
