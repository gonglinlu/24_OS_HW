# 第4次课后练习

> 2021010862 龚林鹭

- 实验环境：rCore
- 第2-6题：ch4

#### 1. 批处理操作系统中，AppManager数据结构的组成（ch2）

```rust
// src/batch.rs
struct AppManager {
    num_app: usize,
    current_app: usize,
    app_start: [usize; MAX_APP_NUM + 1],
}
```

- `num_app: usize`：表示当前管理的应用程序数量。它是一个无符号整数类型（`usize`）。
- `current_app: usize`：表示当前正在运行的应用程序的索引。它也是一个无符号整数类型（`usize`）。
- `app_start: [usize; MAX_APP_NUM + 1]`：表示应用程序的起始地址数组。它是一个长度为 `MAX_APP_NUM + 1` 的数组，其中 `MAX_APP_NUM` 是一个常量，用于表示最大应用程序数量。数组的每个元素都是一个无符号整数类型（`usize`），用于存储相应应用程序的起始地址。

#### 2. AppManager数据结构的初始化过程（ch4）

##### 2.1 定义

```rust
// src/task/task.rs
pub struct TaskManager {
    /// total number of tasks
    num_app: usize,
    /// use inner value to get mutable access
    inner: UPSafeCell<TaskManagerInner>,
}

/// The task manager inner in 'UPSafeCell'
struct TaskManagerInner {
    /// task list
    tasks: Vec<TaskControlBlock>,
    /// id of current `Running` task
    current_task: usize,
}
```

##### 2.2 初始化

```rust
// src/task/mod.rs
lazy_static! {
    pub static ref TASK_MANAGER: TaskManager = {
        println!("init TASK_MANAGER");
        let num_app = get_num_app();
        println!("num_app = {}", num_app);
        let mut tasks: Vec<TaskControlBlock> = Vec::new();
        for i in 0..num_app {
            tasks.push(TaskControlBlock::new(get_app_data(i), i));
        }
        TaskManager {
            num_app,
            inner: unsafe {
                UPSafeCell::new(TaskManagerInner {
                    tasks,
                    current_task: 0,
                })
            },
        }
    };
}
```

- 调用 `get_num_app()` 函数获取应用程序的数量，并将结果赋值给 `num_app` 变量。
- 打印出 "num_app = {}" 的消息，用于显示获取到的应用程序数量。
- 创建一个空的 `Vec` 类型的 `tasks` 变量，用于存储应用程序的控制块。
- 使用 `for` 循环遍历应用程序的数量，并调用 `TaskControlBlock::new()` 函数来创建每个应用程序的控制块，并将其加入到 `tasks` 变量中。
- 创建一个 `TaskManager` 结构体实例，并使用 `num_app` 和 `tasks` 初始化 `TaskManager` 的内部状态。
- 将 `TaskManager` 实例包装在 `UPSafeCell` 中，并通过 `inner` 字段初始化 `TaskManager` 的静态变量 `TASK_MANAGER`。

#### 3. trapframe数据结构的组成（ch4）

````rust
// src/task/context.rs
pub struct TaskContext {
    /// Ret position after task switching
    ra: usize,
    /// Stack pointer
    sp: usize,
    /// s0-11 register, callee saved
    s: [usize; 12],
}
````

1. `ra: usize`：表示任务切换后的返回地址。返回地址是指当任务切换回来时，程序将继续执行的位置。它是一个无符号整数类型（`usize`）。
2. `sp: usize`：表示任务的栈指针。栈指针指向任务当前的栈顶位置，用于管理任务的函数调用栈。它是一个无符号整数类型（`usize`）。
3. `s: [usize; 12]`：表示任务的 s0 到 s11 寄存器的值。这些寄存器是被调用者保存的寄存器，用于保存函数调用过程中的临时变量。数组的每个元素都是一个无符号整数类型（`usize`），用于保存相应寄存器的值。

#### 4. 在系统调用过程中，trapframe数据结构的保存过程（ch4）

