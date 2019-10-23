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
//字符串分割函数
static std::vector<std::string> split(std::string str, std::string pattern) {
    std::string::size_type pos;
    std::vector<std::string> result;
    str += pattern;//扩展字符串以方便操作
    int size = str.size();

    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

static std::string trim(std::string &s) {
    if (s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
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

    RootEntry() {
        this->DIR_FstClus = -1;
    }
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

    /**
     * 包含递归查询
     * ls /NJU 和 cat a.txt 都会使用到该方法
     * 根据路径名称,以及是否是文件夹, 来获取文件的 cluster号
     * 如果文件大小超过512 byte ，那么就会包含多个cluster
     * TODO:
     * @param path: 首位.末位都不是/, 是否是文件夹已经预先设定好
     * @return : 返回cluster entry，如果文件/文件夹不存在，返回-1 的cluster
     * */
    RootEntry fetchClusterEntry(const char *path);

    /**
     * 子目录查询, 包含递归调用
     * */
    RootEntry fetchClusterEntry(vector<string> strings, int startClus);

    /**
     * 从根目录开始打印
     * */
    void printRoot(bool isDetail = false);

    /**
     * 递归打印路径
     * @param pre: path前缀 , 不包含最后一个 /
     * @param entry: 入口对象
     * */
    void printPathRecur(string pre, const RootEntry &entry, bool isDetail = false);

    /**
     * 递归打印路径, 并且给出具体的文件信息
     * 1. 在路径名后，冒号前，另输出此目录下直接子目录和直接子文件的数目
     * 2. 若项为目录，输出此目录下直接子目录和直接子文件的数目
     * 3. 若项为文件，输出文件的大小
     * */
    void printPathInDetail(string pre, const RootEntry &entry);

    /**
     * 读取文件内容
     * @param clusterNum: data 区cluster 号
     * */
    string readFileContent(const int clusterNum) {
        char *str = new char[SecPerClus * BytsPerSec];
        int startByte = fetchDataBaseInByte() + (clusterNum - 2) * SecPerClus * BytsPerSec;
        fseek(fat12_ptr, startByte, SEEK_SET);
        fread(str, 1, SecPerClus * BytsPerSec, fat12_ptr);
        return string(str);
    }

    string readFileContent(const RootEntry &entry) {
        int startByte = fetchDataBaseInByte() + (entry.DIR_FstClus - 2) * SecPerClus * BytsPerSec;
        fseek(fat12_ptr, startByte, SEEK_SET);
        int32_t itemNum = entry.DIR_FileSize > SecPerClus * BytsPerSec ? entry.DIR_FileSize : SecPerClus * BytsPerSec;
        char *str = new char[itemNum];
        fread(str, 1, itemNum, fat12_ptr);
        str[itemNum] = '\0';
        string ans(str);
        free(str);
        return ans;
    }

    /**
        * 根据entry , 获取其子目录的vector.
        * */
    vector<vector<RootEntry>> fetchPathSolution(const RootEntry &entry) {
        int dataBase = fetchDataBaseInByte();
        int currentClus = entry.DIR_FstClus;
        int value = 0;//value為十六進制數，每次存儲2/4個字節
        int ifOnlyDirectory = 0;
        //如果值大于或等于0xFF8，则表示当前簇已经是文件的最后一个簇了。如果值为0xFF7，表示它是一个坏簇
        vector<vector<RootEntry>> ans;
        vector<RootEntry> dicList;
        vector<RootEntry> fileList;

        while (value < 0xFF8) {
            value = getNextFatValue(fat12_ptr, currentClus);
            if (value == 0xFF7) {
                printf("bad cluster!Reading fails!\n");
                break;
            }
            int byteInCluster = SecPerClus * BytsPerSec; //每簇的字节数
            char *str = new char[byteInCluster]; //暂存从簇中读出的数据
            char *content = str;

            //-2是因为两个Reserv
            int startByte = dataBase + (currentClus - 2) * byteInCluster;
            //解析content中的数据,依次处理各个条目,目录下每个条目结构与根目录下的目录结构相同
            int loop = 0;
            while (loop < byteInCluster) {
                RootEntry entry{}; //根目录
                RootEntry *rootEntry_ptr = &entry;
                //read to the entry
                fseek(fat12_ptr, startByte + loop, SEEK_SET);
                fread(rootEntry_ptr, 1, 32, fat12_ptr);
                //此时获取新的entry
                if (!isValidPath(rootEntry_ptr->DIR_Name, 11)) {
                    loop += 32;
                    continue;
                }
                //添加结果
                char tmpStr[12];
                //如果是dic，那么就递归调用print
                validPathTransform(entry, tmpStr);
                if (tmpStr[0] != '.' && isDictory(entry.DIR_Attr))
                    dicList.emplace_back(entry);
                else if (tmpStr[0] != '.')
                    fileList.emplace_back(entry);
                loop += 32;
            }
            //来到下一个位置
            currentClus = value;
        }
        ans.emplace_back(dicList);
        ans.emplace_back(fileList);
        return ans;

    }

    int getNextFatValue(int num) {
        return getNextFatValue(fat12_ptr, num);
    }

private:
    void fillBPB(FILE *fat12, struct BPB *bpb_ptr);

    /**
     * 根据FAT表链表式的存储方式, 根据当前value (以sector为单位) 来获取下一个value (以sector为单位)
     * */
    int getNextFatValue(FILE *fat12, int num);

    /**
     * 获取root dic的起始位置
     * 以byte为单位
     * */
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

    /**
     * 获取data区内部某一个cluster的起始位置
     * 以byte为单位
     *
     * */
    int fetchClusterDataInByte(int cluster) {
        return (RsvdSecCnt + NumFATs * FATSz) * BytsPerSec + (cluster - 2) * (SecPerClus * BytsPerSec);
    }

    bool isValidPath(const char *path, const int maxLen) const {
        if (path[0] == '\0')return false;
        for (int i = 0; i < maxLen; ++i) {
            char ch = path[i];
            if (!(isalnum(ch) || isblank(ch) || ch == '.'))return false;
        }
        return true;
    }

    bool isDictory(const uint8_t attrCode) {
        return (attrCode & 0x10) != 0;
    }

    void validPathTransform(const RootEntry &re, char tempName[12]) const;


};
