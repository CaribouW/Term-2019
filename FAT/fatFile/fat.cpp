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

RootEntry FAT::fetchClusterEntry(const char *path) {
    RootEntry entry{}; //根目录
    RootEntry *rootEntry_ptr = &entry;
    string strPath(path);
    //字符串分隔, 最终strings的长度size和目录级别 N : N =
    vector<string> strings = split(strPath, "/");
    //针对第0个进行分析比较
    string target = strings[0];
    int base = fetchRootDicBaseInByte();
    char realName[12];  //暂存将空格替换成点后的文件名
    for (int i = 0; i < RootEntCnt; ++i) {
        fseek(fat12_ptr, base, SEEK_SET);
        fread(rootEntry_ptr, 1, 32, fat12_ptr);
        base += 32;
        if (!isValidPath(rootEntry_ptr->DIR_Name, 11))
            continue;
        //是文件
        validPathTransform(*rootEntry_ptr, realName);
        //根目录下的file
        if (target == realName && strings.size() == 1)
            return *rootEntry_ptr;
            //属于子文件夹查询
        else if (strings.size() > 1) {
            //获取子数组
            strings = vector<string>(strings.begin() + 1, strings.end());
            return fetchClusterEntry(strings, rootEntry_ptr->DIR_FstClus);
        }
    }
    //没找到
    return RootEntry{};
}

RootEntry FAT::fetchClusterEntry(vector<string> strings, int startClus) {
    if (0 == strings.size())return RootEntry{};
    string target = strings[0];
    int dataBase = fetchDataBaseInByte();
    int curClus = startClus;
    int value = 0;
    while (value < 0xFF8) {
        value = getNextFatValue(fat12_ptr, curClus);
        if (value == 0xFF7) {
            printf("bad cluster!Reading fails!\n");
            break;
        }
        int byteInCluster = SecPerClus * BytsPerSec; //每簇的字节数
        char *str = new char[byteInCluster]; //暂存从簇中读出的数据
        char *content = str;
        int startByte = dataBase + (curClus - 2) * byteInCluster;
        fseek(fat12_ptr, startByte, SEEK_SET);
        fread(content, 1, byteInCluster, fat12_ptr);
        int loop = 0;
        while (loop < byteInCluster) {
            RootEntry entry{}; //根目录
            RootEntry *rootEntry_ptr = &entry;
            //read to the entry
            fseek(fat12_ptr, startByte + loop, SEEK_SET);
            fread(rootEntry_ptr, 1, 32, fat12_ptr);
            if (!isValidPath(rootEntry_ptr->DIR_Name, 11)) {
                loop += 32;
                continue;
            }
            char tempName[12];  //暂存替换空格为点后的文件名
            validPathTransform(*rootEntry_ptr, tempName);
            if (target == tempName && 1 == strings.size()) {
                return *rootEntry_ptr;
            } else if (target == tempName && strings.size() > 1) {
                //获取子数组
                strings = vector<string>(strings.begin() + 1, strings.end());
                return fetchClusterEntry(strings, rootEntry_ptr->DIR_FstClus);
            }
            loop += 32;
        }
        //来到下一个位置
        curClus = value;
    }
    return RootEntry{};
}

