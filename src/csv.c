#include <string.h>

#include "csv.h"
#include "log.h"

char*
ps_csv_get_column(char* content, int column)
{
    char* itr = content;
    char* val = content;
    int i = 0;

    while(NULL != (itr = strstr(itr, PS_CSV_SPACER)))
    {
        if(i == column)
        {
            //log_debug("%p %c\n", itr, *itr);
            //*itr = '\n';
            break;
        }
        itr++;
        i++;
        val = itr;
    }

    return val;
}
