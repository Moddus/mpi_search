#include <CuTest.h>
#include <util.h>
#include <csv.h>
#include <log.h>

void
GetColumn(CuTest* tc)
{
    char test[] = "a;b;c;d";
    char* val;
    int val_len;
    ps_csv_get_column(test, &val, &val_len, 0);
    CuAssertStrEquals(tc, "a", val);
    PS_FREE(val);
    ps_csv_get_column(test, &val, &val_len, 1);
    CuAssertStrEquals(tc, "b", val);
    PS_FREE(val);
    ps_csv_get_column(test, &val, &val_len, 2);
    CuAssertStrEquals(tc, "c", val);
    PS_FREE(val);
    ps_csv_get_column(test, &val, &val_len, 3);
    CuAssertStrEquals(tc, "d", val);
    PS_FREE(val);
    ps_csv_get_column(test, &val, &val_len, PS_CSV_ALL_COL);
    CuAssertStrEquals(tc, "a;b;c;d", val);

}

CuSuite*
make_csv_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, GetColumn);

    return suite;
}
