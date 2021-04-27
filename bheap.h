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

#ifndef _BINARYHEAP
#define _BINARYHEAP

typedef unsigned long ulong;

typedef struct binary_heap {
	double* score; /* index -> score */
	long* forward; /* handle -> index */
	long* backward; /* index -> handle */

	/* Note: the score and backward fields should be the same size.
	The forward field can be larger because it has to cover the popped elements. */
	ulong capacity;
	ulong handle_capacity;

	ulong next_index;
	ulong next_handle;
} binary_heap;

binary_heap* binary_heap_new();
void binary_heap_free(binary_heap* b);
size_t binary_heap_size(binary_heap* bh);

#ifdef DEBUG
void binary_heap_print(binary_heap* bh);
#endif

long binary_heap_insert(binary_heap* bh, double score);
int binary_heap_get(double* score, binary_heap* bh, ulong handle);
int binary_heap_update(binary_heap* bh, unsigned long handle, double score);
int binary_heap_remove(binary_heap* bh, unsigned long handle);

int binary_heap_pop(binary_heap* bh, long* handle, double* score);
int binary_heap_top(binary_heap* bh, long* handle, double* score);

#endif
