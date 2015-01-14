#include <string.h>

#include "csv.h"
#include "log.h"
#include "util.h"

char*
ps_csv_get_column(char* content,
                  int column)
{
    int i = 0;

    char* itr = strtok(content, PS_CSV_SPACER);
    while(NULL != itr)
    {
        itr = strtok(NULL, PS_CSV_SPACER);
    }

    return content;
}
