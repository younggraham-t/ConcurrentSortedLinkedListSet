#include "set.h"
#include <iostream>


// Non-concurrent
set_t* set_create();
void set_init(set_t *s);
int set_insert(set_t *s, int key);
int set_remove(set_t *s, int key);
int set_contains(set_t *s, int key);
void set_destroy(set_t *s);
void set_print(set_t *s);

// Coarse Pthreads
set_t* coarse_pth_create();
int coarse_pth_insert(set_t *s, int key);
int coarse_pth_remove(set_t *s, int key);
int coarse_pth_contains(set_t *s, int key);
void coarse_pth_destroy(set_t *s);
void coarse_pth_print(set_t *s);
node_snapshot_t coarse_pth_get_head_snapshot(set_t* s);
node_snapshot_t coarse_pth_get_node_snapshot(void* node_ptr);

// Fine Pthreads
set_t* fine_pth_create();
int fine_pth_insert(set_t *s, int key);
int fine_pth_remove(set_t *s, int key);
int fine_pth_contains(set_t *s, int key);
void fine_pth_destroy(set_t *s);
void fine_pth_print(set_t *s);
node_snapshot_t fine_pth_get_head_snapshot(set_t* s);
node_snapshot_t fine_pth_get_node_snapshot(void* node_ptr);

// Coarse OMP
set_t* coarse_omp_create();
int coarse_omp_insert(set_t *s, int key);
int coarse_omp_remove(set_t *s, int key);
int coarse_omp_contains(set_t *s, int key);
void coarse_omp_destroy(set_t *s);
void coarse_omp_print(set_t *s);
node_snapshot_t coarse_omp_get_head_snapshot(set_t* s);
node_snapshot_t coarse_omp_get_node_snapshot(void* node_ptr);


// Fine Pthreads
set_t* fine_omp_create();
int fine_omp_insert(set_t *s, int key);
int fine_omp_remove(set_t *s, int key);
int fine_omp_contains(set_t *s, int key);
void fine_omp_destroy(set_t *s);
void fine_omp_print(set_t *s);
node_snapshot_t fine_omp_get_head_snapshot(set_t* s);
node_snapshot_t fine_omp_get_node_snapshot(void* node_ptr);

#define ASSIGN_CONTROLLER_FUNCTIONS(prefix) \
    controller.create           = prefix##_create; \
    controller.insert           = prefix##_insert; \
    controller.remove           = prefix##_remove; \
    controller.contains         = prefix##_contains; \
    controller.destroy          = prefix##_destroy; \
    controller.print            = prefix##_print; \
    controller.get_head_snapshot = prefix##_get_head_snapshot; \
    controller.get_node_snapshot = prefix##_get_node_snapshot;


set_controller_t get_set_controller(ImplType impl) {
	set_controller_t controller;
	
	switch (impl) {
		case NON_CONC:
			controller.create = set_create;
			controller.insert = set_insert;
			controller.remove = set_remove;
			controller.contains = set_contains;
			controller.destroy = set_destroy;
			controller.print = set_print;
			break;
		case COARSE_PTH:
			ASSIGN_CONTROLLER_FUNCTIONS(coarse_pth);
			break;
		case FINE_PTH:
			ASSIGN_CONTROLLER_FUNCTIONS(fine_pth);
			break;
		case COARSE_OMP:
			ASSIGN_CONTROLLER_FUNCTIONS(coarse_omp);
			break;
		case FINE_OMP:
			ASSIGN_CONTROLLER_FUNCTIONS(fine_omp);
			break;
			
	}
	return controller;
}



#undef ASSIGN_CONTROLLER_FUNCTIONS


int verify_set_integrity(set_controller_t c, set_t* s, int expected_max_count) {
    int actual_count = 0;
    
    node_snapshot_t curr = c.get_head_snapshot(s);
    
    while (curr.next != nullptr) {
        actual_count++;
        

        node_snapshot_t next_node = c.get_node_snapshot(curr.next);
		// std::cout << curr.key << std::endl;
		// std::cout << next_node.key << std::endl;
        
        if (curr.key >= next_node.key) {
            std::cout << "INTEGRITY ERROR: Out of sorted order at key " << curr.key << std::endl;;
			std::cout << "Next Key: " << next_node.key << std::endl;
            return 0;
        }
        
        curr = next_node;
    }
    
    if (actual_count > 0 || c.get_head_snapshot(s).next != nullptr) {
        actual_count++; 
    }

    if (actual_count > expected_max_count) {
        std::cout << "INTEGRITY ERROR: Expected no more than " << expected_max_count 
                  << " nodes, but found " << actual_count << std::endl;
        return 0;
    }

    return 1; // Success!
}

