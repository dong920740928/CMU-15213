# Bomb Lab

Use `gdb` command with `-tui` argument to disassemble file `bomb`, and use `layout split` in gdb console to get the assembly code.

In each phase, program read a string as the parameter input of the phase function. In each phase function, there are several "explode bomb" instructions which can "explode" the program. Our goal is to figure out the correct input of each phase that avoid the execution of those "explode bomb" instructions.

## Phase 1

Read the assembly code in function `phase_1`. It reads a string from input, and compares the string with a string constant store at address `0x402400`.
```assembly
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq
```

Use command `objdump -s -j .rodata bomb` to get the text section, and find the string in address `0x402400`.
```
 4023f0 79207468 6973206f 6e652e00 00000000  y this one......
 402400 426f7264 65722072 656c6174 696f6e73  Border relations
 402410 20776974 68204361 6e616461 20686176   with Canada hav
 402420 65206e65 76657220 6265656e 20626574  e never been bet
 402430 7465722e 00000000 576f7721 20596f75  ter.....Wow! You
```

As the content printed, the correct input of phase 1 is `Border relations with Canada have never been better.`

## Phase 2
Read the assembly code in function `phase_2`. It reads six numbers from input, then check the array for some conditions. 

```assembly
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)	// check whether a[0]==1
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
  400f1a:	01 c0                	add    %eax,%eax
  400f1c:	39 03                	cmp    %eax,(%rbx) // check whether a[i+1] == 2a[i]
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>	// loop
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	retq 
```

The first number must be `1`, and for each i from 1 to 6, a<sub>i+1</sub> = 2a<sub>i</sub>
So the correct input of phase 2 is `1 2 4 8 16 32`

## Phase 3
Read the assembly code in function `<phase_3>`. It reads two numbers from input and then executes a switch operation. The base address of the jump table of switch operation is `0x402470`.
While debugging the program, we can dump the memory of the jump table to determine the operations for each case of `switch (a)`.

Since there are 8 entries (0 <= a <= 7), we use command `x/16 0x402470` in gdb.
Then we get the code blocks of all 8 cases of variable `a`.
```
(gdb) x/16 0x402470
0x402470:       0x00400f7c      0x00000000      0x00400fb9      0x00000000
0x402480:       0x00400f83      0x00000000      0x00400f8a      0x00000000
0x402490:       0x00400f91      0x00000000      0x00400f98      0x00000000
0x4024a0:       0x00400f9f      0x00000000      0x00400fa6      0x00000000
```

Each eight bytes display an entry, so we catch the 8 entries for 0 <= a <= 7. 
For example, if a equals to 1, then code start from `400fb9` would be executed. If `b == 311` does not hold, bomb will be exploded.

```assembly
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx	// input b
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx	// input a
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)	// unsigned compare
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>	// check whether 0 <= a <= 7
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax	// switch(a)
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)	// switch table base
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax	// case a == 0
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>	// t = 207
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax	// case a == 2
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>	// t = 707
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax	// case a == 3
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>	// t = 256
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax	// case a == 4
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>	// t = 389
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax	// case a == 5
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>	// t = 206
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax	// case a == 6
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>	// t = 682
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax	// case a == 7
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>	// t = 327
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax	// case a == 1:	t = 311
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax	// check whether b == t
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq
```

The equivalent switch case operation of the assembly code above is
```cpp
uint t;
switch (a){
	case 0:
		t = 207;
		break;
	case 1:
		t = 311;
		break;
	case 2:
		t = 707;
		break;
	case 3:
		t = 256;
		break;
	case 4:
		t = 389;
		break;
	case 5:
		t = 206;
		break;
	case 6:
		t = 682;
		break;
	case 7:
		t = 327;
		break;
}

if (b != t){
	// expode bomb
}
```

So all the solution input of `phase_3` are:
```
0 207
1 311
2 707
3 256
4 389
5 206
6 682
7 327
```
Any of them can pass `phase_3`.

