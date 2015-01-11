#include <CuTest.h>
#include <regexw.h>
#include <pcre.h>
#include <util.h>

void
TestCreateAndFree(CuTest* tc)
{
    ps_regex_t* re;

    ps_regex_create(&re, "[ab]a");

    CuAssertStrEquals(tc, "[ab]a", re->regex);
    CuAssertIntEquals(tc, FALSE, re->found);
    CuAssertIntEquals(tc, 0, re->error_offset);
    CuAssertPtrNotNull(tc, re->regex_compiled);
    CuAssertPtrNotNull(tc, re->pcre_extra);

    ps_regex_free(re);
    CuAssertPtrEquals(tc, NULL, re->regex_compiled);
    CuAssertPtrEquals(tc, NULL, re->pcre_extra);
}

void
TestFind(CuTest* tc)
{
    ps_regex_t* re;

    ps_regex_create(&re, "foobar");
    ps_regex_find(re, "This contains foobar !!!", 0);

    CuAssertIntEquals(tc, TRUE, re->found);

    ps_regex_find(re, "This does not match.", 0);

    CuAssertIntEquals(tc, FALSE, re->found);
}

CuSuite*
make_regex_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestCreateAndFree);
    SUITE_ADD_TEST(suite, TestFind);

    return suite;
}
