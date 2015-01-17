#include <CuTest.h>
#include <util.h>
#include <csv.h>
#include <log.h>

void
GetColumn(CuTest* tc)
{
    char test[] = "a;b;c;d";
    char* val;
    ps_csv_get_column(test, &val, 0);
    CuAssertStrEquals(tc, "a", val);
    PS_FREE(val);
    ps_csv_get_column(test, &val, 1);
    CuAssertStrEquals(tc, "b", val);
    PS_FREE(val);
    ps_csv_get_column(test, &val, 2);
    CuAssertStrEquals(tc, "c", val);
    PS_FREE(val);
    ps_csv_get_column(test, &val, 3);
    CuAssertStrEquals(tc, "d", val);
    PS_FREE(val);
}

CuSuite*
make_csv_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, GetColumn);

    return suite;
}
