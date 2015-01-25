#include <string.h>

#include "csv.h"
#include "log.h"
#include "util.h"

ps_status_t
ps_csv_get_column(const char* content,
                  char** val,
                  int column)
{
    ps_status_t rv = PS_SUCCESS;
    int i = 0;

    if( column == PS_CSV_ALL_COL )
    {
        *val = (char*) content;

        return rv;
    }

    char* cp = strdup(content);

    char* itr = strtok(cp, PS_CSV_SPACER);
    if(column > 0)
    {
        while(NULL != itr)
        {
            itr = strtok(NULL, PS_CSV_SPACER);
            i++;
            if(i >= column) break;
        }
    }

    size_t len = strlen(itr) + 1;
    PS_MALLOC(*val, len * sizeof(char));
    strncpy(*val, itr, len);
    PS_FREE(cp);

    return rv;

error:
    log_err("error: %d\n", PS_ERROR_ALLOCATION);
    return PS_ERROR_ALLOCATION;

}
