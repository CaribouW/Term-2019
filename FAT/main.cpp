#include "fat/fat.h"

int main() {
    //文件引用
    FAT fat("test.img");
//    fat.printRootFiles();
    int n = fat.fetchClusterNum("DEMO/THIRD.TXT/d");//2
    string s = fat.readFileContent(n);
    return 0;
}




