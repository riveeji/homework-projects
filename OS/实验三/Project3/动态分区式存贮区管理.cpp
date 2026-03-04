#include <iostream>   // 用于输入输出流
#include <memory>     // 用于智能指针
#include <vector>     // 用于动态数组
#include <algorithm>  // 用于查找等算法
#include <iomanip>    // 用于格式化输出

// 定义内存管理命名空间
namespace MemoryManager {
    constexpr int TOTAL_MEM = 512;  // 总内存大小

    // 内存分配策略枚举
    enum class AllocationPolicy {
        FIRST_AVAILABLE,  // 首次适应
        MOST_FITTING,     // 最佳适应
        LEAST_FITTING     // 最坏适应
    };

    // 定义内存块结构体
    struct MemoryChunk {
        int start;        // 起始地址
        int length;       // 长度
        bool is_free;     // 是否空闲
        std::string owner;// 所属进程名
        MemoryChunk* next_free; // 指向下一个空闲块（用于空闲链表）

        MemoryChunk(int s, int l, bool free, std::string o = "")
            : start(s), length(l), is_free(free), owner(o), next_free(nullptr) {}

        bool can_hold(int size) const {
            return is_free && length >= size;
        }

        int end() const {
            return start + length - 1;
        }
    };

    // 分区管理类
    class PartitionManager {
    private:
        std::vector<std::unique_ptr<MemoryChunk>> memory_map; // 内存块集合
        MemoryChunk* free_chain;       // 空闲块链表头指针
        AllocationPolicy current_policy; // 当前分配策略

        // 更新空闲链表
        void link_free_blocks() {
            free_chain = nullptr;
            MemoryChunk* last_free = nullptr;

            for (auto& chunk : memory_map) {
                if (chunk->is_free) {
                    if (!free_chain) {
                        free_chain = chunk.get();
                    }
                    if (last_free) {
                        last_free->next_free = chunk.get();
                    }
                    last_free = chunk.get();
                }
            }
            if (last_free) {
                last_free->next_free = nullptr;
            }
        }

        // 合并相邻空闲块
        void merge_adjacent() {
            bool merged;
            do {
                merged = false;
                for (size_t i = 0; i < memory_map.size() - 1; ++i) {
                    auto& current = memory_map[i];
                    auto& next = memory_map[i + 1];

                    if (current->is_free && next->is_free &&
                        current->end() + 1 == next->start) {
                        current->length += next->length;
                        memory_map.erase(memory_map.begin() + i + 1);
                        merged = true;
                        break;
                    }
                }
            } while (merged);
            link_free_blocks(); // 重新建立空闲链表
        }

    public:
        PartitionManager() : free_chain(nullptr), current_policy(AllocationPolicy::FIRST_AVAILABLE) {
            // 初始只有一个整个内存块是空闲的
            memory_map.emplace_back(std::make_unique<MemoryChunk>(0, TOTAL_MEM, true));
            link_free_blocks();
        }

        // 设置分配策略
        void set_policy(AllocationPolicy policy) {
            current_policy = policy;
        }

        // 分配内存，返回分配后的起始地址（不含描述符）
        int allocate(const std::string& process, int size) {
            if (size <= 0) return -1;

            const int required = size + 1; // 加上描述符空间
            MemoryChunk* target = nullptr;

            // 根据策略查找合适的空闲块
            switch (current_policy) {
            case AllocationPolicy::FIRST_AVAILABLE:
                for (auto it = free_chain; it; it = it->next_free) {
                    if (it->can_hold(required)) {
                        target = it;
                        break;
                    }
                }
                break;

            case AllocationPolicy::MOST_FITTING: {
                MemoryChunk* best = nullptr;
                int min_diff = TOTAL_MEM + 1;
                for (auto it = free_chain; it; it = it->next_free) {
                    if (it->can_hold(required)) {
                        int diff = it->length - required;
                        if (diff < min_diff) {
                            min_diff = diff;
                            best = it;
                        }
                    }
                }
                target = best;
                break;
            }

            case AllocationPolicy::LEAST_FITTING: {
                MemoryChunk* worst = nullptr;
                int max_diff = -1;
                for (auto it = free_chain; it; it = it->next_free) {
                    if (it->can_hold(required)) {
                        int diff = it->length - required;
                        if (diff > max_diff) {
                            max_diff = diff;
                            worst = it;
                        }
                    }
                }
                target = worst;
                break;
            }
            }

            if (!target) {
                std::cerr << "为进程 " << process << " 分配内存失败！\n";
                return -1;
            }

            // 分割空闲块
            const int remaining = target->length - required;
            const int alloc_start = target->start;

            target->is_free = false;
            target->owner = process;
            target->length = required;

            if (remaining > 0) {
                auto new_chunk = std::make_unique<MemoryChunk>(
                    alloc_start + required, remaining, true);
                memory_map.insert(
                    std::find_if(memory_map.begin(), memory_map.end(),
                        [target](const auto& ptr) { return ptr.get() == target; }) + 1,
                    std::move(new_chunk));
            }

            merge_adjacent(); // 合并空闲块
            return alloc_start + 1; // 返回描述符后的地址
        }

        // 释放指定进程的内存
        bool release(const std::string& process) {
            bool found = false;
            for (auto& chunk : memory_map) {
                if (!chunk->is_free && chunk->owner == process) {
                    chunk->is_free = true;
                    chunk->owner.clear();
                    found = true;
                }
            }
            if (found) {
                merge_adjacent();
                return true;
            }
            std::cerr << "未找到进程 " << process << "，释放失败！\n";
            return false;
        }

        // 显示内存状态
        void display() const {
            std::cout << "内存分配情况如下：\n";
            std::cout << std::setw(8) << "起始地址"
                << std::setw(8) << "结束地址"
                << std::setw(12) << "状态"
                << std::setw(15) << "所属进程"
                << std::setw(10) << "大小\n";

            for (const auto& chunk : memory_map) {
                std::cout << std::setw(8) << chunk->start
                    << std::setw(8) << chunk->end()
                    << std::setw(12) << (chunk->is_free ? "空闲" : "已分配")
                    << std::setw(15) << (chunk->owner.empty() ? "-" : chunk->owner)
                    << std::setw(10) << chunk->length << "\n";
            }
            std::cout << "\n";
        }
    };
}

// 主函数用于测试
int main() {
    using namespace MemoryManager;

    PartitionManager manager;
    manager.set_policy(AllocationPolicy::FIRST_AVAILABLE); // 设置为首次适应策略

    manager.display();

    manager.allocate("J1", 162);
    manager.allocate("J2", 64);
    manager.allocate("J3", 120);
    manager.allocate("J4", 86);
    manager.display();

    manager.release("J1");
    manager.release("J3");
    manager.display();

    manager.allocate("J5", 72);
    manager.allocate("J6", 100);
    manager.display();

    manager.release("J2");
    manager.allocate("J7", 36);
    manager.allocate("J8", 60);
    manager.display();

    manager.release("J4");
    manager.allocate("J9", 110);
    manager.allocate("J10", 42);
    manager.display();

    return 0;
}
