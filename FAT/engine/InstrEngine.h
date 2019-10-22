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
     * */
    void execute_ls(string path, bool isDetail = false) {
        if ("/" == path) fat.printRoot();
        else {
            fat.printPathRecur(path, fat.fetchClusterEntry((char *) path.c_str()));
        }
    }

    /**
     * cat指令实现
     * */
    void execute_cat(string path) {
        RootEntry re = fat.fetchClusterEntry((char *) path.c_str());
        string content = fat.readFileContent(re.DIR_FstClus);
        cout << content << endl;
    }

private:
    FAT fat;

};


#endif //FAT_INSTRENGINE_H
