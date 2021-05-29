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
#include <float.h>

#include "bheap.h"

binary_heap* binary_heap_new() {
	binary_heap* b = malloc(sizeof(binary_heap));
	if (!b) return NULL;

	b->score = malloc(20 * sizeof(double));
	if (!(b->score)) {
		free(b);
		return NULL;
	}

	b->forward = intint_new(31);
	if (!(b->forward)) {
		free(b->score);
		free(b);
		return NULL;
	}

	b->backward = intint_new(31);
	if (!(b->backward)) {
		free(b->score);
		intint_free(b->forward);
		free(b);
		return NULL;
	}

	b->capacity = 20;
	b->handle_capacity = 20;

	b->next_index = 1;
	b->next_handle = 1;
	return b;
}

void binary_heap_free(binary_heap* b) {
	free(b->score);
	free(b->forward);
	free(b->backward);
	free(b);
}

/* Functions to expand or shrink array beyond the original capacity */
int expand_array_double(double** arr, int new_capacity) {
	double* ptr = realloc(*arr, new_capacity * sizeof(double));
	if (ptr) {
		*arr = ptr;
		return 0;
	} else {
		return 1;
	}
}

int expand_array_int(int** arr, int new_capacity) {
	int* ptr = realloc(*arr, new_capacity * sizeof(int));
	if (ptr) {
		*arr = ptr;
		return 0;
	} else {
		return 1;
	}
}

int push_back(binary_heap* bh, double score) {
	int ind = bh->next_index;
	int h = bh->next_handle;

	/* Make new space and make sure they're properly resized. */
	if (h == bh->handle_capacity) {
		int new_cap = bh->handle_capacity * 1.1;
		bh->handle_capacity = new_cap;
	}
	if (ind == bh->capacity) {
		int new_cap = bh->capacity * 1.1;
		int err = expand_array_double(&(bh->score), new_cap);
		if (err) return -1;
		bh->capacity = new_cap;
	}

	/* Update the mappings */
	bh->score[ind] = score;
	intint_update(bh->backward, ind, h);
	intint_update(bh->forward, h, ind);
	/* Update the IDs */
	bh->next_handle++;
	bh->next_index++;
	/* Return handle */
	return h;
}

size_t binary_heap_size(binary_heap* bh) {
	return bh->next_index - 1;
}

/* Removes the last item from the heap */
void pop_back(binary_heap* bh, int* handle, double* score) {
	int last_ind = bh->next_index - 1;
	/* Save the score and handle of the last item */
	*score = bh->score[last_ind];
	int h = intint_find(bh->backward, last_ind, -1);
	*handle = h;
	/* Erase the score and handles */
	bh->score[last_ind] = 0;
	intint_remove(bh->forward, h);
	intint_remove(bh->backward, last_ind);
	/* Size reduced by 1 
	but next handle is monotone increasing 
	so each handle is unique. */
	bh->next_index--;
}

/* Finds the score associated with a handle */
int binary_heap_get(double* score, binary_heap* bh, int handle) {
	if (handle >= bh->next_handle || handle == 0) {
		/* No such handle */
		return 1;
	}
	int index = intint_find(bh->forward, handle, -1);
	if (index <= 0) {
		/* Handle has already been deleted */
		return 1;
	}
	*score = bh->score[index];
	return 0;
}

/* Swaps arr[i] and arr[j] for a double array */
void swap_arr_dbl(double* arr, int i, int j) {
	double tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

/* Swaps arr[i] and arr[j] for a int array */
void swap_arr_int(int* arr, int i, int j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

/* Swaps the items at index1 and index2 */
void swap(binary_heap* bh, int index1, int index2) {
	swap_arr_dbl(bh->score, index1, index2);
	intint_swap(bh->backward, index1, index2);

	int handle1 = intint_find(bh->backward, index1, -1);
	int handle2 = intint_find(bh->backward, index2, -1);
	intint_swap(bh->forward, handle1, handle2);
}

/******* Heap operations **************/

#define PARENT(n) ((n) >> 1)
#define LEFT(n) ((n) << 1)
#define RIGHT(n) (((n) << 1) + 1)

int in_range(binary_heap* bh, unsigned int index) {
	return (1 <= index && index < bh->next_index);
}

void min_heapify(binary_heap* bh, unsigned int n) {
	unsigned int i = n;
	while (1) {
		unsigned int l = LEFT(i);
		unsigned int r = RIGHT(i);
		double proband = bh->score[i];

		unsigned int smallest;
		if (in_range(bh, l) && bh->score[l] < proband) {
			smallest = l;
		} else {
			smallest = i;
		}
		if (in_range(bh, r) && bh->score[r] < bh->score[smallest]) {
			smallest = r;
		}
		if (smallest != i) {
			swap(bh, i, smallest);
			/* Iterative form of min_heapify(bh, smallest) */
			i = smallest;
		} else {
			break;
		}
	}
}

void decrease_key(binary_heap* bh, unsigned int i, double score) {
	bh->score[i] = score;
	while (i > 1 && bh->score[i] < bh->score[PARENT(i)]) {
		swap(bh, i, PARENT(i));
		i = PARENT(i);
	}
}

void increase_key(binary_heap* bh, unsigned int i, double score) {
	bh->score[i] = score;
	min_heapify(bh, i);
}

int binary_heap_insert(binary_heap* bh, double score) {
	int ind = bh->next_index;
	int handle = push_back(bh, score);
	if (handle == -1) {
		/* Error pushing new element */
		return -1;
	}
	decrease_key(bh, ind, score);
	return handle;
}

int binary_heap_top(binary_heap* bh, int* handle, double* score) {
	if (binary_heap_size(bh) == 0) {
		/* Empty heap so no top element */
		*handle = -1;
		return 1;
	}
	*handle = intint_find(bh->backward, 1, -1);
	*score = bh->score[1];
	return 0;
}

int binary_heap_pop(binary_heap* bh, int* handle, double* score) {
	if (binary_heap_size(bh) == 0) {
		/* Empty heap so can't pop */
		*handle = -1;
		return 1;
	}
	/* Put smallest element at the end and pop it */
	swap(bh, 1, bh->next_index-1);
	pop_back(bh, handle, score);
	/* Sink the new root that was pulled up from the end */
	if (bh->next_index > 1) {
		min_heapify(bh, 1);
	}
	return 0;
}

// Updates the value referred by handle to a new value
int binary_heap_update(binary_heap* bh, int handle, double score) {
	if (handle == 0 || handle >= bh->next_handle) {
		/* No such handle */
		return 1;
	}
	int i = intint_find(bh->forward, handle, -1);
	if (i <= 0) {
		/* No such index */
		return 1;
	}
	if (score < bh->score[i]) {
		decrease_key(bh, i, score);
	} else {
		increase_key(bh, i, score);
	}
	return 0;
}

// Removes handle from heap
int binary_heap_remove(binary_heap* bh, int handle) {
	if (binary_heap_update(bh, handle, -DBL_MAX)) {
		return 1; /* Can't update handle */
	}

	int dummy;
	double score;
	return binary_heap_pop(bh, &dummy, &score);
}