## Phase 4
Read the assembly code in `phase_4`. It reads two numbers `a b` from input. There is limit for `a` and `b`, 0 <= a <= 14 and b == 0.
Then call a recursive function `fun4`. It calculates `fun4(14, 0, a)` and the return value must be zero otherwise bomb will explode.

```assembly
000000000040100c <phase_4>://
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax
  401024:	e8 c7 fb ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29>
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)	// check whether 0 <= a <= 14
  401033:	76 05                	jbe    40103a <phase_4+0x2e>
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx
  40103f:	be 00 00 00 00       	mov    $0x0,%esi
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi
  401048:	e8 81 ff ff ff       	callq  400fce <func4>	// fun4(14, 0, a)
  40104d:	85 c0                	test   %eax,%eax	// check whether return value is 0
  40104f:	75 07                	jne    401058 <phase_4+0x4c>
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)	// check whether b == 0
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq
```

The function `fun4` takes three arguments, It might be recursive for serveral tiers and then return a value.
Read the assembly code of `fun4`, we can get the equivalent code below:

```cpp
int fun4(int x, int y, int z){
	int t = (x - y + x < y) / 2 + y;
	if (t > z){
		// entry 1
		x = t - 1;
		return 2 * fun4(x, y, z);
	} else if (t < z) {
		// entry 2
		y = t + 1;
		return 2 * fun4(x, y, z) + 1;
	} else {
		// entry 3
		return 0;
	}
}
``` 

As we can see, there are three entries in `fun4`, two recursive and one return zero. It any tier steps into enrty one during recursive function call, the return value must above than 0 and bomb explodes. So in safe case, entry 2 will not be executed so argument `y` is always `0`.
Then we can simplify the equivalent code of `fun4` like below:

```cpp
int fun4(int x, int y, int z){
	int t = x / 2;
	if (t > z){
		// entry 1
		x = t - 1;
		return 2 * fun4(x, y, z);
	} else if (t < z) {
		// entry 2 (never jump into here)
		y = t + 1;
		return 2 * fun4(x, y, z) + 1;
	} else {
		// entry 3
		return 0;
	}
}
```

Since the top function call is `fun4(14, 0 , a)`, the correct function call must be
```cpp
fun4(14, 0, a) --> 0		// a == 7
or
fun4(14, 0, a) --> fun4(6, 0, a) --> 0	// a == 3
or
fun4(14, 0, a) --> fun4(6, 0, a) --> fun4(2, 0, a) --> 0	// a == 1
or
fun4(14, 0, a) --> fun4(6, 0, a) --> fun4(2, 0, a) --> fun4(0, 0, a) --> 0	// a == 0
```

So all the solutions of `phase_4` are:
```
7 0
3 0
1 0
0 0
```

Any of them can pass `phase_4`.

## Phase 5
Read the assembly code in `phase_5`. It reads a string of six letters, and translate it into another string by a translation table at `0x4024b0`. Then compare result with a string constant at `0x40245e`. If they are not equal, bomb explodes.

```assembly 
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length>
  40107f:	83 f8 06             	cmp    $0x6,%eax
  401082:	74 4e                	je     4010d2 <phase_5+0x70>
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70>
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx
  40108f:	88 0c 24             	mov    %cl,(%rsp)
  401092:	48 8b 14 24          	mov    (%rsp),%rdx
  401096:	83 e2 0f             	and    $0xf,%edx
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)
  4010a4:	48 83 c0 01          	add    $0x1,%rax
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax
  4010ac:	75 dd                	jne    40108b <phase_5+0x29>
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal>
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>
  4010c6:	e8 6f 03 00 00       	callq  40143a <explode_bomb>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt>
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq
```
The logic of the translation just like below:
```cpp
char input[6];
char table[16];
char *output = "flyers"
char translation_result[6];

// input...

for (int i = 0; i < 6; ++i){
	translation_result[i] = table[input[i] & 0xf];
}

// compare with output...
```

Use command `objdump -s -j .rodata ./bomb` to get the string constant data, and find the table at `0x4024b0`:

```
4024b0 6d616475 69657273 6e666f74 7662796c  maduiersnfotvbyl
```

