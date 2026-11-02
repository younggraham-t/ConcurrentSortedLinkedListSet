
#ifndef SORTED_LINKED_LIST_SET_H
#define SORTED_LINKED_LIST_SET_H


#include <string_view>

struct set;

typedef struct set set_t;




enum ImplType {
	FINE_PTH,
	COARSE_PTH,
	FINE_OMP,
	COARSE_OMP,
	NON_CONC
	
};

constexpr std::string_view to_string(ImplType t) {
    switch (t) {
        case ImplType::FINE_PTH:   return "fine_pth";
		case ImplType::COARSE_PTH: return "coarse_pth";
		case ImplType::FINE_OMP:  return "fine_omp";
		case ImplType::COARSE_OMP: return "coarse_omp";
        default:           return "Unknown";
    }
}

struct node_snapshot_t {
    int key;
    void* next;
};

struct set_controller_t {
	set_t* (*create)();
	void (*init)(set_t *s);
	int (*insert)(set_t *s, int key);
	int (*remove)(set_t *s, int key);
	int (*contains)(set_t *s, int key);
	void (*destroy)(set_t *s);
	void (*print)(set_t *s);


	node_snapshot_t (*get_head_snapshot)(set_t* s);
    node_snapshot_t (*get_node_snapshot)(void* node_ptr);
};

set_controller_t get_set_controller(ImplType type);
int verify_set_integrity(set_controller_t c, set_t *s, int expected_count);

#endif
