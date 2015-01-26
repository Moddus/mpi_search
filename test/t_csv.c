#include <CuTest.h>
#include <util.h>
#include <csv.h>
#include <log.h>
#include <string.h>

void
GetColumn(CuTest* tc)
{
    char test[] = "a;b;c;d";
    int test_len = strlen(test) + 1;
    char* val;
    int val_len;

    ps_csv_get_column(test, test_len, &val, &val_len, 0);
    CuAssertStrEquals(tc, "a", val);
    CuAssertIntEquals(tc, 1 , val_len);
    PS_FREE(val);
    ps_csv_get_column(test, test_len, &val, &val_len, 1);
    CuAssertStrEquals(tc, "b", val);
    CuAssertIntEquals(tc, 1 , val_len);
    PS_FREE(val);
    ps_csv_get_column(test, test_len, &val, &val_len, 2);
    CuAssertStrEquals(tc, "c", val);
    CuAssertIntEquals(tc, 1 , val_len);
    PS_FREE(val);
    ps_csv_get_column(test, test_len, &val, &val_len, 3);
    CuAssertStrEquals(tc, "d", val);
    CuAssertIntEquals(tc, 1 , val_len);
    PS_FREE(val);
    ps_csv_get_column(test, test_len, &val, &val_len, PS_CSV_ALL_COL);
    CuAssertStrEquals(tc, "a;b;c;d", val);
    CuAssertIntEquals(tc, 7 , val_len);

    char test2[] = "a;b;c;d\ne;f;g;h\n";
    ps_csv_get_column(test2, 8, &val, &val_len, 6);
    CuAssertStrEquals(tc, NULL, val);
    CuAssertIntEquals(tc, -1 , val_len);
}

CuSuite*
make_csv_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, GetColumn);

    return suite;
}
