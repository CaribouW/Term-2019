#include "fat/fat.h"

int main() {
    //文件引用
    FAT fat("fat12.img");

//    FILE *fat12;
//    fat12 = fopen("fat12.img", "rb");
//
//    BPB bpb{};
//    BPB *bpb_ptr = &bpb;
//    FAT fat{};
//
//    fat.fillBPB(fat12, bpb_ptr); //读出引导区的结构
//
//    //ROOT ENTRY
    fat.printRootFiles();
    return 0;
}




