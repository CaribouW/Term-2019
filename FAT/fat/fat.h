#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cctype>
#include <iostream>
#include <vector>
//data sector 从33 开始, 之前root dictory 占用了14 sectors
#pragma pack (1) /*指定按1字节对齐*/
using namespace std;

typedef unsigned char u8;//1字节
typedef unsigned short u16;//2字节
typedef unsigned int u32; //4字节

//(physical sector number) = 33 + (FAT entry number) - 2

//前11Bytes ignore
//偏移11个字节
struct BPB {
    u16 BPB_BytsPerSec;     //每扇区字节数（2B）
    u8 BPB_SecPerClus;      //每簇扇区数 1
    u16 BPB_RsvdSecCnt;     //Boot记录占用的扇区数 2
    u8 BPB_NumFATs = 2;     //FAT表个数 1
    u16 BPB_RootEntCnt;     //根目录最大文件数 2
    u16 BPB_TotSec16;       //2
    u8 BPB_Media;           // 1
    u16 BPB_FATSz16 = 16;    //FAT扇区数 2
    u16 BPB_SecPerTrk;      // 2
    u16 BPB_NumHeads;       // 2
    u32 BPB_HiddSec;        // 4
    u32 BPB_TotSec32;       //如果BPB_FATSz16为0，该值为FAT扇区数
};

//根目录条目
struct RootEntry {
    union {
        char DIR_Name[11];  //File_name & Extension
        struct {
            char name[8];   //8Bytes for the name
            char ext[3];    // extension
        };
    };
    u8 DIR_Attr;        //文件属性
    char reserved[10];
    u16 DIR_WrtTime;
    u16 DIR_WrtDate;
    u16 DIR_FstClus;    //开始簇号
    u32 DIR_FileSize;   //File size大小 Bytes
};

class FAT {
public:
    FILE *fat12_ptr;
    BPB *bpb_ptr;
    int BytsPerSec;     //每扇区字节数
    int SecPerClus;     //每簇扇区数
    int RsvdSecCnt;     //Boot记录占用的扇区数
    int NumFATs;        //FAT表个数
    int RootEntCnt;     //根目录最大文件数
    int FATSz;          //FAT扇区数
    int TotSec;         //扇区数总数
    int FstClus; //开始簇号
    int FileSize;//文件大小
    int NextCluser;

    FAT() {}

    FAT(char *str) {
        FILE *pFile;
        pFile = fopen(str, "rb");
        this->fat12_ptr = pFile;
        //set up BPB
        BPB bpb{};
        BPB *bpb_ptr = &bpb;
        fillBPB(fat12_ptr, bpb_ptr);
        this->bpb_ptr = bpb_ptr;
    }

    int getFATValue(int num) {
        return getNextFatValue(this->fat12_ptr, num);
    }

    void printRootFiles() {
        RootEntry rootEntry{}; //根目录
        RootEntry *rootEntry_ptr = &rootEntry;
        printFiles(this->fat12_ptr, rootEntry_ptr);
    }

/**
     * 输出目录下文件
     * @param directory: 目录名称,仅仅是作为输出的时候的前缀
     * @param startClus: 开始簇,作为链表形式进行输出
     * */
    void printChildren(FILE *fat12, string directory, int startClus);

private:
    void fillBPB(FILE *fat12, struct BPB *bpb_ptr);

    /**
     * 根据FAT表链表式的存储方式, 根据当前value (以sector为单位) 来获取下一个value (以sector为单位)
     * */
    int getNextFatValue(FILE *fat12, int num);


    void printFiles(FILE *fat12, struct RootEntry *rootEntry_ptr);


    int fetchRootDicBaseInByte() {
        return (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec;
    }

    /**
     * 获取数据data区的起始位置
     * 以byte为单位
     * */
    int fetchDataBaseInByte() {
        //之所以分子要加上(BPB_BytsPerSec-1)，是为了保证此公式在根目录区无法填满整数扇区时仍然成立
        return BytsPerSec *
               (RsvdSecCnt + FATSz * NumFATs +
                (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
    }

    bool isValidPath(const char *path, const int maxLen) const {
        if (path[0] == '\0')return false;
        for (int i = 0; i < maxLen; ++i) {
            char ch = path[i];
            if (!(isalnum(ch) || isblank(ch)))return false;
        }
        return true;
    }

    bool isDictory(const uint8_t attrCode) {
        return (attrCode & 0x10) != 0;
    }

    void validPathTransform(const RootEntry &re, char tempName[12], const bool isDic = false) const;
};
