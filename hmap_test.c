#include "hmap.h"

#include <CUnit/Basic.h>

size_t sz = 6;

int
init_suite() {
    return 0;
}

int
clean_suite() {
    return 0;
}

void
test_hmap_set() {
    hmap_t *h = hmap_new(sz);
    hmap_set(h, "k1", "data k1");

    char* r = hmap_get(h, "k1");

    CU_ASSERT_STRING_EQUAL((char*)r, "data k1");
    CU_ASSERT_EQUAL(h->size, sz);
    CU_ASSERT_EQUAL(h->filled, 1);

    hmap_set(h, "k2", "data k2");
    hmap_set(h, "k3", "data k3");
    CU_ASSERT_EQUAL(h->filled, 3);

    r = hmap_get(h, "k3");
    CU_ASSERT_STRING_EQUAL((char*)r, "data k3");

    hmap_free(h);
}

void 
test_hmap_unset() {
    hmap_t *h = hmap_new(sz);

    hmap_set(h, "k1", "data k1");
    hmap_set(h, "k2", "data k2");
    hmap_set(h, "k3", "data k3");

    hmap_unset(h, "k2");
    hmap_unset(h, "k1");
    hmap_unset(h, "k3");
    char* r = hmap_get(h, "k2");

    CU_ASSERT_PTR_NULL(r);
    CU_ASSERT_EQUAL(h->size, sz);
    CU_ASSERT_EQUAL(h->filled, 0);

    hmap_free(h);
}

void
test_hmap_iter() {
    hmap_t *h = hmap_new(sz);
    hmap_set(h, "k1", "data k1");
    hmap_set(h, "k2", "data k2");
    hmap_set(h, "k3", "data k3");

    hmap_iter_t* it = hmap_iter_new(h);
    int looped = 0;
    for (hmap_node_t* n = hmap_iter_next(it); n != NULL; n = hmap_iter_next(it), ++looped) {
        CU_ASSERT_PTR_NOT_NULL(n);
    }
    free(it);

    hmap_free(h);
    CU_ASSERT_EQUAL(looped, 3);
}

int main()
{
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry()) {
      return CU_get_error();
    }

   /* add a suite to the registry */
    CU_pSuite pSuite = CU_add_suite("Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
           (NULL == CU_add_test(pSuite, "hmap_set", test_hmap_set))
        || (NULL == CU_add_test(pSuite, "hmap_unset", test_hmap_unset))
        || (NULL == CU_add_test(pSuite, "hmap_iter", test_hmap_iter))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
