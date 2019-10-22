//
// Created by mac on 2019/10/21.
//

#ifndef FAT_INSTRENGINE_H
#define FAT_INSTRENGINE_H

#include "../fat/fat.h"

//负责控制台指令的执行
class InstrEngine {
public:

    InstrEngine(char *s) {
        fat = FAT(s);
    }

    /**
     * ls指令实现
     * @param path: 目标路径, 开始结尾都没有/
     * @param isDetail: 是否是 -l
     * */
    void execute_ls(string path, bool isDetail = false) {
        if ("/" == path) fat.printRoot(isDetail);
        else fat.printPathRecur(path, fat.fetchClusterEntry((char *) path.c_str()), isDetail);
    }

    /**
     * cat指令实现
     * */
    void execute_cat(string path) {
        RootEntry re = fat.fetchClusterEntry((char *) path.c_str());
        string content = fat.readFileContent(re.DIR_FstClus);
        //print
        printf("%s\n", (char *) content.c_str());
    }

private:
    FAT fat;

};


#endif //FAT_INSTRENGINE_H
