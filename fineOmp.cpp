#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <climits>

#include "set.h"

/**
 * Thread Safe Sorted Linked List Set
 *
 * Fine Grained implementation using pthreads
 *
 * @author Graham Young
 *
 *
 */


struct Node {

	int key;
	Node* next;
	omp_lock_t lock;
	
};


static void node_init(Node *n) {
	n->next = nullptr;
	omp_init_lock(&n->lock);
}

static Node* node_create(int key) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	
	if(newNode == NULL) return nullptr;
	
	node_init(newNode);
	newNode->key = key;
	
	return newNode;
}

static void node_destroy(Node *n) {
	if (n == NULL) return;
	omp_destroy_lock(&n->lock);

}



struct set {
	Node* head;
};

void fine_omp_init(set_t *s) {
	Node* dummyNode = node_create(INT_MIN);
	s->head = dummyNode;
}

set_t* fine_omp_create() {
	set_t* newSet = (set_t*)malloc(sizeof(set_t));

	if (newSet == NULL) return nullptr;
	fine_omp_init(newSet);

	return newSet;
}

	

void fine_omp_destroy(set_t *s) {

	if (s == NULL ) return;
	Node* curr = s->head;
	while (curr != NULL) {
		Node* next = curr->next;
		node_destroy(curr);
		free(curr);
		curr = next;	
		
	}
	s->head = nullptr;

}

int fine_omp_contains(set_t *s, int key) {
	Node* prev = s->head;
	omp_set_lock(&prev->lock);
	Node* curr = prev->next;
	while(curr != NULL) {
		omp_set_lock(&curr->lock);
		if (curr->key == key) {
			omp_unset_lock(&prev->lock);
			omp_unset_lock(&curr->lock);
			return 1;
		}
		else if (curr->key > key) {
			omp_unset_lock(&prev->lock);
			omp_unset_lock(&curr->lock);
			return 0;
		}
		
		omp_unset_lock(&prev->lock);
		prev = curr;
		curr = curr->next;
	}
	omp_unset_lock(&prev->lock);
	return 0;
}

/**
* @returns 1 on success, 0 on fail
*
*/
int fine_omp_insert(set_t *s, int key) {
	Node* newNode = node_create(key);

	if (newNode == NULL) {
		perror("malloc");
		return 0;
	}
	

	Node* prev = s->head;
	omp_set_lock(&prev->lock);
	Node* curr = prev->next;

	// std::cout << "new:" << newNode->key << std::endl;
	while(curr != NULL) {
		// std::cout << "prev:" << prev->key << std::endl;
		// std::cout << "curr:" << curr->key << std::endl;
		
		omp_set_lock(&curr->lock);

		if (curr->key > newNode->key) {
			newNode->next = curr;
			// this is the case if curr is the head
			if (prev == NULL) {
				s->head = newNode;
			}
			else {
				prev->next = newNode;
			}
			omp_unset_lock(&prev->lock);
			omp_unset_lock(&curr->lock);
			return 1;
		}
		else if (curr->key == newNode->key) {
			omp_unset_lock(&prev->lock);
			omp_unset_lock(&curr->lock);
			return 0;
		}

		omp_unset_lock(&prev->lock);
		prev = curr;
		curr = curr->next;
		//should be the same as the old curr lock
	}
	prev->next = newNode;
	omp_unset_lock(&prev->lock);
	return 1;

}


int fine_omp_remove(set_t *s, int key) {
	Node* prev = s->head;
	omp_set_lock(&prev->lock);
	Node* curr = prev->next;
	while (curr != NULL) {
		omp_set_lock(&curr->lock);
		if (curr->key == key) {
			prev->next = curr->next;
			omp_unset_lock(&curr->lock);
			omp_unset_lock(&prev->lock);
			node_destroy(curr);
			free(curr);
			return 1;
		}
		else if (curr->key > key) {
			omp_unset_lock(&curr->lock);
			omp_unset_lock(&prev->lock);
			return 0;
		}
		omp_unset_lock(&prev->lock);
		prev = curr;
		curr = curr->next;

	}
	omp_unset_lock(&prev->lock);
	return 0;
}



void fine_omp_print(set_t *s) {
	Node* curr = s->head;
	while(curr != NULL) {
		std::cout << curr->key << " ";
		curr = curr->next;
	}
	std::cout << std::endl;
}


node_snapshot_t fine_omp_get_head_snapshot(set_t* s) {
	//since we use a dummy node we are treating head->next as the "head" for testing purposes
    if (!s || !s->head->next) return {0, nullptr};
    if (!s->head->next->next) return {s->head->next->key, nullptr};
    return {s->head->next->key, (void*)s->head->next->next};
}

node_snapshot_t fine_omp_get_node_snapshot(void* node_ptr) {
    Node* n = (Node*)node_ptr;
    if (!n || !n->next) return {n->key, nullptr};
    return {n->key, (void*)n->next};
}
