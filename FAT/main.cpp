
#include "engine/InstrEngine.h"
#include <regex>

//ls
static auto ls_default = regex("ls[\\s]*");
//ls -l
static auto ls_detail_default = regex("ls([\\s][\\-][l]+)+");
//cat <file-name>
static auto cat_pattern = regex("cat[\\s]+[/0-9a-zA-Z\\.]+");
//ls <file-name>
static auto ls_simple_file = regex("ls[\\s][/0-9a-zA-Z\\.]+");
//ls -l <file-name>
static auto ls_detail_file = regex("(ls([\\s][\\-][l]+)+[\\s][/0-9a-zA-Z\\.]+)|"
                                   "(ls[\\s][/0-9a-zA-Z\\.]+([\\s][\\-][l]+)+)|"
                                   "(ls([\\s][\\-][l]+)+[\\s][/0-9a-zA-Z\\.]+([\\s][\\-][l]+)+)");
//异常处理内容
//包括 : 文件 or 文件夹不存在
//  指令格式出错
//      1. 指令本身格式不规范---正则
//      2. 即使正确格式 : cat 后面不可以跟文件夹 / ls 后面不可以跟文件

int main() {
    const char *path = new char[1024]{"ref.img"};
    //文件引用
    printf("Command begin\n");
    InstrEngine engine(path);
    delete[] path;
    char *input = new char[1024];
    printf("> ");
    cin.getline(input, 1024);
    string line = string(input);
    while (line != "q") {
        //预先删除多余的空格
        line = regex_replace(line, regex("\\s+"), " ");
        line = trim(line);
        vector<string> comList = split(line, " ");
        //ls stage
        if (regex_match(line, ls_default)) {
            engine.execute_ls("/");
        } else if (regex_match(line, ls_detail_default)) {
            engine.execute_ls("/", true);
        } else if (regex_match(line, cat_pattern)) {
            //去除首位的 /
            string path = regex_replace(comList[1], regex("^/|/$"), "");
            engine.execute_cat(path);
        } else if (regex_match(line, ls_simple_file)) {
            //ls <file>
            string path = regex_replace(comList[1], regex("^/|/$"), "");
            engine.execute_ls(path);
        } else if (regex_match(line, ls_detail_file)) {
            //ls -l <file>
            uint32_t index;
            for (index = 1; index < comList.size(); ++index) {
                if (comList[index][0] != '-')break;
            }
            string path = regex_replace(comList[index], regex("^/|/$"), "");

            engine.execute_ls(path, true);
        } else if (line == "clear") {
            system("clear");
        } else {
            printf("%s\n", COMMAND_ILLEGAL);
        }
        printf("> ");
        cin.getline(input, 1024);
        line = string(input);
    }
    free(input);
    printf("Good bye!\n");
    return 0;
}