and the target string at `0x40245e`:
```
402450 73656372 65742073 74616765 2100666c  secret stage!.fl
402460 79657273 00000000 00000000 00000000  yers............
```

so the translation result must be `flyers`. Look up the table, and figure out the six offsets of each character in `flyers`:
```
'f': 0x9
'l': 0xf
'y': 0xe
'e': 0x5
'r': 0x6
's': 0x7
```

Since `phase_5` get the four lower bits in each characters of our input string, we must pick up correct characters form ascii table to satisfy the translation logic. There are many solutions, one of them just like below:
```
0x39 0x3f 0x3e 0x35 0x36 0x37
```

Get the dexcimal code:
```
57 63 62 53 54 55
```

Finally string:
```
9?>567
```

## Phase 6
Read the assembly code in `phase_6`. It try to read a sequece of number list `1 2 3 4 5 6`, then construct a link list according the numbers and six global static `Node` variable at `0x6032d0`. Finally it checks whether the link list is in descending order. If the link list is not in descending order, bomb explodes.

This phase is much more complex than formers.
I devide the assembly code into seven small segments for understanding the logic of whole phase.

1. read six numbers `arr`.
2. check each number in `arr` is between `1` and `6` and the numbers in `arr` are different
4. map each `x` to `7-x` in `arr`
5. make a list of `Node *`, named `tmp_nodes`.  `tmp_nodes[i]` points to Node`arr[i]`
6. construct a link list by `tmp_nodes`, The `next field` of `tmp_nodes[i] points to `tmp_nodes[i+1] for `0 <= i <= 4`, `next field` of `tmp_nodes[5]` is `NULL`
7. check whether `value` of `tmp_nodes` is in descending order.

beginning
```
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
```

segment 1:
```asm
  
  401100:	49 89 e5             	mov    %rsp,%r13
  401103:	48 89 e6             	mov    %rsp,%rsi
  401106:	e8 51 03 00 00       	callq  40145c <read_six_numbers>
  40110b:	49 89 e6             	mov    %rsp,%r14
```

This segment is easy to understand.
  
segment 2:
```
  
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d
  401114:	4c 89 ed             	mov    %r13,%rbp
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax
  40111b:	83 e8 01             	sub    $0x1,%eax
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  401128:	41 83 c4 01          	add    $0x1,%r12d
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d
  401130:	74 21                	je     401153 <phase_6+0x5f>
  401132:	44 89 e3             	mov    %r12d,%ebx
  401135:	48 63 c3             	movslq %ebx,%rax
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx
  401148:	83 fb 05             	cmp    $0x5,%ebx
  40114b:	7e e8                	jle    401135 <phase_6+0x41>
  40114d:	49 83 c5 04          	add    $0x4,%r13
  401151:	eb c1                	jmp    401114 <phase_6+0x20>
```

This segment facts as c++ code below:

```cpp
for (int i = 0; i < 6; ++i){
    if ((uint)(arr[i] - 1) > 5){
        bomb();
    }
    for (int j = i + 1; j < 6; ++j){
        if (arr[i] == arr[j]){
            bomb();
        }
    }
}
```

segment 3:

```
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi
  401158:	4c 89 f0             	mov    %r14,%rax
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx
  401160:	89 ca                	mov    %ecx,%edx
  401162:	2b 10                	sub    (%rax),%edx
  401164:	89 10                	mov    %edx,(%rax)
  401166:	48 83 c0 04          	add    $0x4,%rax
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c>
```

This segment acts c++ code below and easy to understand.
```cpp
for (int i = 0; i < 6; ++i){
    arr[i] = 7 - arr[i];
}
```

segment 4:

```
  40116f:	be 00 00 00 00       	mov    $0x0,%esi
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx
  40117a:	83 c0 01             	add    $0x1,%eax
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82>
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2)
  40118d:	48 83 c6 04          	add    $0x4,%rsi
  401191:	48 83 fe 18          	cmp    $0x18,%rsi
  401195:	74 14                	je     4011ab <phase_6+0xb7>
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f>
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82>
```

