#include <iostream>
#include <queue>
#include <list>
#include <iomanip>
#include <string>
#include <fstream> // 添加文件读写
#include <sstream> // 添加字符串解析
using namespace std;

struct PCB {
    string name;
    int need;
    int wait;
    int turn;
    PCB(string n, int nd, int wt) : name(n), need(nd), wait(wt), turn(0) {}
};

// 用于RQ2(短进程优先)
struct SJFCompare {
    bool operator()(const PCB& a, const PCB& b) {
        return a.need > b.need; // need小的优先
    }
};

void runRoundRobin(queue<PCB>& rq1, list<PCB>& finish, int& clock, int timeSlice) {
    while (!rq1.empty()) {
        PCB p = rq1.front();
        rq1.pop();
        int t = min(p.need, timeSlice);
        clock += t;
        p.need -= t;

        if (p.need == 0) {
            p.turn = clock - p.wait;
            finish.push_back(p);
        }
        else {
            rq1.push(p); // 重新入队
        }
    }
}

void runSJF(priority_queue<PCB, vector<PCB>, SJFCompare>& rq2, list<PCB>& finish, int& clock) {
    while (!rq2.empty()) {
        PCB p = rq2.top();
        rq2.pop();
        clock += p.need;
        p.turn = clock - p.wait;
        finish.push_back(p);
    }
}

void printResults(const list<PCB>& finish) {
    int total = 0;
    cout << fixed << setprecision(2);
    cout << "进程周转时间:" << endl;
    for (const auto& p : finish) {
        cout << "进程 " << p.name << " 周转时间: " << p.turn << endl;
        total += p.turn;
    }
    cout << "平均周转时间: " << (float)total / finish.size() << endl;
}
void readFromFile(const string& filename, queue<PCB>& rq1, priority_queue<PCB, vector<PCB>, SJFCompare>& rq2) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "无法打开文件: " << filename << endl;
        exit(1);
    }

    string line, section;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        if (line == "RQ1" || line == "RQ2") {
            section = line;
            continue;
        }

        istringstream iss(line);
        string name;
        int need, wait;
        if (iss >> name >> need >> wait) {
            PCB p(name, need, wait);
            if (section == "RQ1") {
                rq1.push(p);
            }
            else if (section == "RQ2") {
                rq2.push(p);
            }
        }
    }
}

int main() {
    queue<PCB> rq1;
    priority_queue<PCB, vector<PCB>, SJFCompare> rq2;
    list<PCB> finish;
    int clock = 0;

    readFromFile("process_input.txt", rq1, rq2); // 从文件读取数据

    runRoundRobin(rq1, finish, clock, 7);  // 时间片为 7
    runSJF(rq2, finish, clock);

    printResults(finish);
    return 0;
}
