#include <CuTest.h>
#include <util.h>
#include <csv.h>
#include <log.h>

void
GetColumn(CuTest* tc)
{
    char* col = ps_csv_get_column("test", 0);
    log_debug("%s\n", col);
}

CuSuite*
make_csv_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, GetColumn);

    return suite;
}
