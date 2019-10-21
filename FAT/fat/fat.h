#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
//data sector 从33 开始, 之前root dictory 占用了14 sectors
#pragma pack (1) /*指定按1字节对齐*/

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
        pFile = fopen("fat12.img", "rb");
        this->fat12_ptr = pFile;
        //set up BPB
        BPB bpb{};
        BPB *bpb_ptr = &bpb;
        fillBPB(fat12_ptr, bpb_ptr);
        this->bpb_ptr = bpb_ptr;
    }

    int getFATValue(int num) {
        return getFATValue(this->fat12_ptr, num);
    }

    void printRootFiles() {
        RootEntry rootEntry{}; //根目录
        RootEntry *rootEntry_ptr = &rootEntry;
        printFiles(this->fat12_ptr, rootEntry_ptr);
    }

private:
    void fillBPB(FILE *fat12, struct BPB *bpb_ptr);

    int getFATValue(FILE *fat12, int num);

    void printChildren(FILE *fat12, char *directory, int startClus);

    void printFiles(FILE *fat12, struct RootEntry *rootEntry_ptr);

    int fetchRootDicBaseInByte() {
        return (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec;
    }


    int fetchDataBaseInByte() {
        return BytsPerSec *
               (RsvdSecCnt + FATSz * NumFATs +
                (RootEntCnt * 32 + BytsPerSec - 1) / BytsPerSec);
    }
};
