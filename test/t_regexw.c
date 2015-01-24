#include <CuTest.h>
#include <regexw.h>
#include <pcre.h>
#include <util.h>
#include <string.h>

#include "test_all.h"

void
TestCreateAndFree(CuTest* tc)
{
    ps_regex_t* re;

    TEST_CHECK(tc, ps_regex_create(&re, "[ab]a"));

    CuAssertStrEquals(tc, "[ab]a", re->regex);
    CuAssertIntEquals(tc, FALSE, re->found);
    CuAssertIntEquals(tc, 0, re->error_offset);
    CuAssertPtrNotNull(tc, re->regex_compiled);
    CuAssertPtrNotNull(tc, re->pcre_extra);

    TEST_CHECK(tc, ps_regex_free(re));
    CuAssertPtrEquals(tc, NULL, re->regex_compiled);
    CuAssertPtrEquals(tc, NULL, re->pcre_extra);
}

void
TestFind(CuTest* tc)
{
    char test_string[] = "This contains foobar !!!";
    char test_string2[] = "This does not match.";
    ps_regex_t* re;

    ps_regex_create(&re, "foobar");
    ps_regex_find(re, test_string, strlen(test_string), 0);

    CuAssertIntEquals(tc, TRUE, re->found);

    ps_regex_find(re, test_string2, strlen(test_string2), 0);

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
