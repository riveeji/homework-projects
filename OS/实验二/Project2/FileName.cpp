#include <iostream>
#include <vector>
using namespace std;

const int n = 5; // 进程数
const int m = 3; // 资源种类

int Available[m] = { 2, 3, 3 }; // 系统可用资源数

// Max[i][j]：进程i对资源j的最大需求
int Max[n][m] = {
    {3, 4, 7},  // P1
    {1, 3, 4},  // P2
    {0, 0, 3},  // P3
    {2, 2, 1},  // P4
    {1, 1, 0}   // P5
};

// Allocation[i][j]：当前已分配给进程i的资源数
int Allocation[n][m] = {
    {2, 1, 2},  // P1
    {1, 0, 2},  // P2
    {0, 0, 1},  // P3
    {1, 0, 0},  // P4
    {1, 1, 0}   // P5
};

int Need[n][m]; // Need[i][j] = Max[i][j] - Allocation[i][j]

void calculateNeed() {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            Need[i][j] = Max[i][j] - Allocation[i][j];
}

bool isSafe() {
    int Work[m];
    bool Finish[n] = { false };
    for (int i = 0; i < m; ++i) Work[i] = Available[i];

    for (int count = 0; count < n; ++count) {
        bool found = false;
        for (int i = 0; i < n; ++i) {
            if (!Finish[i]) {
                bool ok = true;
                for (int j = 0; j < m; ++j)
                    if (Need[i][j] > Work[j]) {
                        ok = false;
                        break;
                    }
                if (ok) {
                    for (int j = 0; j < m; ++j)
                        Work[j] += Allocation[i][j];
                    Finish[i] = true;
                    found = true;
                }
            }
        }
        if (!found) return false;
    }
    return true;
}

bool requestResource(int p, vector<int> req) {
    cout << "进程 P" << (p + 1) << " 请求资源: ";
    for (int r : req) cout << r << " ";
    cout << endl;

    for (int i = 0; i < m; ++i) {
        if (req[i] > Need[p][i]) {
            cout << "非法请求：请求超过最大需求\n\n";
            return false;
        }
        if (req[i] > Available[i]) {
            cout << "资源不足：Pi 阻塞\n\n";
            return false;
        }
    }

    for (int i = 0; i < m; ++i) {
        Available[i] -= req[i];
        Allocation[p][i] += req[i];
        Need[p][i] -= req[i];
    }

    if (isSafe()) {
        cout << "请求成功，系统安全\n\n";
        return true;
    }
    else {
        for (int i = 0; i < m; ++i) {
            Available[i] += req[i];
            Allocation[p][i] -= req[i];
            Need[p][i] += req[i];
        }
        cout << "请求被拒绝：系统不安全\n\n";
        return false;
    }
}

int main() {
    calculateNeed();

    // 测试请求序列（实验要求 a~d）
    requestResource(1, { 0, 3, 4 }); // P2
    requestResource(3, { 1, 0, 1 }); // P4
    requestResource(0, { 2, 0, 1 }); // P1
    requestResource(2, { 0, 0, 2 }); // P3

    return 0;
}
