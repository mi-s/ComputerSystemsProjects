/* 
 * CS:APP Data Lab 
 * 
 * Name: Michael Su, CNET: michaelsu
 * 
 * bits.c - Source file with your solutions to the project.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the project by
editing the collection of functions in this source file.

CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }



NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. The maximum number of ops for each function is given in the
     header comment for each function.

/*
 * STEP 2: Modify the following functions according to the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the btest checker to verify that your solutions produce
 *      the correct answers.
 */


#endif

/************************************************ 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
  /* absVal returns (~x + 1) for negative numbers
   * and (x + 0) for positive numbers */
  int isNegative = (x >> 31) & 1;
  int mask = (isNegative << 31) >> 31;
  int complement = x ^ mask;
  return complement + isNegative;
}

/************************************************ 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
  /* addOK checks if x and y have different signs.
   * If they don't, then it checks if x and y both have
   * the same sign as their sum.  If either condition is true, return 1 */
  int xy = x + y;
  int xSign = x >> 31;
  int ySign = y >> 31;
  int xySign = xy >> 31;
  int x_ySignDiff = !!(xSign ^ ySign);
  int x_xySignSame = !(xSign ^ xySign);
  int y_xySignSame = !(ySign ^ xySign);
  return x_ySignDiff | (x_xySignSame & y_xySignSame);
}

/************************************************ 
 * - return 1 if all even-numbered bits in word set to 1
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allEvenBits(int x) {
  /* allEvenBits returns an equality statement comparing
   * the even bits of x with an int that has all even bits set to 1 */
  int halfEven = (0x55 << 8) + 0x55;
  int fullEven = (halfEven << 16) + halfEven;
  return !((x & fullEven) ^ fullEven);
}

/************************************************
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  /* bang returns 1 if x is not positive or negative, otherwise returns 0 */
  int neg_x = ~x + 1;
  int isNegative = (x >> 31) & 1;
  int isPositive = (neg_x >> 31) & 1;
  return (isNegative | isPositive) ^ 1;
}	

/************************************************
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  /* bitCount adds up pairs of bits, then quads, then octuples,
   * then finally adds up the bits of both halves of x */
  int halfOneMask = (0x55 << 8) + 0x55;
  int oneMask = (halfOneMask << 16) + halfOneMask;
  int halfTwoMask = (0x33 << 8) + 0x33;
  int twoMask = (halfTwoMask << 16) + halfTwoMask;
  int halfFourMask = (0xF << 8) + 0xF;
  int fourMask = (halfFourMask << 16) + halfFourMask;
  int eightMask = (0xFF << 16) + 0xFF;
  int sixteenMask = (0xFF << 8) + 0xFF;
  int addOnes = (x & oneMask) + ((x >> 1) & oneMask);
  int addTwos = (addOnes & twoMask) + ((addOnes >> 2) & twoMask);
  int addFours = (addTwos & fourMask) + ((addTwos >> 4) & fourMask);
  int addEights = (addFours & eightMask) + ((addFours >> 8) & eightMask);
  return (addEights & sixteenMask) + ((addEights >> 16) & sixteenMask);
}

/************************************************
 * bitNor - ~(x|y) using only ~ and & 
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
  /* The complement of a union is equal to
   * the intersection of two complements */
  return ~x & ~y;
}

/************************************************
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
  /* byteSwap extracts the n'th and m'th bytes, swaps their positions,
   * clears the n'th and m'th positions of x, then re-inserts the bytes
   * in swapped positions */
  int nShift = n << 3;
  int mShift = m << 3;
  int nByte = (x >> nShift) & 0xFF;
  int mByte = (x >> mShift) & 0xFF;
  int nByte_m = nByte << mShift;
  int mByte_n = mByte << nShift;
  int nClear = 0xFF << nShift;
  int mClear = 0xFF << mShift;
  int xClear_n = (x | nClear) ^ nClear;
  int xClear_nm = (xClear_n | mClear) ^ mClear;
  return xClear_nm | nByte_m | mByte_n;
}

/************************************************ 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int isFalse = !x;
  int zMask = (isFalse << 31) >> 31;
  int yMask = ~zMask;
  return (y & yMask) | (z & zMask);
}	

/************************************************
 * ezThreeFourths - multiplies by 3/4 rounding toward 0,
 *   Should exactly duplicate effect of C expression (x*3/4),
 *   including overflow behavior.
 *   Examples: ezThreeFourths(11) = 8
 *             ezThreeFourths(-9) = -6
 *             ezThreeFourths(1073741824) = -268435456 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int ezThreeFourths(int x) {
  /* ezThreeFourths calculates (3x)/4 and rounds down for positive
   * numbers using truncation and rounds up for negative numbers using round */
  int x3 = x + x + x;
  int isNegative = x3 >> 31;
  int rem = x3 & 3;
  int round = ((rem >> 1) | (rem & 1)) & isNegative;
  int quotient = x3 >> 2;
  return quotient + round;
} 

/************************************************ 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  /* fitsBits compares the least-significant 31 bits of x (or their complement 
   * for negative x) with the least-significant (n - 1) bits.  If they
   * are identical, then return 1. */
  int negMask = x >> 31;
  int negComp = x ^ negMask;
  int neg1 = ~0;
  int shift = n + neg1;
  int mask = ~(neg1 << shift);
  int truncate = negComp & mask;
  return !(negComp ^ truncate); 
}

