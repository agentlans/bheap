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

#ifndef _INTINT
#define _INTINT

// The start of a doubly-linked list
struct node {
	int key;
	int value;
	struct node* prev;
	struct node* next;
};

// Stores int-int pairs in a hash table
// designed for internal use only
struct intint {
	struct node* arr;
	int buckets;
};

struct intint* intint_new(int num_buckets);
void intint_free(struct intint* ii);
void intint_insert(struct intint* ii, int key, int val);
int intint_find(struct intint* ii, int key, int default_value);
void intint_remove(struct intint* ii, int key);
void intint_update(struct intint* ii, int key, int new_val);
void intint_swap(struct intint* ii, int key1, int key2);

#endif