```assembly
# src/trap/trap.S
__alltraps:
    csrrw sp, sscratch, sp
    # now sp->*TrapContext in user space, sscratch->user stack
    # save other general purpose registers
    sd x1, 1*8(sp)
    # skip sp(x2), we will save it later
    sd x3, 3*8(sp)
    # skip tp(x4), application does not use it
    # save x5~x31
    .set n, 5
    .rept 27
        SAVE_GP %n
        .set n, n+1
    .endr
    # we can use t0/t1/t2 freely, because they have been saved in TrapContext
    csrr t0, sstatus
    csrr t1, sepc
    sd t0, 32*8(sp)
    sd t1, 33*8(sp)
    # read user stack from sscratch and save it in TrapContext
    csrr t2, sscratch
    sd t2, 2*8(sp)
    # load kernel_satp into t0
    ld t0, 34*8(sp)
    # load trap_handler into t1
    ld t1, 36*8(sp)
    # move to kernel_sp
    ld sp, 35*8(sp)
    # switch to kernel space
    csrw satp, t0
    sfence.vma
    # jump to trap_handler
    jr t1
```

1. `csrrw sp, sscratch, sp`：这行代码将当前栈指针 `sp` 的值保存到 `sscratch` 寄存器中，并将 `sp` 寄存器的值更新为之前保存在 `sscratch` 中的值。这样做是为了在用户态和内核态之间切换时，保存和恢复用户栈的状态。
2. 保存通用寄存器：代码中通过 `sd` 指令保存了一系列通用寄存器的值到用户栈中，这些寄存器包括 `x1`、`x3` 和 `x5` 到 `x31`，其中 `x1` 保存在栈偏移为 `1*8(sp)` 的位置，`x3` 保存在 `3*8(sp)` 的位置，而其他寄存器依次保存在 `4*8(sp)`、`5*8(sp)`、...、`34*8(sp)` 的位置。
3. 保存处理器状态：通过 `csrr` 指令读取 `sstatus` 和 `sepc` 寄存器的值，并通过 `sd` 指令保存到用户栈中的 `32*8(sp)` 和 `33*8(sp)` 的位置。这些寄存器保存了处理器的状态，包括当前的运行模式、中断使能状态以及当前指令的地址。
4. 保存用户栈：通过 `csrr` 指令读取 `sscratch` 寄存器的值，并通过 `sd` 指令保存到用户栈中的 `2*8(sp)` 的位置。这样做是为了保存用户栈的地址，以便在异常处理完成后恢复到用户态。
5. 切换到内核空间：通过 `ld` 指令从用户栈中加载 `kernel_satp`、`kernel_sp` 和 `trap_handler` 的值，并将 `satp` 寄存器设置为 `kernel_satp` 的值，`sp` 寄存器设置为 `kernel_sp` 的值。这样做是为了将处理器从用户态切换到内核态，并设置相应的内核栈和页表。
6. 跳转到异常处理程序：通过 `jr` 指令跳转到 `trap_handler` 寄存器中保存的异常处理程序的地址，开始执行异常处理过程。

#### 5. 在系统调用返回过程中的从trapframe数据结构恢复应用程序执行上下文的过程（ch4）

```assembly
# src/trap/trap.S
__restore:
    # a0: *TrapContext in user space(Constant); a1: user space token
    # switch to user space
    csrw satp, a1
    sfence.vma
    csrw sscratch, a0
    mv sp, a0
    # now sp points to TrapContext in user space, start restoring based on it
    # restore sstatus/sepc
    ld t0, 32*8(sp)
    ld t1, 33*8(sp)
    csrw sstatus, t0
    csrw sepc, t1
    # restore general purpose registers except x0/sp/tp
    ld x1, 1*8(sp)
    ld x3, 3*8(sp)
    .set n, 5
    .rept 27
        LOAD_GP %n
        .set n, n+1
    .endr
    # back to user stack
    ld sp, 2*8(sp)
    sret
```

1. 调用 `set_user_trap_entry()` 函数设置用户态陷阱入口地址。
2. 获取 trapframe 数据结构的指针 `trap_cx_ptr` 和用户地址空间的页表基址 `user_satp`。
3. 计算恢复函数 `__restore` 的虚拟地址 `restore_va`。
4. 使用汇编代码执行恢复操作，跳转到恢复函数的地址，并将参数传递给 `__restore` 函数进行恢复操作。

#### 6. 系统调用执行过程中的参数和返回值传递过程（ch4）

```rust
// src/trap/mod.rs
pub fn trap_handler() -> ! {
    // ...
    cx.x[10] = syscall(cx.x[17], [cx.x[10], cx.x[11], cx.x[12]]) as usize;
    // ...
}
```

- 事先将参数放入特定的寄存器 `a0`, `a1`, `a2`中。
- `ecall`时，触发`trap_handler()`，调用 `syscall()`，传入当前`TrapContext`的`x[]`中保存的参数。
- 系统调用函数执行完毕后，`trap_handler()`将返回值保存到`TrapContext`中的通用寄存器`a[0]`中。