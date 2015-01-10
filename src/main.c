#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "util.h"
#include "log.h"

int
main(int argc, char *argv[])
{
    int debug = 0;
    char *search = NULL;
    int index;
    int c;
    ps_status_t rv = PS_SUCCESS;
    out_fd = stdout;

    opterr = 0;
    while ((c = getopt (argc, argv, "hds:")) != -1)
    {
        switch (c)
        {
            case 'd':
                debug = 1;
                break;
            case 's':
                search = optarg;
                break;
            case 'h':
            default:
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                    "Unknown option character `\\x%x'.\n",
                    optopt);
                return EXIT_FAILURE;
        }
    }
    printf ("debug = %d, search = %s\n",
    debug, search);

    for (index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);

    char* test = NULL;
    PS_MALLOC(test, sizeof(char) * 10);

    printf("len: %lu\n", sizeof(test));

    test[15] = 'a';
    printf("%s \n", test);
    free(test);

    return EXIT_SUCCESS;

error:
    printf("sdfsdf Error code %d.", rv);
    free(test);

    return rv;
}
