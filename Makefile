.PHONY: clean check

#DEBUGFLAGS=-g -fsanitize=address -DDEBUG

libbheap.so: src/bheap.c src/intint.c
	$(CC) $(DEBUGFLAGS) -fPIC -shared src/bheap.c src/intint.c -Iinclude/ -o libbheap.so

check: libbheap.so
	$(CXX) $(DEBUGFLAGS) test/HeapImitation.cpp ./libbheap.so -Iinclude/ -o libbheap_test
	./libbheap_test > /dev/null
	# If there are no error message above, then all tests passed successfully.
	# Note: if you want, you can run libbheap_test to see the verbose output

clean:
	rm -f libbheap.so libbheap_test

