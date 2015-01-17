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
    free(val);
    ps_csv_get_column(test, &val, 1);
    CuAssertStrEquals(tc, "b", val);
    free(val);
    ps_csv_get_column(test, &val, 2);
    CuAssertStrEquals(tc, "c", val);
    free(val);
    ps_csv_get_column(test, &val, 3);
    CuAssertStrEquals(tc, "d", val);
    free(val);
}

CuSuite*
make_csv_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, GetColumn);

    return suite;
}
