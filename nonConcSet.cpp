#include <iostream>
#include <cstdlib>

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
	
};


void node_init(Node *n) {
	n->next = nullptr;
}

Node* node_create(int key) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	
	if(newNode == NULL) return nullptr;
	
	node_init(newNode);
	newNode->key = key;
	
	return newNode;
}



struct set {
	Node* head;
};

void set_init(set_t *s) {
	s->head = nullptr;
}

set_t* set_create() {
	set_t* newSet = (set_t*)malloc(sizeof(set_t));

	if (newSet == NULL) return nullptr;

	set_init(newSet);

	return newSet;
}

	

void set_destroy(set_t *s) {

	Node* curr = s->head;
	while (curr->next != NULL) {
		Node* next = curr->next;
		free(curr);
		curr = next;	
		
	}
	free(curr);

}

int set_contains(set_t *s, int key) {
	Node* curr = s->head;
	while(curr != NULL) {
		if (curr->key == key) {
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}

/**
* @returns 1 on success, 0 on fail
*
*/
int set_insert(set_t *s, int key) {
	//TODO if set contains key exit

	if (set_contains(s, key)) {
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
		newNode->next = s->head;
		s->head = newNode;
		return 1;
	}

	Node* curr = s->head;
	Node* prev = s->head;

	// std::cout << "new:" << newNode->key << std::endl;
	while(curr != NULL) {
		// std::cout << "prev:" << prev->key << std::endl;
		// std::cout << "curr:" << curr->key << std::endl;
		if (curr->key >= newNode->key) {
			newNode->next = curr;
			prev->next = newNode;
			return 1;
		}

		prev = curr;
		curr = curr->next;
	}
	prev->next = newNode;
	return 1;

}


int set_remove(set_t *s, int key) {
	if (!set_contains(s, key)) return 0;
	
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



void set_print(set_t *s) {
	Node* curr = s->head;
	while(curr != NULL) {
		std::cout << curr->key << " ";
		curr = curr->next;
	}
	std::cout << std::endl;
}


