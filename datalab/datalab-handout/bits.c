/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
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

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

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

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
    the coding rules are less strict.  You are allowed to use looping and
    conditional control.  You are allowed to use both ints and unsigneds.
    You can use arbitrary integer and unsigned constants.

    You are expressly forbidden to:
    1. Define or use any macros.
    2. Define any additional functions in this file.
    3. Call any functions.
    4. Use any form of casting.
    5. Use any data type other than int or unsigned.  This means that you
    cannot use arrays, structs, or unions.
    6. Use any floating point data types, operations, or constants.


    NOTES:
    1. Use the dlc (data lab checker) compiler (described in the handout) to
    check the legality of your solutions.
2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
    that you are allowed to use for your implementation of the function.
    The max operator count is checked by dlc. Note that '=' is not
    counted; you may use as many of these as you want without penalty.
    3. Use the btest test harness to check your functions for correctness.
    4. Use the BDD checker to formally verify your functions
    5. The maximum number of ops for each function is given in the
    header comment for each function. If there are any inconsistencies
    between the maximum ops in the writeup and in this file, consider
    this file the authoritative source.

    /*
     * STEP 2: Modify the following functions according the coding rules.
     *
     *   IMPORTANT. TO AVOID GRADING SURPRISES:
     *   1. Use the dlc compiler to check that your solutions conform
     *      to the coding rules.
     *   2. Use the BDD checker to formally verify that your solutions produce
     *      the correct answers.
     */

#endif
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) { return ~((~x) | (~y)); }
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    /* shift the byte n to right */
    return (x >> (n << 3)) & 0xFF;
}
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int logicalShift(int x, int n) {
    /* shift to right and clear '1's on th left */
    int s = 0x80 << 24;
    return (x >> n) & (~((s >> n) << 1));
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    /* count 1 in each two bits, then merge to each four bits, eight bits, ...
     */
    int mark_offset1 = 0x55;
    int mark_offset2 = 0x33;
    int mark_offset4 = 0x0f;
    int mark_offset8 = (0xff << 16) + 0xff;
    int mark_offset16 = (0xff << 8) + 0xff;

    mark_offset1 = (mark_offset1 << 8) + 0x55;
    mark_offset1 = (mark_offset1 << 16) + mark_offset1;

    mark_offset2 = (mark_offset2 << 8) + 0x33;
    mark_offset2 = (mark_offset2 << 16) + mark_offset2;

    mark_offset4 = (mark_offset4 << 8) + 0x0f;
    mark_offset4 = (mark_offset4 << 16) + mark_offset4;

    x = ((x & mark_offset1) + ((x >> 1) & mark_offset1));
    x = ((x & mark_offset2) + ((x >> 2) & mark_offset2));
    x = ((x & mark_offset4) + ((x >> 4) & mark_offset4));
    x = ((x & mark_offset8) + ((x >> 8) & mark_offset8));
    x = ((x & mark_offset16) + ((x >> 16) & mark_offset16));

    return x;
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
    /* clear bits on the left of the first 1 from right */
    return (((x >> 31) & 0x1) ^ 0x1) & ((((~((~x) + 1)) ^ x) >> 31) & 0x1);
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    /* -2^31 */
    return 1 << 31;
}
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    /* x < 2^(n-1) and x >= -2^(n-1) or n == 32 */
    int bound = (~(1 << (n + (~0))) + 1);
    int sign1 = (x + bound);
    int sign2 = (bound + (~x));
    int sign3 = (n & 32) >> 5;

    return (((sign1 & sign2) >> 31) & 1) | sign3;
}
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    /* add 1 to x>>n if x < 0 and -x is not divided by 2^n */
    int directQ = x >> n;
    int invX = (~x) + 1;
    int mark = (1 << n) + (~0);
    int add = ((x & (~((invX & mark) + (~0)))) >> 31) & 1;

    return directQ + add;
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) { return (~x) + 1; }
/*
 * isPositive - return 1 if x > 0, return 0 otherwise
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    /* sign bit of x == 0 and sign bit of -x == 1 */
    int invX = ~x;
    return ((invX & (invX + 1)) >> 31) & 1;
}

/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    /* if x and y have the same sign, then return x < y + 1, else return x < 0
     */
    int diffSign = x ^ y;
    return (((diffSign & x) | ((~diffSign) & (x + (~y)))) >> 31) & 1;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    /* calc the index of the leftmost '1' in bits of x. for x = 0x01000000
     * return 24 */
    int sign16, sign8, sign4, sign2, sign1;
    sign16 = (((1 << 16) + (~x)) >> 31) & 1;
    x = x >> (sign16 << 4);
    sign8 = (((1 << 8) + (~x)) >> 31) & 1;
    x = x >> (sign8 << 3);
    sign4 = ((16 + (~x)) >> 31) & 1;
    x = x >> (sign4 << 2);
    sign2 = ((4 + (~x)) >> 31) & 1;
    x = x >> (sign2 << 1);
    sign1 = ((2 + (~x)) >> 31) & 1;

    return (sign16 << 4) + (sign8 << 3) + (sign4 << 2) + (sign2 << 1) + sign1;
}
/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    /* if exp == 0xff and frac != 0, inverse the leftmost bit. */

    int exp = uf & 0x7f800000;
    int frac = uf & 0x007fffff;
    int nan = exp == 0x7f800000 && frac != 0;

    if (!nan) {
        uf ^= 0x80000000;
    }

    return uf;
}
/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    int s = 0;
    unsigned ux = x;
    int exp = 0;
    unsigned mark = 0x80000000;
    unsigned res = 0;
    unsigned m = 0;
    int frac_offset = 0;

    if (x < 0) {
        ux = -x;
        s = 1;
    }
    //   printf("float_i2f x=%x %d ux=%x %d\n", x, x, ux, ux);

    while (mark && (ux & mark) == 0) {
        /* handle the frac of x */
        mark >>= 1;
        ++frac_offset;
    }

    if (mark) {
        // x != 0
        mark -= 1;
        m = mark & ux;
        while (mark) {
            ++exp;
            mark >>= 1;
        }

        // round
        {
            int up;
            unsigned tail;

            m <<= frac_offset;
            tail = m & 0xff;
            up = (tail > 0x80) || (tail == 0x80 && (m & 0x100));

            m >>= 8;
            m += up;
            exp += (m & 0x00800000) > 0;  // 1.11111... up to 2
            m &= 0x007fffff;
            //           printf("tail=%x up=%d m=%x\n", tail, up, m);
        }

        // bias
        exp += 127;
    }
    //  printf("s=%x exp=%x %d m=%x %d\n", s, exp, exp, m, m);

    res |= s << 31;
    // printf("step1 res=%x\n", res);
    res |= exp << 23;
    // printf("step2 res=%x\n", res);
    res |= m;
    // printf("step3 res=%x\n", res);

    return res;
}
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    unsigned frac = uf & 0x007fffff;
    unsigned exp = (uf >> 23) & 0x000000ff;
    unsigned res = 0;

    if (exp == 0) {
        // denormal
        unsigned overflow = frac & 0x00400000;
        frac <<= 1;
        if (overflow) {
            exp = 1;
        }
    } else if (exp == 0xff) {
        // special value
    } else {
        // normal
        ++exp;
        if (exp == 0xff) {
            // overflow
            frac = 0;
        }
    }

    res |= frac;
    res |= (exp << 23);
    res |= (uf & 0x80000000);

    return res;
}
