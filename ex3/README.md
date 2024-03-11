# Ex3

- 题目要求：
    - 写一个函数`print_stackframe()`，用于获取当前位置的函数调用栈信息。
    - 实现获取以下**一种或多种**信息的功能：函数入口地址、函数名、参数调用参数、返回值。


- 操作系统：Linux(WSL2)

- 编程语言：C++

- 代码文件：`print_stackframe.cpp`

- 运行方式：
    ```
    g++ -g -rdynamic print_stackframe.cpp -o print_stackframe
    ./print_stackframe
    ```

- 输出示例：
    ```
    ###Start of the program###
    #0: ./print_stackframe(_Z14printStackInfov+0x53) [0x557d08ec64fc]
    #1: ./print_stackframe(_Z5func3NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE+0x15) [0x557d08ec6601]
    #2: ./print_stackframe(_Z5func2NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE+0x52) [0x557d08ec6656]
    #3: ./print_stackframe(_Z5func1NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE+0x52) [0x557d08ec6710]
    #4: ./print_stackframe(main+0x79) [0x557d08ec67f1]
    #5: /lib/x86_64-linux-gnu/libc.so.6(+0x29d90) [0x7f253e5dbd90]
    #6: /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80) [0x7f253e5dbe40]
    #7: ./print_stackframe(_start+0x25) [0x557d08ec63e5]
    ###End of the program###
    ```

- 结果分析
    - ###Start of the program### 和 ###End of the program###：是我添加的调试输出，指示了整个打印程序的开始和结束。
    - #0: ./print_stackframe(_Z14printStackInfov+0x53) [0x557d08ec64fc]：这行表示程序中的第一个栈帧。#0 表示栈帧的序号，./print_stackframe 是可执行文件的名称，_Z14printStackInfov 是函数名，+0x53 是函数内的偏移量，[0x557d08ec64fc] 是函数的地址。
    - #1: ./print_stackframe(_Z5func3NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE+0x15) [0x557d08ec6601]：类似地，这行表示第二个栈帧，其中的函数名是 func3，具有参数 std::__cxx11::basic_string。
    - #2: ./print_stackframe(_Z5func2NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE+0x52) [0x557d08ec6656]：这行表示第三个栈帧，函数名为 func2。
    - #3: ./print_stackframe(_Z5func1NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE+0x52) [0x557d08ec6710]：这行表示第四个栈帧，函数名为 func1。
    - #4: ./print_stackframe(main+0x79) [0x557d08ec67f1]：这行表示 main 函数的栈帧，具有偏移量 +0x79。
    - #5: /lib/x86_64-linux-gnu/libc.so.6(+0x29d90) [0x7f253e5dbd90]：这行表示 libc 库中的栈帧。
    - #6: /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80) [0x7f253e5dbe40]：这行表示 __libc_start_main 函数的栈帧。
    - #7: ./print_stackframe(_start+0x25) [0x557d08ec63e5]：这行表示程序的入口点 _start 函数的栈帧。