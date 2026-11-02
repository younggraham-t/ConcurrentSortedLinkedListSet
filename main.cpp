#include "set.h"
#include "tests.h"
#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <string_view>
#include <chrono>

static unsigned int BASE_SEED = 42;

struct Options {
	int numThreads;
	int operations;
	int insert;
	int remove;
	int contains;
	int range;
	ImplType impl;
	
	friend std::ostream& operator<<(std::ostream& os, const Options& o) {
		return os << "Options{numThreads: " << o.numThreads
				  << ", operations: " << o.operations
				  << ", insert: " << o.insert
				  << ", remove: " << o.remove
				  << ", contains: " << o.contains
				  << ", range: " << o.range
				  << ", implemtation: " << to_string(o.impl)
				  << "}";
    }
};


void run_workload(set_controller_t controller, Options opts, unsigned int base_seed);


int main(int argc, char* argv[]) {
	
	Options opts;
	
	for (int i = 1; i < argc; i++) {
		std::string_view arg(argv[i]);
		
		if (arg == "--threads") {
			if (i+1 <argc) {
				try {
					opts.numThreads = std::stoi(argv[++i]); // Throws if invalid or out of range
				} catch (const std::invalid_argument& e) {
					std::cerr << "Invalid input: " << e.what() << '\n';
				} catch (const std::out_of_range& e) {
					std::cerr << "Value out of range: " << e.what() << '\n';
				}

			} 
			else {
				std::cerr << "Error: --threads requires an argument";
				return 1;

			}
		}
		else if (arg == "--ops") {
			if (i+1 <argc) {
				try {
					opts.operations = std::stoi(argv[++i]); // Throws if invalid or out of range
				} catch (const std::invalid_argument& e) {
					std::cerr << "Invalid input: " << e.what() << '\n';
				} catch (const std::out_of_range& e) {
					std::cerr << "Value out of range: " << e.what() << '\n';
				}

			} 
			else {
				std::cerr << "Error: --ops requires an argument";
				return 1;

			}
		}
		else if (arg == "--insert") {
			if (i+1 <argc) {
				try {
					opts.insert = std::stoi(argv[++i]); // Throws if invalid or out of range
				} catch (const std::invalid_argument& e) {
					std::cerr << "Invalid input: " << e.what() << '\n';
				} catch (const std::out_of_range& e) {
					std::cerr << "Value out of range: " << e.what() << '\n';
				}

			} 
			else {
				std::cerr << "Error: --insert requires an argument";
				return 1;

			}
		}
		else if (arg == "--remove") {
			if (i+1 <argc) {
				try {
					opts.remove = std::stoi(argv[++i]); // Throws if invalid or out of range
				} catch (const std::invalid_argument& e) {
					std::cerr << "Invalid input: " << e.what() << '\n';
				} catch (const std::out_of_range& e) {
					std::cerr << "Value out of range: " << e.what() << '\n';
				}

			} 
			else {
				std::cerr << "Error: --remove requires an argument";
				return 1;

			}
		}
		else if (arg == "--contains") {
			if (i+1 <argc) {
				try {
					opts.contains = std::stoi(argv[++i]); // Throws if invalid or out of range
				} catch (const std::invalid_argument& e) {
					std::cerr << "Invalid input: " << e.what() << '\n';
				} catch (const std::out_of_range& e) {
					std::cerr << "Value out of range: " << e.what() << '\n';
				}

			} 
			else {
				std::cerr << "Error: --contains requires an argument";
				return 1;

			}
		}
		else if (arg == "--impl") {
			if (i+1 <argc) {
				std::string impl = argv[++i];
				if (impl == "non_conc") opts.impl = NON_CONC;
				else if (impl == "fine_pth") opts.impl = FINE_PTH;
				else if (impl == "coarse_pth") opts.impl = COARSE_PTH;

			} 
			else {
				std::cerr << "Error: --impl requires an argument";
				return 1;

			}
		}
		else if (arg == "--range") {
			if (i+1 <argc) {
				try {
					opts.range = std::stoi(argv[++i]); // Throws if invalid or out of range
				} catch (const std::invalid_argument& e) {
					std::cerr << "Invalid input: " << e.what() << '\n';
				} catch (const std::out_of_range& e) {
					std::cerr << "Value out of range: " << e.what() << '\n';
				}

			} 
			else {
				std::cerr << "Error: --range requires an argument";
				return 1;

			}
		}
		else if (arg == "--test") {
			run_test_suite(ImplType::COARSE_PTH);
			run_test_suite(ImplType::FINE_PTH);
			return 0;
		}

	}
	if (opts.insert + opts.remove + opts.contains != 100) {
		std::cerr << "Error: --insert, --remove, and --contains must add to 100" << std::endl;
		return 1;
	}

	std::cout << opts << std::endl;
	
	set_controller_t controller = get_set_controller(opts.impl);

	run_workload(controller, opts, BASE_SEED);
	
	return 0;
}


void run_workload(set_controller_t controller, Options opts, unsigned int base_seed) {
	set_t* my_set = controller.create();

	omp_set_num_threads(opts.numThreads);
	double before = omp_get_wtime();

	if (opts.impl == NON_CONC) {
		unsigned int private_seed = base_seed + 1;
		for (int i = 0; i <= opts.operations; i++) {
			int op = rand_r(&private_seed) % 100;
			int key = (rand_r(&private_seed) % opts.range) + 1;


			if (op < opts.insert) {
				controller.insert(my_set, key);
			}
			else if (op < opts.insert + opts.remove) {
				controller.remove(my_set, key);
			}
			else {
				controller.contains(my_set, key);
			}
		}
		
	}
	else {
		#pragma omp parallel 
		{
			unsigned int private_seed = base_seed + omp_get_thread_num();
			#pragma omp for 
			for (int i = 0; i <= opts.operations; i++) {
				int op = rand_r(&private_seed) % 100;
				int key = (rand_r(&private_seed) % opts.range) + 1;


				if (op < opts.insert) {
					controller.insert(my_set, key);
				}
				else if (op < opts.insert + opts.remove) {
					controller.remove(my_set, key);
				}
				else {
					controller.contains(my_set, key);
				}
			}
			
		}

	}

	double after = omp_get_wtime();
	
    std::cout << "Time Elapsed (ms): " << (after - before) * 1000.0 << "\n";

	controller.destroy(my_set);
	free(my_set);
}
