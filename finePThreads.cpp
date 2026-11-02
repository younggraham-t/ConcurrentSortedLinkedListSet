#include <iostream>
#include <cstdlib>
#include <pthread.h>

#include "set.h"

/**
 * Non-Thread Safe Sorted Linked List Set
 *
 * @author Graham Young
 *
 *
 */


struct Node {

	int key;
	Node* next;
	pthread_mutex_t lock;
	
};


static void node_init(Node *n) {
	n->next = nullptr;
	pthread_mutex_init(&n->lock, NULL);
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
};

void fine_pth_init(set_t *s) {
	s->head = nullptr;
}

set_t* fine_pth_create() {
	set_t* newSet = (set_t*)malloc(sizeof(set_t));

	if (newSet == NULL) return nullptr;

	fine_pth_init(newSet);

	return newSet;
}

	

void fine_pth_destroy(set_t *s) {

	if (s == NULL ) return;
	Node* curr = s->head;
	while (curr != NULL) {
		Node* next = curr->next;
		pthread_mutex_destroy(&curr->lock);
		free(curr);
		curr = next;	
		
	}
	s->head = nullptr;

}

int fine_pth_contains(set_t *s, int key) {
	Node* curr = s->head;
	while(curr != NULL) {
		pthread_mutex_lock(&curr->lock);
		if (curr->key == key) {
			pthread_mutex_unlock(&curr->lock);
			return 1;
		}
		
		pthread_mutex_unlock(&curr->lock);
		curr = curr->next;
	}
	return 0;
}

/**
* @returns 1 on success, 0 on fail
*
*/
int fine_pth_insert(set_t *s, int key) {

	if (fine_pth_contains(s, key)) {
		return 0;
	} 

	Node* newNode = node_create(key);

	if (newNode == NULL) {
		perror("malloc");
		return 0;
	}
	
	

	//only 1 element
	if (s->head == NULL) {
		s->head = newNode;
		return 1;
	}
	
	//if the new element goes at the head
	if (s->head->key > newNode->key) {
		Node* oldHead = s->head;
		pthread_mutex_lock(&oldHead->lock);
		newNode->next = s->head;
		s->head = newNode;
		pthread_mutex_unlock(&oldHead->lock);
		return 1;
	}

	Node* curr = s->head;
	Node* prev = s->head;

	// std::cout << "new:" << newNode->key << std::endl;
	while(curr != NULL) {
		// std::cout << "prev:" << prev->key << std::endl;
		// std::cout << "curr:" << curr->key << std::endl;

		pthread_mutex_lock(&curr->lock);
		pthread_mutex_lock(&prev->lock);
		
		if (curr->key >= newNode->key) {
			newNode->next = curr;
			prev->next = newNode;
			pthread_mutex_unlock(&prev->lock);
			pthread_mutex_unlock(&curr->lock);
			return 1;
		}

		pthread_mutex_unlock(&prev->lock);
		pthread_mutex_unlock(&curr->lock);
		prev = curr;
		curr = curr->next;
	}
	prev->next = newNode;
	return 1;

}


int fine_pth_remove(set_t *s, int key) {
	if (!fine_pth_contains(s, key)) return 0;
	
	if (s->head->key == key) {
		if (s->head->next != NULL) {
			s->head = s->head->next;
		}
		else {
			s->head = NULL;
		}
	}

	Node* curr = s->head;
	Node* prev = s->head;
	while (curr != NULL) {
		if (curr->key == key) {
			prev->next = curr->next;
			free(curr);
			return 1;
		}
		prev = curr;
		curr = curr->next;

	}
	return 1;
}



void fine_pth_print(set_t *s) {
	Node* curr = s->head;
	while(curr != NULL) {
		std::cout << curr->key << " ";
		curr = curr->next;
	}
	std::cout << std::endl;
}


node_snapshot_t fine__pth_get_head_snapshot(set_t* s) {
    if (!s || !s->head) return {0, nullptr};
    return {s->head->key, (void*)s->head};
}

node_snapshot_t fine__pth_get_node_snapshot(void* node_ptr) {
    Node* n = (Node*)node_ptr;
    if (!n || !n->next) return {n->key, nullptr};
    return {n->key, (void*)n->next};
}
