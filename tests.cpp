#include "set.h"
#include "tests.h"
#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <string_view>

static int TEST_THREADS = 4;

void run_test_suite(ImplType impl) {
	std::cout << "TEST SUITE: " << to_string(impl) << std::endl;
	set_controller_t c = get_set_controller(impl);
	test_single_thread(c);
	test_set_and_linked_list(c);
	test_race_conditions(c);
}


void test_single_thread(set_controller_t c) {
	set_t* test_set = c.create();
	omp_set_num_threads(1);
	bool test_passed = true;
	for (int i = 0; i <= 1000; i++) {
		c.insert(test_set, i);
	}

	for (int i = 0; i <= 1000; i++) {
		if (!c.contains(test_set, i)) {
			std::cout << "TEST FAIL: Value " << i << " not in set (should be)" << std::endl;
			test_passed = false;
		}
	}
	
	for (int i = 0; i < 500; i++) {
		c.remove(test_set, i);
	}

	for (int i = 500; i <= 1000; i++) {
		if (!c.contains(test_set, i)) {
			std::cout << "TEST FAIL: Value " << i << " missing after partial deletion" << std::endl;
			test_passed = false;
		}
	}

	for (int i = 0; i < 500; i++) {
		if (c.contains(test_set, i)) {
			std::cout << "TEST FAIL: Value " << i << " still in set (shouldn't be)" << std::endl;
			test_passed = false;
		}
	}
	
	if (test_passed) {
        std::cout << "SUCCESS: Single-threaded verification test passed cleanly!\n";
    }

	c.destroy(test_set);
	free(test_set);
}


void test_set_and_linked_list(set_controller_t c) {
	set_t* test_set = c.create();
	omp_set_num_threads(TEST_THREADS);
	bool test_passed = true;

	#pragma omp parallel 
	{
		//insert values from 1-1000 reversed
		#pragma omp for
		for (int i = 1000; i>0; i--) {
			c.insert(test_set, i);
		}

		#pragma omp barrier
		
		//check if values are sorted correctly
		#pragma omp single 
		{
			// c.print(test_set);
			if (!verify_set_integrity(c, test_set, 1000)) {
				test_passed = false;
			}
		}

		//insert the same values again
		#pragma omp for
		for (int i = 1000; i>0; i--) {
			int res = c.insert(test_set, i);
			if (res == 1) {
				#pragma omp critical
				std::cout << "TEST FAILED: Duplicate value: " << i << " inserted" << std::endl;

				test_passed = false;
			}
		}

		#pragma omp barrier

		//reverify values are sorted correctly 
		#pragma omp single 
		{
			// c.print(test_set);
			if (!verify_set_integrity(c, test_set, 1000)) {
				test_passed = false;
			}
		}
	}

	
	if (test_passed) {
        std::cout << "SUCCESS: Set order verification test passed cleanly!\n";
    }

	
	c.destroy(test_set);
	free(test_set);
}


void test_race_conditions(set_controller_t c) {
	set_t* test_set = c.create();
	omp_set_num_threads(TEST_THREADS);
	bool test_passed = true;

	#pragma omp parallel 
	{
		//insert values from 0-49
		#pragma omp for 
		for (int i = 0; i < 50; i++) {
			c.insert(test_set, i);
		}
		
		#pragma omp barrier

		//for 2000 operations either insert, remove, or lookup keys in the range of 0-49 repeatedly
		// the goal is to try to modify or lookup the same key simultaneously
		#pragma omp for
		for (int i = 0; i < 10000; i++) {
			if (i%3 == 0) c.remove(test_set, i % 50);
			else if (i%3 == 1) c.insert(test_set, i % 50);
			else if (i%3 == 2) c.contains(test_set, i % 50);
		}
		
		#pragma omp barrier
		
		#pragma omp single 
		{
			// c.print(test_set);
			if (!verify_set_integrity(c, test_set, 50)) {
				test_passed = false;
			}
		}

	}
	
	
	
	if (test_passed) {
        std::cout << "SUCCESS: Race condition test passed cleanly!\n";
    }

	
	c.destroy(test_set);
	free(test_set);
}
