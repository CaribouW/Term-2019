#include "fat/fat.h"

int main() {
    //文件引用
    FILE *fat12;
    fat12 = fopen("fat12.img", "rb");

    BPB bpb{};
    BPB *bpb_ptr = &bpb;
    FAT fat{};

    fat.fillBPB(fat12, bpb_ptr); //读出引导区的结构

    //ROOT ENTRY
    RootEntry rootEntry{}; //根目录
    RootEntry *rootEntry_ptr = &rootEntry;
    fat.printFiles(fat12, rootEntry_ptr);
    return 0;
}




