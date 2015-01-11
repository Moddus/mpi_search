#include <CuTest.h>
#include <regexw.h>
#include <pcre.h>

void
TestCreateAndFree(CuTest *tc)
{
    ps_regex_t* re;

    ps_regex_create(&re, "[ab]a");

    CuAssertStrEquals(tc, "[ab]a", re->regex);
    CuAssertPtrNotNull(tc, re->regex_compiled);
}

CuSuite*
make_regex_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestCreateAndFree);

    return suite;
}