void FAT::printPathRecur(string pre, const RootEntry &entry, bool isDetail) {
    if (!isDetail) {
        //如果是单个文件文件
        char name[12];
        if (!isDictory(entry.DIR_Attr)) {
            validPathTransform(entry, name);
            printf("%s/%s  ", (char *) pre.c_str(), name);
            return;
        }
        //文件夹
        auto sub = fetchPathSolution(entry);

        //输出前缀
        printf("%s/:\n", (char *) pre.c_str());
        printf("\033[31m.  ..  \033[0m");
        //sub[0]为diclist, sub[1]为fileList
        for (const auto e:sub[0]) {
            validPathTransform(e, name);
            printf("\033[31m%s/%s  \033[0m", (char *) pre.c_str(), name);
//            printf("%s/%s  ", (char *) pre.c_str(), name);
        }
        for (const auto e:sub[1]) {
            validPathTransform(e, name);
            printf("%s/%s  ", (char *) pre.c_str(), name);
        }
        printf("\n");
        //递归
        for (const auto e:sub[0]) {
            validPathTransform(e, name);
            printPathRecur(pre + '/' + name, e);
        }
        printf("\n");
    } else {
        char name[12];
        //单个文件，输出size
        if (!isDictory(entry.DIR_Attr)) {
            validPathTransform(entry, name);
            printf("%s/%s  %u", (char *) pre.c_str(), name, entry.DIR_FileSize);
            return;
        }
        //文件夹
        auto sub = fetchPathSolution(entry);
        //输出前缀
        printf("%s/ %lu %lu:\n", (char *) pre.c_str(), sub[0].size(), sub[1].size());
        printf("\033[31m.\n..\033[0m\n");
        for (const auto e:sub[0]) {
            //dic
            validPathTransform(e, name);
            auto lists = fetchPathSolution(e);
            printf("\033[31m%s/%s\033[0m  %lu %lu\n", (char *) pre.c_str(), name, lists[0].size(), lists[1].size());
        }
        for (const auto e:sub[1]) {
            validPathTransform(e, name);
            printf("%s/%s  %u\n", (char *) pre.c_str(), name, e.DIR_FileSize);
        }
        //递归
        for (const auto e:sub[0]) {
            validPathTransform(e, name);
            printPathRecur(pre + '/' + name, e, true);
        }
    }
}

void FAT::printRoot(bool isDetail) {
    RootEntry entry{}; //根目录
    RootEntry *rootEntry_ptr = &entry;
    int base = fetchRootDicBaseInByte();
    char realName[12];  //暂存将空格替换成点后的文件名
    vector<RootEntry> dicList;
    vector<RootEntry> fileList;
    for (int i = 0; i < RootEntCnt; ++i) {
        fseek(fat12_ptr, base, SEEK_SET);
        fread(rootEntry_ptr, 1, 32, fat12_ptr);
        base += 32;
        if (!isValidPath(rootEntry_ptr->DIR_Name, 11))
            continue;
        //是文件
        validPathTransform(*rootEntry_ptr, realName);
        if (isDictory(rootEntry_ptr->DIR_Attr))
            dicList.emplace_back(*rootEntry_ptr);
        else {
            fileList.emplace_back(*rootEntry_ptr);
        }
    }
    //==================
    //for print
    if (!isDetail) {
        printf("/:\n");
        for (int i = 0; i < fileList.size(); ++i) {
            validPathTransform(fileList[i], realName);
            printf("%s  ", realName);
        }
        for (int i = 0; i < dicList.size(); ++i) {
            validPathTransform(dicList[i], realName);
            printf("\033[31m%s  \033[0m", realName);
        }
        printf("\n");
        for (int i = 0; i < dicList.size(); ++i) {
            validPathTransform(dicList[i], realName);
            string pre = "/" + string(realName);
            printPathRecur(pre, dicList[i]);
        }
        printf("\n");
    } else {
        int dicNum = dicList.size(), fileNum = fileList.size();
        printf("/ %d %d:\n", dicNum, fileNum);
        //每一个dic都需要计算
        for (int i = 0; i < fileList.size(); ++i) {
            validPathTransform(fileList[i], realName);
            printf("%s  %u\n", realName, fileList[i].DIR_FileSize);
        }
        for (int i = 0; i < dicList.size(); ++i) {
            validPathTransform(dicList[i], realName);
            //根据realName获取vector
            auto lists = fetchPathSolution(fileList[i]);
            printf("\033[31m%s  \033[0m%lu %lu\n", realName, lists[0].size(), lists[1].size());
        }
        //递归
        for (int i = 0; i < dicList.size(); ++i) {
            validPathTransform(dicList[i], realName);
            string pre = "/" + string(realName);
            printPathRecur(pre, dicList[i], true);
        }
    }
    //没找到
}

