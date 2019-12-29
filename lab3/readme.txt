michaelsu

(0) 
On exit, 44,998 bytes are still in use.  Out of these, 43,646 bytes are reachable and 
1,352 are possibly lost. 

(1)
1. Conditional jump/move dependent on uninitialised value
2. 0 does not have the issue originating at malloc
3. main is recognizing the problem
4. No error is reported.  I am not sure why there is no error.  The array is initialized at size 120 but
   no values are stored in the array.  If I had to guess, y equals whatever is stored in the first element,
   which should be zero but could be a leftover junk value.  Depending on what value is coincidentally there,
   the function prints the correct output.

(2)
1. No difference after compiler
2. They both attempt to access an array element even though the array has not been given any values.
3. They are very different because the act of allocation allows valgrind to detect leaks.  vgme 5.
4. The line has both a read and a write error.
5. It does not detect an error.  Non malloc'd array declaration screws up valgrind.
 

(3)
1. The array at x is referenced after it has already been freed.
2. vgme 7 tells us that the read and write are being applied to a previously allocated block,
   but that block has been freed.
3. Since the read/write are for ints in the code, "invalid read of size 4" gives the size of int

(4)
1. free(same(x))
2. free(same(fun1)) - text segment
   free(x + 42)
   free(same(gxs))
   free(x) - heap
3. Line information
4. It causes free to free the object from the heap.

(5)
1. There is no return address.  After allocating the array, it is never freed.  Valgrind
   does not know the line it happened, but it does indicate the lost bytes.
2. Yes - line 95

(6)
1. The error occurs during the loop.  The function attempts to access a memory region it does not
   have access to.
