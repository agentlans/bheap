/*
    Copyright 2021 Alan Tseng

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>

#include "intint.h"

struct node* node_new(int key, int val) {
	struct node* p = malloc(sizeof(struct node));
	if (!p) return NULL;
	*p = (struct node) {key, val, NULL, NULL};
	return p;
}

void node_free(struct node* v) {
	struct node* w = v;
	// Free all the nodes after this node.
	while (w) {
		struct node* next = w->next;
		if (w->prev) free(w);
		w = next;
	}
}

struct node* node_find(struct node* nod, int key) {
	if (key < 0) return NULL;
	if (!nod) return NULL;
	struct node* v = nod;
	while (v) {
		if (v->key == key) return v;
		v = v->next;
	}
	return NULL;
}

void node_link(struct node* nod1, struct node* nod2) {
	if (nod1) nod1->next = nod2;
	if (nod2) nod2->prev = nod1;
}

// Removes a node from its neighbours
void node_remove(struct node* nod) {
	// Make the previous and next nodes point to each other
	struct node* prev = nod->prev;
	struct node* next = nod->next;
	if (prev) prev->next = next;
	if (next) next->prev = prev;
	// Free the node itself
	free(nod);
}


void intint_free(struct intint* ii) {
	for (int i = 0; i < ii->buckets; ++i) {
		node_free(ii->arr + i);
	}
	if (ii) free(ii->arr);
	free(ii);
}

struct intint* intint_new(int num_buckets) {
	struct intint* p = malloc(sizeof(struct intint));
	if (!p) return NULL;
	// Allocate enough buckets.
	struct node* arr = malloc(num_buckets * sizeof(struct node));
	if (!arr) {
		free(p);
		return NULL;
	}
	for (int i = 0; i < num_buckets; ++i) {
		arr[i] = (struct node) {0, 0, NULL, NULL};
	}
	// Set up the integer-integer map
	*p = (struct intint) {arr, num_buckets};
	return p;
}

// Inserts new key and value into the map.
// Caution: doesn't check for repeated keys!
void intint_insert(struct intint* ii, int key, int val) {
	int bucket = key % ii->buckets;
	struct node* root = ii->arr + bucket;
	struct node* next = root->next;
	struct node* new_node = node_new(key, val);
	node_link(root, new_node);
	node_link(new_node, next);
}

// Returns pointer to node if found
struct node* find_node(struct intint* ii, int key) {
	int bucket = key % ii->buckets;
	return node_find(ii->arr + bucket, key);
}

// Retrieves value associated with key
int intint_find(struct intint* ii, int key, int default_value) {
	struct node* found_node = find_node(ii, key);
	if (found_node) {
		return found_node->value;
	} else {
		return default_value;
	}
}

// Removes key from the map if found
void intint_remove(struct intint* ii, int key) {
	struct node* nod = find_node(ii, key);
	if (nod) {
		node_remove(nod);
	}
}

// Updates value associated with key or creates a new entry if key not found
void intint_update(struct intint* ii, int key, int new_val) {
	struct node* nod = find_node(ii, key);
	if (nod) {
		// Just set the node's value to new value
		nod->value = new_val;
	} else {
		// Create new node.
		intint_insert(ii, key, new_val);
	}
}

// Swaps values of two different keys if found (keys <= 0 are considered invalid)
void intint_swap(struct intint* ii, int key1, int key2) {
	if (key1 <= 0 && key2 <= 0) return;
	if (key1 <= 0) {
		intint_remove(ii, key2);
		return;
	}
	if (key2 <= 0) {
		intint_remove(ii, key1);
		return;
	}

	struct node* nod1 = find_node(ii, key1);
	struct node* nod2 = find_node(ii, key2);
	// Do the swap
	int temp = nod1->value;
	nod1->value = nod2->value;
	nod2->value = temp;
}

