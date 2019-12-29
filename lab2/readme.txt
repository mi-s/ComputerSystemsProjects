(Ex 1)
By setting watchpoints, using step, and using list to see the desired result, I was able to find the bugs.
multiStrlen had an issue where q would take on the values of the old memory addresses
of p as q is incremented.  In twoFingerSort, I used display result to follow the result as I stepped through the loop.
Using this, I realized that the loop started at the incorrect index, so the first letter wasn't sorting.

(Ex 2)
The array data4 is corrupted on line 27 during the loop.  

(gdb) info local
offset = 51
i = 103
data = "Xfmm-!nbzc"
stringToPrint = "Some additional text."
data3 = "A final bit of text."
data4 = "Well, maybe just a little uis to eat xl,ijtst loe  \000\000\000"

These are some of the local variables as data4 is being corrupted.  To find this, I simply set a breakpoint
to before the loop and then stepped in increments of 10 while using display data4 and display offset.

(Ex 3)
1. 8 Bytes, their values originate from %ebp and %ecx.
2. The integer 10
3. (gdb) break *0x0804837f
4. Returns 11, stored in the %eax register