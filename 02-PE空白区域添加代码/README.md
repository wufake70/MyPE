# PE空白区域插入字节码(目前只针对32位PE)
```
PE文件插入可执行Byte Code(字节码)，该代码若是保护本程序；则为‘加壳’，该代码若有破坏行为，则是‘病毒’。

将要实现功能，插入字节码使之运行时先弹窗。

```
## VC6反汇编调试分析
```
#test.c 源码

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
int a=0;
int main(){
	while(1) printf("%d\n",a++),system("pause > nul");
	MessageBox(0,0,0,0); // 调用MessageBox函数，保证vc6编译时一定链接user32.dll。
	return 0;
}
```
* ![](.src/01-vc6%E8%B0%83%E8%AF%95%E5%88%86%E6%9E%90.png)
* ![](.src/02-vc6%E8%B0%83%E8%AF%95%E5%88%86%E6%9E%90-%E6%8B%86%E5%88%86%E6%BA%90%E7%A0%81.png)
* ![](.src/03-%E5%BE%97%E5%88%B0MessageBox%E7%9A%84%E5%AD%97%E8%8A%82%E7%A0%81.png)
```
字节吗分析
00401067 8B F4                mov         esi,esp
00401069 6A 00                push        0
0040106B 6A 00                push        0
0040106D 6A 00                push        0
0040106F 6A 00                push        0
00401071 FF 15 EC C2 42 00    call        dword ptr [__imp__MessageBoxA@16 (0042c2ec)]
00401077 3B F4                cmp         esi,esp
00401079 E8 D2 01 00 00       call        __chkesp (00401250)

6A 00 表示是 填参数
FF 15 表示 call 调用
E8 表示 call，后面跟的地址是: 要调用的函数地址 - E8当前的地址 - 5
    要调用的函数地址，就如 MessageBoxA是user32.dll的函数，需要查询他的RVA，不同主机不同，
    E8当前的地址，在ram中地址
    补充 E9 表示 jmp(jump)，后接地址同 E8
```
* `查询MessageBoxA的RVA`--x32dbg
* ![](.src/04-x32dbg%E6%9F%A5%E8%AF%A2MessageBoxA%E7%9A%84RVA.png)
* 或者直接打印出`MessageBoxA地址`
```
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
int main(){	
	printf("MessageBoxA Address: %p\n",MessageBoxA);
	system("pause > nul");
	return 0;
}
```
## 构造要写入的代码
* `6A 00 6A 00 6A 00 6A 00 E8 00 00 00 00 E9 00 00 00 00`
```
E8 后的地址:
    MessageBoxA address: 768910D0(不同主机，不同时段，都不一样)
    E8 Byte Code address: 这段代码可以插在PE任何空白区域，
        注意 win8以上系统程序入口要大于SizeOfHeaders(DOS_MZ+DOS块+PE头+节表 在ram对齐后的数据)
```
* PEtool查看PE头&节表
* ![](.src/05-PE%E5%A4%B4%26%E8%8A%82%E8%A1%A8.png)
```
可以.text之后，.rdata之前的空白区域，插入代码，即 0x23960的位置，
又因 file align 和ram align相同，0x23960+ImageBase即为内存地址。

则 E8 address: 0x423968
   E9 address: 0x42396D

计算
    E8后接地址: 768910D0 - 423968 - 5 = 7646 D763  // 调用MessageBoxA
    E9后接地址: 401290 - 42396D - 5 = FFFD D91E    // 执行完MessageBoxA，返回原来程序入口，继续执行原有逻辑
    注意: 以小段存储方式进行修改
```
* 构造完后的代码: `6A 00 6A 00 6A 00 6A 00 E8 63 D7 46 76 E9 1E D9 FD FF`
* 修改原`EntryOfPoint`为插入代码的地址`23960`，即可完成。
## 需要注意
* `修改后的pe在不同主机上，一定会出错，MessageBoxA地址不同。`
* `修改后的pe在同一主机上，关机重新启动，MessageBoxA地址也会不同，pe也会出错。`
* `修改的原程序必须链接过user32.dll，否则无法使用MessageBoxA。`