/************************************************
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  /* getByte shifts the n-th byte to index 0 and clears bytes 1-3. */
  int shift = n << 3;
  int byte = x >> shift;
  int mask = ~(0xFF);
  return (byte | mask) ^ mask;
}

/************************************************ 
 * greatestBitPos - return a mask that marks the position of the
 *             most significant 1 bit. If x == 0, return 0
 *   Example: greatestBitPos(96) = 0x40
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 70
 *   Rating: 4 
 */
int greatestBitPos(int x) {
  int oneBit = x | (x >> 1);
  int twoBits = oneBit | (oneBit >> 2);
  int fourBits = twoBits | (twoBits >> 4);
  int eightBits = fourBits | (fourBits >> 8);
  int sixteenBits = eightBits | (eightBits >> 16);
  int leftBits = ((~sixteenBits >> 1) ^ (1 << 31));
  return sixteenBits & leftBits;
}

/************************************************
 * implication - return x -> y in propositional logic - 0 for false, 1
 * for true
 *   Example: implication(1,1) = 1
 *            implication(1,0) = 0
 *   Legal ops: ! ~ ^ |
 *   Max ops: 5
 *   Rating: 2
 */
int implication(int x, int y) {
  /* if x is 0, then return 1
   * if x is 1, then return y */ 
  return !x | y;	
}

/************************************************ 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  /* isAsciiDigit checks if x is 6 bits long, contains the bits representing
   * 16 and 32, and does not contain the bits representing
   * 8 and 2 or 4 at the same time.  If all conditions hold, return 1. */
  int notTruncated = !((x & 63) ^ x);
  int has48 = !((x & 48) ^ 48);
  int first4 = x & 15;
  int has8 = first4 >> 3;
  int has4 = first4 >> 2;
  int has2 = first4 >> 1;
  return notTruncated & has48 & !(has8 & (has2 | has4));
}

/************************************************ 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
  /* if x and y are equal, x ^ y returns 0 */
  return !(x ^ y);
}

/************************************************
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
  /* isLess checks if x < y by comparing the signs of x,
   * y, and x - y */
  int xSign = x >> 31;
  int ySign = y >> 31;
  int xNeg_yPos = xSign & !ySign;
  int xPos_yNeg = ySign & !xSign;
  int yNegative = ~y + 1;
  int xyDiffSign = (x + yNegative) >> 31;
  int signDiff = xyDiffSign & 1;
  return xNeg_yPos | (!xPos_yNeg & signDiff);
}

/************************************************
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
  /* (x >> 31) equals 0 when x is positive
   * and -1 when x is negative. */
  return !(x >> 31);
}

/************************************************
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
  /* isPower2 checks if x >= 0.  If this is true, then it finds if
   * there is exactly one set bit in the first 31 bits of x. */
  int notNegative = !(x >> 31);	
  int neg1 = ~0;
  int allOnes = x + neg1;
  int isZero = !(~x ^ neg1);
  int rightBitsClear = !(x & allOnes);
  int leftBitsClear = !((x & ~allOnes) ^ x);
  return notNegative & rightBitsClear & leftBitsClear & !isZero;
}

/************************************************
 * isTmin - returns 1 if x is the minimum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmin(int x) {
  /* If x is equal to (~x + 1) and x is not zero, 
   * then it is the Tmin */
  int xComp = ~x;
  int neg_x = xComp + 1;
  int isIdentical = !(x ^ neg_x);
  int isZero = !(xComp ^ ~0);
  return isIdentical & !isZero;
}

/************************************************
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) {
  /* Arithmetic shift returns 111...111 */
  return ~0;
}

/************************************************
 * rotateLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateLeft(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateLeft(int x, int n) {
  /* rotateLeft shifts x left by n and takes the bits removed at the left
   * and re-inserts them into the right */
  int min = 1 << 31;
  int mask = (min >> n) << 1;
  int temp = x & mask;
  int temp_no_carry = ((temp >> 1) | min) ^ min;
  int negativeN = ~n + 1;
  int temp_at_0 = temp_no_carry >> (31 + negativeN);
  int shift = x << n;
  int merge = shift | temp_at_0;
  return merge;
}

/************************************************
 * satMul2 - multiplies by 2, saturating to Tmin or Tmax if overflow
 *   Examples: satMul2(0x30000000) = 0x60000000
 *             satMul2(0x40000000) = 0x7FFFFFFF (saturate to TMax)
 *             satMul2(0x60000000) = 0x80000000 (saturate to TMin)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int satMul2(int x) {
  /* satMul2 compares the signs of x and (x + x).  If they are the same, 
   * it returns (x + x).  If they are different, there was overflow, so
   * it returns Tmin for negative x values and Tmax for positive values. */
  int sign = (1 << 31);
  int xSign = x & sign;
  int overflow = (xSign >> 31) ^ ~sign;
  int xx = x + x;
  int xxSign = xx & sign;
  int sameSign = !(xSign ^ xxSign);
  int mask = (sameSign << 31) >> 31;
  return (xx & mask) | (overflow & ~mask);
}
