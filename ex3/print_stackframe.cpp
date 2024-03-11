#include <iostream>
#include <execinfo.h>
using namespace std;

void printStackInfo()
{
    int const max_frames = 16; // 最多打印16层调用栈
    void* buffer[max_frames] = {nullptr}; // 用于存放调用栈信息
    int count = backtrace(buffer, max_frames); // 获取调用栈
    char** symbols = backtrace_symbols(buffer, count); // 将调用栈信息转成字符串

    for (int i = 0; i < count; i++) {
        // 打印每一层调用栈信息
        cout << "#" << i << ": " << symbols[i] << endl;
    }

    free(symbols); // 释放分配的内存
}

void func3(string s) { printStackInfo(); }
void func2(string s) { func3("Thisisfunc3"); }
void func1(string s) { func2("Thisisfunc2"); }

int main() { 
    cout << "###Start of the program###" << endl;
    func1("Thisisfunc1");
    cout << "###End of the program###" << endl;
    return 0;
}