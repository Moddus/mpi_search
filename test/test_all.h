#ifndef TEST_ALL_H
#define TEST_ALL_H

#include <CuTest.h>
#include <util.h>

#define TEST_CHECK(tc, rv) CuAssertIntEquals(tc, PS_SUCCESS, rv);

#endif
