#include "CUnit/Basic.h"
#include <unistd.h>

void test1() {
	printf("Soy el test 1!, y pruebo que 2 sea igual a 1+1");
	CU_ASSERT_EQUAL(1+1, 2);
}

void test2() {
	printf("Soy el test 2!, y doy segmentation fault");
	char* ptr = NULL;
	*ptr = 9;
}

void test3() {
	printf("Soy el test 3!");
}

int main() {
  CU_initialize_registry();

  CU_pSuite prueba = CU_add_suite("Suite de prueba", NULL, NULL);
  CU_add_test(prueba, "uno", test1);
  CU_add_test(prueba, "dos", test2);
  CU_add_test(prueba, "tres", test3);

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
 
  return CU_get_error();
}

