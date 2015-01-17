#ifndef CSV_H
#define CSV_H

#include "util.h"

#define PS_CSV_SPACER (";")

ps_status_t
ps_csv_get_column(const char* content,
                  char** val,
                  int column);

#endif
