//
// Created by mac on 2019/10/21.
//

#include "fat.h"

static void str_cp(char *s1, const char *s2) {
    int n = strlen(s2);
    for (int i = 0; i < n; ++i) {
        s1[i] = s2[i];
    }
}

void FAT::fillBPB(FILE *fat12, struct BPB *bpb_ptr) {
    int32_t index;
    //偏移量
    uint8_t offset = 11;
    index = fseek(fat12, offset, SEEK_SET);

    if (-1 == index) {
        printf("fseek in fillBPB failed!\n");
    }
    index = fread(bpb_ptr, 1, 25, fat12);

    if (index != 25) {
        printf("fread in fillBPB failed!");
    }
    //set
    BytsPerSec = bpb_ptr->BPB_BytsPerSec;
    SecPerClus = bpb_ptr->BPB_SecPerClus;
    RsvdSecCnt = bpb_ptr->BPB_RsvdSecCnt;
    NumFATs = bpb_ptr->BPB_NumFATs;
    RootEntCnt = bpb_ptr->BPB_RootEntCnt;
    FATSz = bpb_ptr->BPB_FATSz16;
}

int FAT::getNextFatValue(FILE *fat12, int num)   //NUM=开始簇号DIR_FstClus
{
    //FAT1的偏移字节
    int fatBase = RsvdSecCnt * BytsPerSec;
    //FAT项的偏移字节
    int fatPos = fatBase + num * 3 / 2;
    //奇偶FAT项处理方式不同，分类进行处理，从0号FAT项开始
    int type = 0;
    if (num % 2 == 0) {
        type = 0;
    } else {
        type = 1;
    }

    //先读出FAT项所在的两个字节
    u16 bytes;
    u16 *bytes_ptr = &bytes;
    int check;
    check = fseek(fat12, fatPos, SEEK_SET);
    if (check == -1)
        printf("fseek in getFATValue failed!");

    check = fread(bytes_ptr, 1, 2, fat12);
    if (check != 2)
        printf("fread in getFATValue failed!");

    //u16为short，结合存储的小尾顺序和FAT项结构可以得到
    //type为0的话，取byte2的低4位和byte1构成的值，type为1的话，取byte2和byte1的高4位构成的值
    if (type == 0) {
        return bytes << 4;
    } else {
        return bytes >> 4;
    }
}

void FAT::validPathTransform(const RootEntry &re, char tempName[12]) const {
    if (re.name[0] == '.')tempName[0] = re.name[0];
    if (re.name[1] == '.')tempName[1] = re.name[1];
    if (re.name[0] == '.' && re.name[1] != '.') {
        tempName[1] = '\0';
        return;
    } else if (re.name[0] == '.' && re.name[1] != '.') {
        tempName[2] = '\0';
        return;
    }
    if (re.ext[0] == ' ') {
        int tempLong = -1;
        for (char k : re.DIR_Name) {
            if (k != ' ') {
                tempName[++tempLong] = k;
            } else {
                break;
            }
        }
        tempName[++tempLong] = '\0';  //到此为止，把文件名提取出来放到tempName里
    } else {
        int tempLong = -1;
        for (int k = 0; k < 11; k++) {
            if (re.DIR_Name[k] != ' ') {
                tempName[++tempLong] = re.DIR_Name[k];
            } else {
                if ((re.DIR_Attr & 0x10) == 0)
                    tempName[++tempLong] = '.';
                while (re.DIR_Name[k] == ' ') k++;
                k--;
            }
        }
        tempName[++tempLong] = '\0';  //到此为止，把文件名提取出来放到tempName里
    }

}
