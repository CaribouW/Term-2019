////
//// Created by mac on 2019/10/21.
////
//
//#ifndef FAT_FAT_H
//#define FAT_FAT_H
//
//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <cstdint>
//#include <cctype>
//#include <iostream>
//
////前11Bytes ignore
////偏移11个字节
//typedef unsigned char u8;//1字节
//typedef unsigned short u16;//2字节
//typedef unsigned int u32; //4字节
//struct BPB {
//    u16  BPB_BytsPerSec;
//    u8   BPB_SecPerClus;
//    u16  BPB_RsvdSecCnt;
//    u8   BPB_NumFATs=2;
//    u16  BPB_RootEntCnt;
//    u16  BPB_TotSec16;
//    u8   BPB_Media;
//    u16  BPB_FATSz16=9;
//    u16  BPB_SecPerTrk;
//    u16  BPB_NumHeads;
//    u32  BPB_HiddSec;
//    u32  BPB_TotSec32;
//};
////目录 or 文件的entry内容
//struct RootEntry {
//    union {
//        //2. If the first byte of the Filename field is 0xE5,
//        //then the directory entry is free (i.e., currently unused), and hence there is no file
//        //or subdirectory associated with the directory entry.
//        //3. If the first byte of the Filename field is 0x00, then this directory entry is free and all the remaining directory entries in this directory are also free.
//        char fullPath[11];    //3+8
//        struct {
//            char name[8];
//            char ext[3];
//        };
//    };
//    u8 DIR_Attr;    //文件属性
//    u16 reserved;   //保留
//    u16 DIR_CreateTime; //创建时间
//    u16 DIR_CreateDate; //创建date
//    u16 DIR_LastAccessDate;
//    u16 Ignore;         //
//    u16 DIR_WrtTime;    //写时间
//    u16 DIR_WrtDate;    //写date
//    //(physical sector number) = 33 + (FAT entry number) - 2
//    u16 DIR_FstClus;    //开始簇号
//    u32 DIR_FileSize;   //File size大小 Bytes
//};
//
//
////打印子目录
//void fillBPB(FILE *fat12, BPB *bpb_ptr);
//
//// 打印当前 '.' 目录下的所有文件 file
//void printFiles(FILE *fat12, struct RootEntry *rootEntry_ptr);
//
//void printChildren(FILE *fat12, char *directory, int startClus);
//
//int getFATValue(FILE *fat12, int num);
//
//#endif //FAT_FAT_H
