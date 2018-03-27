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

Since the top function call is `fun4(14, 0 , a)`, the correct code trace must be
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