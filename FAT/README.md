## FAT12文件系统解析

主要结构

- Boot Sector (引导扇区)
- FAT表
  - 包含两个一样的FAT , 各占据9个字节
- 根目录Sector
- 数据区域

### 文件结构

- CMakeLists.txt : cmake 构建文件
- main.cpp : 程序入口文件
- engine : 指令驱动模块
- fatFile : FAT 文件解析模块 
- my_print.asm : asm实现printf 函数功能

### 运行说明

根目录下，使用cmake来进行Makefile生成

```shell
cmake CMakeLists.txt
```

仍旧在根目录下，运行

```
make
```

即可进行文件的生成. 镜像文件默认为 `test.img` 

此时在根目录下生成一个可执行文件 `FAT`

执行

```
./FAT
```

出现指令命令行之后, 即可进行下一步的 img 内部文件查看

- 使用 `make clean` 删除生成文件内容