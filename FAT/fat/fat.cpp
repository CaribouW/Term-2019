//
// Created by mac on 2019/10/21.
//

#include "fat.h"
#include <cctype>

//判定是否是有效字符 数字、字母、空格
static bool isValidPath(const char ch) {
    return isalnum(ch) || isblank(ch);
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


void FAT::printChildren(FILE *fat12, char *directory, int startClus) {
    //数据区的第一个簇（即2号簇）的偏移字节
    int dataBase = fetchDataBaseInByte();
    //存放文件路径及全名
    char fullName[24];
    int strLength = strlen(directory);
    strcpy(fullName, directory);
    fullName[strLength] = '/';
    strLength++;
    fullName[strLength] = '\0';
    char *fileName = &fullName[strLength];

    int currentClus = startClus;
    int value = 0;//value為十六進制數，每次存儲2/4個字節
    int ifOnlyDirectory = 0;
    //如果值大于或等于0xFF8，则表示当前簇已经是文件的最后一个簇了。如果值为0xFF7，表示它是一个坏簇
    while (value < 0xFF8) {
        value = getNextFatValue(fat12, currentClus);
        if (value == 0xFF7) {
            printf("bad cluster!Reading fails!\n");
            break;
        }
        int byteInCluster = SecPerClus * BytsPerSec; //每簇的字节数
        char *str = new char[byteInCluster]; //暂存从簇中读出的数据
        char *content = str;

        //-2是因为两个Reserv
        int startByte = dataBase + (currentClus - 2) * byteInCluster;
        int check;
        check = fseek(fat12, startByte, SEEK_SET);
        if (check == -1)
            printf("fseek in printChildren failed!");

        check = fread(content, 1, byteInCluster, fat12);
        if (check != byteInCluster)
            printf("fread in printChildren failed!");

        //解析content中的数据,依次处理各个条目,目录下每个条目结构与根目录下的目录结构相同
        int loop = 0;
        while (loop < byteInCluster) {
            char tempName[12];  //暂存替换空格为点后的文件名
            if (content[loop] == '\0') {
                loop += 32;
                continue;
            }   //空条目不输出
            //过滤非目标文件
            int pathInvalid = 0;
            for (int j = loop; j < loop + 11; j++) {
                if (!isValidPath(content[j])) {
                    pathInvalid = 1;    //非英文及数字、空格
                    break;
                }
            }
            if (pathInvalid == 1) {
                loop += 32;
                continue;
            }   //非目标文件不输出
            int tempLong = -1;
            for (int k = 0; k < 11; k++) {
                if (content[loop + k] != ' ') {
                    tempName[++tempLong] = content[loop + k];
                } else {
                    tempName[++tempLong] = '.';
                    while (content[loop + k] == ' ') k++;
                    k--;
                }
            }
            tempLong++;
            tempName[tempLong] = '\0';  //到此为止，把文件名提取出来放到tempName里

            strcpy(fileName, tempName);
            printf("%s\n", fullName);
            ifOnlyDirectory = 1;
            loop += 32;
        }
        free(str);
        //
        currentClus = value;
    }
    if (ifOnlyDirectory == 0)
        printf("%s\n", fullName);  //空目录的情况下，输出目录
}

void FAT::printFiles(FILE *fat12, struct RootEntry *rootEntry_ptr) {
    int base = fetchRootDicBaseInByte(); //根目录首字节的偏移数
    int check;
    char realName[12];  //暂存将空格替换成点后的文件名

    //依次处理根目录中的各个条目
    for (int i = 0; i < RootEntCnt; i++) {

        check = fseek(fat12, base, SEEK_SET);
        if (check == -1)
            printf("fseek in printFiles failed!");

        check = fread(rootEntry_ptr, 1, 32, fat12);
        if (check != 32)
            printf("fread in printFiles failed!");

        base += 32;

        if (rootEntry_ptr->DIR_Name[0] == '\0') continue;     //空条目不输出

        //过滤非目标文件
        int boolean = 0;
        for (char ch : rootEntry_ptr->DIR_Name) {
            if (!isValidPath(ch)) {
                boolean = 1;    //非英文及数字、空格
                break;
            }
        }
        if (boolean == 1) continue;  //非目标文件不输出
        //文件
        if ((rootEntry_ptr->DIR_Attr & 0x10) == 0) {
            int tempLong = -1;
            for (int k = 0; k < 11; k++) {
                if (rootEntry_ptr->DIR_Name[k] != ' ') {
                    tempLong++;
                    realName[tempLong] = rootEntry_ptr->DIR_Name[k];
                } else {
                    tempLong++;
                    realName[tempLong] = '.';
                    while (rootEntry_ptr->DIR_Name[k] == ' ') k++;
                    k--;
                }
            }
            tempLong++;
            realName[tempLong] = '\0';  //到此为止，把文件名提取出来放到了realName里

            //输出文件
            printf("%s\n", realName);
        } else {
            //目录
            int tempLong = -1;
            for (char k : rootEntry_ptr->DIR_Name) {
                if (k != ' ') {
                    tempLong++;
                    realName[tempLong] = k;
                } else {
                    tempLong++;
                    realName[tempLong] = '\0';
                    break;
                }
            }   //到此为止，把目录名提取出来放到了realName

            //输出目录及子文件
            printChildren(fat12, realName, rootEntry_ptr->DIR_FstClus);
        }
    }
}