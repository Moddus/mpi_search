#include <CuTest.h>
#include <string.h>

#include <log.h>
#include <util.h>
#include <file_searcher.h>
#include <mpi_functions.h>

#include "test_all.h"

void
CreateAndFree(CuTest* tc)
{
    char* file_path = "";
    ps_searcher_t* searcher = NULL;
    ps_search_task_t* task = NULL;

    TEST_CHECK(tc, ps_searcher_task_create(&task,
                                            0,
                                            0,
                                            strlen(file_path),
                                            file_path));

    CuAssertPtrNotNull(tc, task);

    TEST_CHECK(tc, ps_file_searcher_create(&searcher,
                                            "foobar",
                                            task));

    CuAssertPtrNotNull(tc, searcher);
    CuAssertStrEquals(tc, "foobar", searcher->regex->regex);
    CuAssertStrEquals(tc, file_path, searcher->task->filename);

    TEST_CHECK(tc, ps_file_searcher_free(&searcher));
    CuAssertPtrEquals(tc, NULL, searcher);
}

CuSuite*
make_file_searcher_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, CreateAndFree);

    return suite;
}