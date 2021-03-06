#include <CuTest.h>
#include <stdio.h>
#include <util.h>

CuSuite* make_regex_suite();
CuSuite* make_csv_suite();
CuSuite* make_file_searcher_suite();

void RunAllTests(void)
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

#if 1
    CuSuiteAddSuite(suite, make_regex_suite());
    CuSuiteAddSuite(suite, make_csv_suite());
    CuSuiteAddSuite(suite, make_file_searcher_suite());
#endif

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int
main(int argc, char *argv[])
{
    out_fd = stdout; /*For Logging*/
    RunAllTests();

    return EXIT_SUCCESS;
}
