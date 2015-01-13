#include <CuTest.h>
#include <stdio.h>
#include <util.h>

CuSuite* StrUtilGetSuite();
CuSuite* make_regex_suite();
CuSuite* make_csv_suite();

void RunAllTests(void)
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

#if 1
    CuSuiteAddSuite(suite, StrUtilGetSuite());
    CuSuiteAddSuite(suite, make_regex_suite());
    CuSuiteAddSuite(suite, make_csv_suite());
#endif

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void)
{
    out_fd = stdout; /*For Logging*/
    RunAllTests();
}
