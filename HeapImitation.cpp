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

#include <map>
#include <algorithm>
#include <random>
#include <iostream>
#include <cassert>

extern "C" {
#include "bheap.h"
}

std::default_random_engine generator;

// Gold standard for heap behaviour.
// But not a true heap because looking up the min element takes O(n) time.
class HeapImitation {
public:
	typedef std::map<int,double>::value_type KvType;
	int insert(double x) {
		int ind = next_index;
		data[ind] = x;
		next_index++;
		return ind;
	}
	double get(int handle) {
		if (data.find(handle) == data.end()) {
			return 0;
		} else {
			return data[handle];
		}
	}
	bool update(int handle, double x) {
		if (data.find(handle) != data.end()) {
			data[handle] = x;
			return true;
		} else {
			return false;
		}
	}
	bool erase(int handle) {
		if (data.find(handle) != data.end()) {
			data.erase(handle);
			return true;
		} else {
			return false;
		}
	}
	// Returns handle, score of the minimum element
	std::pair<int,double> top() {
		if (data.size() == 0) {
			return {-1,0};
		}
		auto it = std::min_element(data.begin(), data.end(), less_than);
		return *it;
	}
	std::pair<int,double> pop() {
		auto kv = top();
		erase(kv.first);
		return kv;
	}
	int get_next_index() const {
		return next_index;
	}
	int size() {
		return data.size();
	}
private:
	static bool less_than(const KvType& kv1, const KvType& kv2) {
		return kv1.second < kv2.second;
	}
	std::map<int,double> data;
	int next_index = 1;
};

// Random number generators for the random trials
std::uniform_int_distribution<int> op_distribution(0,3); /* either insert, get, update, or erase */

// Real number in [0.0001, 1.0]. 0 is a special value for when the C code can't find handle
std::uniform_real_distribution<double> real_distribution(0.0001,1.0);

int random_operation() {
	return op_distribution(generator);
}

double random_real() {
	return real_distribution(generator);
}

int random_int(int max) {
	std::uniform_int_distribution<int> dist(0,max);
	return dist(generator);
}

int random_handle(const HeapImitation& h) {
	return random_int(h.get_next_index()); // - 1);
}

/* Compares two values and prints either OK or ERROR */
template <class T>
void compare(const T& x1, const T& x2, const char* msg) {
	if (x1 == x2) {
		std::cout << "\033[1;32mOK\033[0m " << msg << " " << x1 << " matches." << std::endl;
	} else {
		std::cout << "\033[1;31mERROR\033[0m " << msg << " " << x1 << " doesn't match " << x2 << std::endl;
		throw "Error occurred.";
	}
}

// Does a randomly-selected action on either the C heap or the C++ imitation heap
void do_random_action(HeapImitation& h, binary_heap* bh) {
	int op = random_operation();

	int handle = random_handle(h);
	long handle_c = 0;

	double x = random_real();
	double x_c = 0;

	bool success = false;
	int c_error = 0;

	std::pair<int,double> kv;
	//std::cout << "Doing operation " << op << std::endl;
	switch (op) {
	case 0:
		// Insert
		handle = h.insert(x);
		handle_c = binary_heap_insert(bh, x);
		compare<long>(handle, handle_c, "inserted handle");
		break;
	case 1:
		// Get
		x = h.get(handle);
		binary_heap_get(&x_c, bh, handle);
		compare<double>(x, x_c, "returned value");
		break;
	case 2:
		// Update
		std::cout << "Update handle " << handle << " to " << x << std::endl;
		success = h.update(handle, x);
		c_error = binary_heap_update(bh, handle, x);
		assert(success xor c_error);
		break;
	case 3:
		// Erase
		std::cout << "Erase handle " << handle << std::endl;
		success = h.erase(handle);
		c_error = binary_heap_remove(bh, handle);
		assert(success xor c_error);
		break;
	case 4:
		// Top
		kv = h.top();
		handle = kv.first;
		x = kv.second;

		binary_heap_top(bh, &handle_c, &x_c);
		compare<long>(handle, handle_c, "top handle");
		compare<double>(x, x_c, "top value");
		break;
	case 5:
		// Pop
		kv = h.pop();
		handle = kv.first;
		x = kv.second;

		binary_heap_pop(bh, &handle_c, &x_c);
		compare<long>(handle, handle_c, "popped handle");
		compare<double>(x, x_c, "popped value");
		break;
	}
	//binary_heap_print(bh);
	compare<ulong>(h.size(), binary_heap_size(bh), "heap size");
}

int main() {
	// Repeat experiment many times
	for (int trial = 0; trial < 100; ++trial) {
		std::cout << "\033[1;34mINFO\033[0m " << "Trial " << trial << std::endl;
		HeapImitation h;
		binary_heap* bh = binary_heap_new();
		// For each trial, do 1000 random actions and check agreement after every step.
		for (int i = 0; i < 1000; ++i) {
			do_random_action(h, bh);
		}
		binary_heap_free(bh);
	}
	std::cout << "Tests complete.";
	return 0;
}

