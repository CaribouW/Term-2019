
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


int main() {
    //文件引用
    printf("Command begin\nread from {test.img} file\n");
    InstrEngine i("test.img");
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
            i.execute_ls("/");
        } else if (regex_match(line, ls_detail_default)) {
            i.execute_ls("/", true);
        } else if (regex_match(line, cat_pattern)) {
            //去除首位的 /
            string path = regex_replace(comList[1], regex("^/|/$"), "");
            i.execute_cat(path);
        } else if (regex_match(line, ls_simple_file)) {
            //ls <file>
            string path = regex_replace(comList[1], regex("^/|/$"), "");
            i.execute_ls(path);
        } else if (regex_match(line, ls_detail_file)) {
            //ls -l <file>
            string path = regex_replace(comList[1], regex("^/|/$"), "");
            i.execute_ls(path, true);
        } else {
            printf("**Error:command not legal!\n");
        }
        printf("> ");
        cin.getline(input, 1024);
        line = string(input);
    }
    return 0;
}




