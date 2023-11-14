# RVA转换FOA 实现数据修改
## RVA、FOA
* RVA: PE文件的相对虚拟地址`Relative Virual Address`是PE文件中的数据、模块等`运行在内存中`的实际地址相对PE文件装载到内存的基址之间的距离。
* FOA： 文件偏移地址`File Offset Address`和内存无关，它是指某个位置距离文件头的偏移。

## X86的RVA、FOA转化，实现数据修改
* 运行x86程序可知 int a的地址值为`0x00426A30`
* 通过winhex可以查询该pe文件`ImageBase为 0x00400000`
* 即可得出该程序运行时`RVA 26A30`
* 再根据`RVA、FOA转化规则,file align、ram align都为1000，rva=foa`，可以直接从文件的26a30开始修改int a。
* 可以使用`ollydbg 动态调试工具`，进行 int a的rva 修改。

## X64的RVA、FOA转化，实现数据修改
* 运行x64程序可知 int a的地址值为`0x00007ff68ae38010`。
* 通过winhex可以查询该pe文件`ImageBase为 0x0000000140000000`。
* 不合情理，无法直接得出`RVA`，`直接动态调试发现，程序如何va 7FF68AE30000，即RVA=8010`
* `找到节表的第二节的VirtualAddress 0x00008000，PointerToRawData 0x00007400`。
* 再根据`RVA、FOA转化规则，差职为0x10，则FOA 0x00007410`，可以直接从文件的26a30开始修改int a。
* 可以使用`ollydbg 动态调试工具`，进行 int a的rva 修改。