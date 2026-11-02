#include "set.h"
#include <iostream>


// non concurent
set_t* set_create();
void set_init(set_t *s);
int set_insert(set_t *s, int key);
int set_remove(set_t *s, int key);
int set_contains(set_t *s, int key);
void set_destroy(set_t *s);
void set_print(set_t *s);

// coarse pthreads
set_t* coarse_pth_create();
void coarse_pth_init(set_t *s);
int coarse_pth_insert(set_t *s, int key);
int coarse_pth_remove(set_t *s, int key);
int coarse_pth_contains(set_t *s, int key);
void coarse_pth_destroy(set_t *s);
void coarse_pth_print(set_t *s);
node_snapshot_t coarse__pth_get_head_snapshot(set_t* s);
node_snapshot_t coarse__pth_get_node_snapshot(void* node_ptr);

set_t* fine_pth_create();
void fine_pth_init(set_t *s);
int fine_pth_insert(set_t *s, int key);
int fine_pth_remove(set_t *s, int key);
int fine_pth_contains(set_t *s, int key);
void fine_pth_destroy(set_t *s);
void fine_pth_print(set_t *s);
node_snapshot_t fine__pth_get_head_snapshot(set_t* s);
node_snapshot_t fine__pth_get_node_snapshot(void* node_ptr);

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
			controller.create = coarse_pth_create;
			controller.insert = coarse_pth_insert;
			controller.remove = coarse_pth_remove;
			controller.contains = coarse_pth_contains;
			controller.destroy = coarse_pth_destroy;
			controller.print = coarse_pth_print;
			controller.get_head_snapshot = coarse__pth_get_head_snapshot;
			controller.get_node_snapshot = coarse__pth_get_node_snapshot;
			break;
		case FINE_PTH:
			controller.create = fine_pth_create;
			controller.insert = fine_pth_insert;
			controller.remove = fine_pth_remove;
			controller.contains = fine_pth_contains;
			controller.destroy = fine_pth_destroy;
			controller.print = fine_pth_print;
			controller.get_head_snapshot = fine__pth_get_head_snapshot;
			controller.get_node_snapshot = fine__pth_get_node_snapshot;
			break;
			
	}
	return controller;
}

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

