#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

struct Job {
    int profit;
    int deadline;
};

bool compareProfit(const Job& a, const Job& b) {
    return a.profit > b.profit; // 按利润降序排序
}

int main() {
    int n;
    cin >> n;
    vector<Job> jobs(n);

    for (int i = 0; i < n; ++i) {
        cin >> jobs[i].profit >> jobs[i].deadline;
    }

    // 按利润降序排序
    sort(jobs.begin(), jobs.end(), compareProfit);

    // 找出最大的deadline
    int max_deadline = 0;
    for (const auto& job : jobs) {
        if (job.deadline > max_deadline) {
            max_deadline = job.deadline;
        }
    }

    // 使用数组标记时间槽是否被占用
    vector<bool> slots(max_deadline + 1, false);
    int total_profit = 0;

    for (const auto& job : jobs) {
        // 从deadline往前找第一个可用的时间槽
        for (int j = min(job.deadline, max_deadline); j >= 1; --j) {
            if (!slots[j]) {
                slots[j] = true;
                total_profit += job.profit;
                break;
            }
        }
    }

    cout << total_profit << endl;
    return 0;
}