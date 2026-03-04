#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>

using namespace std;

const int PAGE_SIZE = 1024;               // 每页大小 1K
const int VIRTUAL_MEMORY_SIZE = 32 * 1024; // 虚拟内存大小 32K
const int PAGE_COUNT = 320;               // 页地址流长度

// 生成页地址流（长度 PAGE_COUNT），确保不会出现 rand()%0
vector<int> generatePageStream() {
    constexpr int numPages = VIRTUAL_MEMORY_SIZE / PAGE_SIZE; // =32
    vector<int> pages;
    srand(static_cast<unsigned>(time(nullptr)));

    while (pages.size() < PAGE_COUNT) {
        // 1) M ∈ [1, numPages-2]
        int M = rand() % (numPages - 2) + 1;
        pages.push_back(M);
        pages.push_back(M + 1);

        // 2) M1 ∈ [0, M-1]
        int M1 = rand() % M;
        pages.push_back(M1);
        pages.push_back(M1 + 1);

        // 3) M2 ∈ [M1+2, numPages-2]
        int lo = M1 + 2;
        int hi = numPages - 2;
        int range = hi - lo + 1;
        if (range <= 0) {
            lo = hi;
            range = 1;
        }
        int M2 = lo + rand() % range;
        pages.push_back(M2);
        pages.push_back(M2 + 1);
    }

    pages.resize(PAGE_COUNT);
    return pages;
}

// FIFO 页面置换模拟
int simulateFIFO(const vector<int>& pages, int frameCount) {
    assert(frameCount > 0);
    deque<int> memory;
    int miss = 0;

    for (int page : pages) {
        if (find(memory.begin(), memory.end(), page) == memory.end()) {
            ++miss;
            if (memory.size() == frameCount) {
                memory.pop_front();
            }
            memory.push_back(page);
        }
    }
    return miss;
}

// LRU 页面置换模拟
int simulateLRU(const vector<int>& pages, int frameCount) {
    assert(frameCount > 0);
    list<int> memory;
    int miss = 0;

    for (int page : pages) {
        auto it = find(memory.begin(), memory.end(), page);
        if (it == memory.end()) {
            ++miss;
            if (memory.size() == frameCount) {
                memory.pop_back();
            }
        }
        else {
            memory.erase(it);
        }
        memory.push_front(page);
    }
    return miss;
}

// Optimal 页面置换模拟
int simulateOptimal(const vector<int>& pages, int frameCount) {
    assert(frameCount > 0);
    vector<int> memory;
    int miss = 0;

    for (size_t i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        if (find(memory.begin(), memory.end(), page) == memory.end()) {
            ++miss;
            if (memory.size() < frameCount) {
                memory.push_back(page);
            }
            else {
                // 找出未来使用最远或不再使用的页面
                int farthest = -1, replaceIdx = 0;
                for (int j = 0; j < memory.size(); ++j) {
                    auto it = find(pages.begin() + i + 1, pages.end(), memory[j]);
                    int distance = (it == pages.end())
                        ? pages.size() + 1
                        : static_cast<int>(it - pages.begin());
                    if (distance > farthest) {
                        farthest = distance;
                        replaceIdx = j;
                    }
                }
                memory[replaceIdx] = page;
            }
        }
    }
    return miss;
}

int main() {
    vector<int> pageStream = generatePageStream();

    for (int frames = 8; frames <= 32; frames += 8) {
        // frameCount 保证大于 0
        int fifoMiss = simulateFIFO(pageStream, frames);
        int lruMiss = simulateLRU(pageStream, frames);
        int optMiss = simulateOptimal(pageStream, frames);

        cout << "页框数: " << frames << endl;
        cout << "FIFO 命中率: "
            << 1.0 - fifoMiss / static_cast<double>(PAGE_COUNT) << endl;
        cout << "LRU  命中率: "
            << 1.0 - lruMiss / static_cast<double>(PAGE_COUNT) << endl;
        cout << "OPT  命中率: "
            << 1.0 - optMiss / static_cast<double>(PAGE_COUNT) << endl;
        cout << "--------------------------------\n";
    }

    return 0;
}
