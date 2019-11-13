//
// Created by mac on 2019/10/21.
//

#ifndef FAT_INSTRENGINE_H
#define FAT_INSTRENGINE_H

#include "../fatFile/fat.h"

//负责控制台指令的执行
class InstrEngine {
public:

    InstrEngine(const char *s) {
        fat = FAT(s);
    }

    /**
     * ls指令实现
     * @param path: 目标路径, 开始结尾都没有/
     * @param isDetail: 是否是 -l
     * */
    void execute_ls(string path, bool isDetail = false) {
        if ("/" == path) fat.printRoot(isDetail);
        else {
            RootEntry re = fat.fetchClusterEntry((char *) path.c_str());
            if (0 == re.DIR_FstClus) {
                printf(FILE_NOT_FOUND);
                return;
            }
            fat.printPathRecur(path, re, isDetail);
        }
    }

    /**
     * cat指令实现
     * */
    void execute_cat(string path) {
        RootEntry re = fat.fetchClusterEntry((char *) path.c_str());
        if (0 == re.DIR_FstClus) {
            printf(FILE_NOT_FOUND);
            return;
        } else if (fat.isDictory(re.DIR_Attr)) {
            printf("**Error: The target is not a file name.You should give a valid file path!\n");
            return;
        }
        //while (value < 0xFF8) 如果文件容量过大，那么就分到多个sec里面
        string content = fat.readFileContent(re);
        //print
        printf(content.c_str());
    }

private:
    FAT fat;

};


#endif //FAT_INSTRENGINE_H
