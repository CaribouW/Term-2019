
#include "engine/InstrEngine.h"

int main() {
    //文件引用
    InstrEngine i("test.img");
    i.execute_cat("DEMO/THIRD.TXT");
    i.execute_ls("DEMO/DIC");
    return 0;
}




