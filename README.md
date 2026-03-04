# C/C++ Coursework Projects Collection

这是我大学期间的课程练习与小项目合集，主要包含 `C/C++`、数据结构与算法、操作系统实验，以及若干算法题实现。

## Environment

- Windows 10/11
- Visual Studio 2019/2022（多数工程为 `.sln + .vcxproj`）
- 少量 C 文件可直接用 `gcc/clang` 编译运行

## Repository Structure

```text
cpp/
├─ OS/                    # 操作系统实验与课设
├─ WTA/                   # Weapon-Target Assignment（优化/仿真）
├─ 哈希表/                 # C 语言哈希表与链表实现
├─ socket/                # 简易 HTTP Socket 服务端
├─ Project1~Project14/    # 课程阶段性练习（多为算法题）
├─ magic/ dead day/ ...   # 独立小作业
└─ visual code cpp/       # 早期 C 语言 Hello World
```

## Project Index

| Folder | Topic | Main Source |
|---|---|---|
| `OS/实验一` | 多级调度模拟（RQ1: 时间片轮转，RQ2: 短作业优先） | `OS/实验一/Project1/多级调度.cpp` |
| `OS/实验二` | 银行家算法安全性检查 | `OS/实验二/Project2/FileName.cpp` |
| `OS/实验三` | 动态分区存储管理（首次/最佳/最坏适应） | `OS/实验三/Project3/动态分区式存贮区管理.cpp` |
| `OS/实验四` | 页面置换算法（FIFO/LRU） | `OS/实验四/Project4/置换算法.cpp` |
| `WTA` | 武器目标分配（WTA）问题建模与求解、结果可视化数据导出 | `WTA/WTA/wta.cpp` |
| `哈希表` | C 语言链表 + 哈希表基础实现 | `哈希表/哈希表/HASH.c` |
| `socket` | 跨平台简易 HTTP 服务器示例（Win/Linux 条件编译） | `socket/socket/FileName.cpp` |
| `Project2` | 顺序表（动态扩容、内存分配容错） | `Project2/Project2/FileName.cpp` |
| `Project4` | 8 数码求解（启发式搜索） | `Project4/Project4/FileName.cpp` |
| `Project6` | 8 数码求解（含扩展节点/生成节点统计） | `Project6/Project6/FileName.cpp` |
| `Project7` | 点在三角形内判断（批量输入版） | `Project7/Project7/main.cpp` |
| `Project8` | 点在三角形内判断（支持文件输入） | `Project8/Project8/解法二.cpp` |
| `Project10` | 点在多种几何体内判断（含边界选项） | `Project10/Project10/解法二.cpp` |
| `Project11` | 几何位置判断增强实现（含边界判断） | `Project11/Project11/解法二.cpp` |
| `Project12` | 几何判断过程可解释版本（含详细输出） | `Project12/Project12/解法二.cpp` |
| `Project13` | 几何判断流程化输出版本 | `Project13/Project13/FileName.cpp` |
| `dead day` | 作业调度贪心（按利润排序 + 截止时间安排） | `dead day/dead day/FileName.cpp` |
| `magic` | 状态压缩 + DFS + 哈希剪枝的概率计算实验 | `magic/magic/FileName.cpp` |
| `2024.4.16` | 入门测试程序（Hello World） | `2024.4.16/2024.4.16/1.cpp` |
| `visual code cpp` | C 语言入门程序（Hello World） | `visual code cpp/hello.c` |

> 说明：部分目录（如 `Project3/5/9/14`、`OS/算法课设`）当前仅有工程文件或内容未整理，后续可继续补充源码与说明。

## How To Run

1. 使用 Visual Studio 打开对应项目的 `.sln` 文件。
2. 选择 `Debug | x64`（多数项目已使用该配置）。
3. 编译并运行。

如果希望命令行编译（示例）：

```bash
g++ -std=c++17 -O2 -o demo "OS/实验四/Project4/置换算法.cpp"
./demo
```

## Notes

- 仓库中保留了部分编译产物（如 `x64/Debug/*.exe`、`.obj`、`.pdb`、`.tlog`），用于课程阶段留档。
- 若后续需要精简仓库体积，建议补充 `.gitignore` 并清理构建中间文件。

## License

仅用于学习与交流，请勿直接用于课程作业提交。
