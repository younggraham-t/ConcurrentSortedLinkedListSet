#include <iostream>
#include <cstdlib>
#include <omp.h>

#include "set.h"

/**
 * Thread Safe Sorted Linked List Set
 *
 * Coarse Grained implementation using OpenMP
 *
 * @author Graham Young
 *
 *
 */


struct Node {

	int key;
	Node* next;
	
};


static void node_init(Node *n) {
	n->next = nullptr;
}

static Node* node_create(int key) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	
	if(newNode == NULL) return nullptr;
	
	node_init(newNode);
	newNode->key = key;
	
	return newNode;
}



struct set {
	Node* head;
	omp_lock_t lock;
};

void coarse_omp_init(set_t *s) {
	s->head = nullptr;
	omp_init_lock(&s->lock);
}

set_t* coarse_omp_create() {
	set_t* newSet = (set_t*)malloc(sizeof(set_t));

	if (newSet == NULL) return nullptr;

	coarse_omp_init(newSet);

	return newSet;
}

	

void coarse_omp_destroy(set_t *s) {

	if (s == NULL ) return;
	Node* curr = s->head;
	while (curr != NULL) {
		Node* next = curr->next;
		free(curr);
		curr = next;	
		
	}
	s->head = nullptr;
	omp_destroy_lock(&s->lock);

}

int coarse_omp_contains(set_t *s, int key) {
	omp_set_lock(&s->lock);
	Node* curr = s->head;
	while(curr != NULL) {
		if (curr->key == key) {
			omp_unset_lock(&s->lock);
			return 1;
		}
		curr = curr->next;
	}
	omp_unset_lock(&s->lock);
	return 0;
}

/**
* @returns 1 on success, 0 on fail
*
*/
int coarse_omp_insert(set_t *s, int key) {
	omp_set_lock(&s->lock);
	Node* newNode = node_create(key);

	if (newNode == NULL) {
		perror("malloc");
		omp_unset_lock(&s->lock);
		return 0;
	}
	
	

	//only 1 element
	if (s->head == NULL) {
		s->head = newNode;
		omp_unset_lock(&s->lock);
		return 1;
	}
	
	//if the new element goes at the head
	if (s->head->key > newNode->key) {
		newNode->next = s->head;
		s->head = newNode;
		omp_unset_lock(&s->lock);
		return 1;
	}

	Node* curr = s->head;
	Node* prev = s->head;

	// std::cout << "new:" << newNode->key << std::endl;
	while(curr != NULL) {
		// std::cout << "prev:" << prev->key << std::endl;
		// std::cout << "curr:" << curr->key << std::endl;
		if (curr->key > newNode->key) {
			newNode->next = curr;
			prev->next = newNode;
			omp_unset_lock(&s->lock);
			return 1;
		}
		else if (curr->key == newNode->key) {
			omp_unset_lock(&s->lock);
			return 0;

		}

		prev = curr;
		curr = curr->next;
	}
	prev->next = newNode;
	omp_unset_lock(&s->lock);
	return 1;

}


int coarse_omp_remove(set_t *s, int key) {
	omp_set_lock(&s->lock);
	if (s->head == NULL) {
		omp_unset_lock(&s->lock);
		return 0;
	}
	// key is at head 
	if (s->head->key == key) {
		if (s->head->next != NULL) {
			Node* old = s->head;
			s->head = s->head->next;
			free(old);


		}
		else {
			s->head = NULL;

		}
		omp_unset_lock(&s->lock);
		return 1;
	}

	Node* curr = s->head;
	Node* prev = s->head;
	while (curr != NULL) {
		if (curr->key == key) {
			prev->next = curr->next;
			free(curr);
			omp_unset_lock(&s->lock);
			return 1;
		}
		prev = curr;
		curr = curr->next;

	}
	omp_unset_lock(&s->lock);
	return 0;
}



void coarse_omp_print(set_t *s) {
	Node* curr = s->head;
	while(curr != NULL) {
		std::cout << curr->key << " ";
		curr = curr->next;
	}
	std::cout << std::endl;
}

node_snapshot_t coarse_omp_get_head_snapshot(set_t* s) {
    if (!s || !s->head) return {0, nullptr};
    if (!s->head->next) return {s->head->key, nullptr};
    return {s->head->key, (void*)s->head->next};
}

node_snapshot_t coarse_omp_get_node_snapshot(void* node_ptr) {
    Node* n = (Node*)node_ptr;
    if (!n || !n->next) return {n->key, nullptr};
    return {n->key, (void*)n->next};
}

