#include "fat/fat.h"

int main() {
    //文件引用
    FAT fat("test.img");
    fat.printRootFiles();
    return 0;
}