This segment constructs `tmp_nodes` from static Node* variables and `arr`. It reads data from address `0x6032b0`. That is a static variable since the address is determined before running program.
Use command `objdump -t ./bomb` to look up the global static vaiables.
```
00000000006032f0 g     O .data	0000000000000010              node3
00000000006032d0 g     O .data	0000000000000010              node1
0000000000603310 g     O .data	0000000000000010              node5
00000000006032e0 g     O .data	0000000000000010              node2
0000000000603300 g     O .data	0000000000000010              node4
0000000000603320 g     O .data	0000000000000010              node6
```
And use command `objdump -s -j .data ./bomb` to look up the value of them.

```
 6032b0 e9030000 00000000 00000000 00000000  ................
 6032c0 00000000 00000000 00000000 00000000  ................
 6032d0 4c010000 01000000 e0326000 00000000  L........2`.....
 6032e0 a8000000 02000000 f0326000 00000000  .........2`.....
 6032f0 9c030000 03000000 00336000 00000000  .........3`.....
 603300 b3020000 04000000 10336000 00000000  .........3`.....
 603310 dd010000 05000000 20336000 00000000  ........ 3`.....
 603320 bb010000 06000000 00000000 00000000  ................
```
In x64 system, size of a virtual memory address is 64bits (48 bits in use). 
According to instruction `mov    0x8(%rdx),%rdx`and global static variables above, we can infer the structure of `Node` below:
```
struct Node{
    int value; // 4 bytes
    int index; // 4 bytes
    struct Node *next; // 8 bytes
}
```
The declararion of nodes can also be inferred:
```
struct Node node6 = { 0x01bb, 0x06, NULL};
struct Node node5 = { 0x01dd, 0x05, &node6};
...
```

By the way, the `next` field of node1 is `e0326000 00000000`
and the address of node2 is `6032e0`, so the compile platform of bomb.c must be Little Endian.

According to the conclusions above, this segment acts c++ code below:
```cpp
struct Node* tmp_nodes[6];
for (int i = 0; i < 6; ++i){
    struct Node *p = node1;
    for (int j = 1; j < arr[i]; ++j){
        p = p->next;
    }
    tmp_nodes[i] = p;
}
```

segment 5:

```
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi
  4011ba:	48 89 d9             	mov    %rbx,%rcx
  4011bd:	48 8b 10             	mov    (%rax),%rdx
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)
  4011c4:	48 83 c0 08          	add    $0x8,%rax
  4011c8:	48 39 f0             	cmp    %rsi,%rax
  4011cb:	74 05                	je     4011d2 <phase_6+0xde>
  4011cd:	48 89 d1             	mov    %rdx,%rcx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9>
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx)
  4011d9:	00 
```

This segment simply construct the link list by `tmp_nodes`.

```cpp
struct Node *p, *q;
p = tmp_nodes;
q = tmp_nodes + 1;
while (q < tmp_nodes + 6) {
    p->next = q;
    q++;
    p = p->next;
}
```

segment 6:

```
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax
  4011e3:	8b 00                	mov    (%rax),%eax
  4011e5:	39 03                	cmp    %eax,(%rbx)
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx
  4011f2:	83 ed 01             	sub    $0x1,%ebp
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb>
```

This segment check whether the link list is in descending order

```cpp
int p = 5;
struct Node *q = tmp_nodes;
while (p > 0){
    if (q->value < q->next->value){
        bomb();
    }
    p--;
    q = q->next;
}
```

ending
```
  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	retq
```

To solution `phase_6`,  weuse reverse deduction method from segment 6 to segment 1.
For segment 6,  values of `tmp_nodes` must in descending order. According to static variables table, it must be `0x039c 0x02b3 0x01dd 0x01bb 0x014c 0x00a8`.
So the `tmp_nodes` after segment 5 must be `node3 node4 node5 node6 node1 node2`
According to static variables table, the original link of nodes is `node1 node2 node3 node4 node5 node6`. So `arr` before segment 4 must be `3 4 5 6 1 2`.
The original input of `arr` is `4 3 2 1 6 5`.
Finally, we figure out the answer of `phase_6`: `4 3 2 1 6 5`.