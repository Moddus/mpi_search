#include <CuTest.h>
#include <util.h>
#include <csv.h>
#include <log.h>

void
GetColumn(CuTest* tc)
{
    char* col = ps_csv_get_column("1;2;3\n", 0);
}

CuSuite*
make_csv_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, GetColumn);

    return suite;
}
