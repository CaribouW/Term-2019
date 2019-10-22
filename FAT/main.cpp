#include "fat/fat.h"

int main() {
    //文件引用
    FAT fat("test.img");
//    RootEntry e = fat.fetchClusterEntry("DEMO/THIRD.TXT");//2

    //对于cat，如果得到的e是file,就直接调用fat.readFileContent(e.DIR_FstClus) ; 否则输出这是一个dic
    //对于ls /PATH , 那么需要一个函数来实现: 给定entry(可以是dic / file) ,
    //输出这个entry下属的文件+文件夹
    RootEntry e = fat.fetchClusterEntry("DEMO");
    auto a = fat.fetchPathSolution(e);
    fat.printPathRecur("/DEMO", e);
//    cout << fat.readFileContent(fat.fetchClusterEntry("DEMO/THIRD.TXT").DIR_FstClus);
    return 0;
}




