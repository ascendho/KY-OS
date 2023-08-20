## 实验思考

1. make工具的编译原理是什么？

make 是一个命令工具，它解释Makefile中的指令（规则）。在Makefile文件中描述了整个工程所有文件的编译顺序、编译规则。在执行make之前，需要一个命名为Makefile的特殊文件（本文的后续将使用Makefile作为这个特殊文件的文件名）来告诉make需要做什么（完成什么任务），该怎么做。



2. 如何直接使用gcc命令完成myapp.c、greeting.h、greeting.c三个文档的编译？

① gcc –c greeting.c                   // 生成greeting.o文件

② gcc –o greeting myapp.c greeting.o  // 把greeting.o和myapp.c一起编译成可执行文件greeting。