#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <random>
#include <chrono>
#include <string> // 添加此头文件以支持 std::to_string
#include <sstream> // 添加此头文件用于字符串转换的替代方案

// 武器类
class Weapon {
private:
    int id;
    int platformId;
    double damage;
    bool assigned;

public:
    Weapon(int id, int platformId, double damage) :
        id(id), platformId(platformId), damage(damage), assigned(false) {}

    int getId() const { return id; }
    int getPlatformId() const { return platformId; }
    double getDamage() const { return damage; }
    bool isAssigned() const { return assigned; }
    void setAssigned(bool status) { assigned = status; }
};

// 目标类
class Target {
private:
    int id;
    double value;        // 目标价值
    double survivability; // 当前存活概率

public:
    Target(int id, double value) :
        id(id), value(value), survivability(1.0) {}

    int getId() const { return id; }
    double getValue() const { return value; }
    double getSurvivability() const { return survivability; }

    // 更新目标存活概率
    void updateSurvivability(double killProbability) {
        survivability *= (1.0 - killProbability);
    }

    // 重置目标状态
    void reset() {
        survivability = 1.0;
    }
};

// 武器平台类
class WeaponPlatform {
private:
    int id;
    std::vector<Weapon> weapons;

public:
    WeaponPlatform(int id) : id(id) {}

    int getId() const { return id; }

    void addWeapon(const Weapon& weapon) {
        weapons.push_back(weapon);
    }

    const std::vector<Weapon>& getWeapons() const {
        return weapons;
    }

    std::vector<Weapon>& getWeapons() {
        return weapons;
    }
};

// WTA问题类
class WTAProblem {
private:
    std::vector<WeaponPlatform> platforms;
    std::vector<Target> targets;
    std::vector<std::vector<double>> effectivenessMatrix; // 武器对目标的效能矩阵

    // 计算总目标期望值
    double calculateExpectedTargetValue() const {
        double totalValue = 0.0;
        for (const auto& target : targets) {
            totalValue += target.getValue() * target.getSurvivability();
        }
        return totalValue;
    }

    // 匈牙利算法辅助函数
    bool hungarianDFS(int u, std::vector<bool>& visited, std::vector<int>& matching,
        const std::vector<std::vector<double>>& costMatrix) {
        for (size_t v = 0; v < targets.size(); ++v) {
            if (!visited[v]) {
                double cost = costMatrix[u][v];
                visited[v] = true;

                if (matching[v] == -1 || hungarianDFS(matching[v], visited, matching, costMatrix)) {
                    matching[v] = u;
                    return true;
                }
            }
        }
        return false;
    }

public:
    // 添加平台
    void addPlatform(const WeaponPlatform& platform) {
        platforms.push_back(platform);
    }

    // 添加目标
    void addTarget(const Target& target) {
        targets.push_back(target);
    }

    // 初始化效能矩阵
    void initializeEffectivenessMatrix() {
        int weaponCount = 0;
        for (const auto& platform : platforms) {
            weaponCount += platform.getWeapons().size();
        }

        effectivenessMatrix.resize(weaponCount, std::vector<double>(targets.size(), 0.0));

        // 使用随机数生成效能值，也可以根据实际情况设定
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.1, 0.9);

        for (size_t i = 0; i < weaponCount; ++i) {
            for (size_t j = 0; j < targets.size(); ++j) {
                effectivenessMatrix[i][j] = dis(gen);
            }
        }
    }

    // 手动设置效能矩阵
    void setEffectivenessMatrix(const std::vector<std::vector<double>>& matrix) {
        effectivenessMatrix = matrix;
    }

    // 获取效能矩阵
    const std::vector<std::vector<double>>& getEffectivenessMatrix() const {
        return effectivenessMatrix;
    }

    // 贪心算法求解
    void solveGreedy() {
        // 重置目标状态
        for (auto& target : targets) {
            target.reset();
        }

        // 重置武器分配状态
        for (auto& platform : platforms) {
            for (auto& weapon : platform.getWeapons()) {
                weapon.setAssigned(false);
            }
        }

        int weaponIndex = 0;
        std::vector<Weapon*> allWeapons;

        // 收集所有武器
        for (auto& platform : platforms) {
            for (auto& weapon : platform.getWeapons()) {
                allWeapons.push_back(&weapon);
                weaponIndex++;
            }
        }

        // 贪心分配
        for (auto* weapon : allWeapons) {
            if (weapon->isAssigned()) continue;

            int weaponId = weapon->getId();
            int platformId = weapon->getPlatformId();
            int globalWeaponIndex = 0;

            // 计算武器的全局索引
            for (const auto& platform : platforms) {
                if (platform.getId() == platformId) {
                    for (const auto& w : platform.getWeapons()) {
                        if (w.getId() == weaponId) break;
                        globalWeaponIndex++;
                    }
                    break;
                }
                else {
                    globalWeaponIndex += platform.getWeapons().size();
                }
            }

            // 寻找最佳目标
            double bestValue = 0.0;
            int bestTargetIndex = -1;

            for (size_t j = 0; j < targets.size(); ++j) {
                double currentDamage = effectivenessMatrix[globalWeaponIndex][j];
                double targetValue = targets[j].getValue();
                double currentSurvivability = targets[j].getSurvivability();
                double damageValue = targetValue * currentSurvivability * currentDamage;

                if (damageValue > bestValue) {
                    bestValue = damageValue;
                    bestTargetIndex = j;
                }
            }

            // 分配武器到最佳目标
            if (bestTargetIndex != -1) {
                weapon->setAssigned(true);
                targets[bestTargetIndex].updateSurvivability(
                    effectivenessMatrix[globalWeaponIndex][bestTargetIndex]);

                std::cout << "武器 " << weaponId << " (平台 " << platformId
                    << ") 分配给目标 " << targets[bestTargetIndex].getId()
                    << " 效能值: " << effectivenessMatrix[globalWeaponIndex][bestTargetIndex]
                    << std::endl;
            }
        }

        // 输出结果
        std::cout << "贪心算法结果:" << std::endl;
        std::cout << "总目标期望价值: " << calculateExpectedTargetValue() << std::endl;
        std::cout << "各目标存活概率: ";
        for (const auto& target : targets) {
            std::cout << "目标 " << target.getId() << ": "
                << target.getSurvivability() * 100 << "% ";
        }
        std::cout << std::endl;
    }

    // 匈牙利算法求解
    void solveHungarian() {
        // 重置目标状态
        for (auto& target : targets) {
            target.reset();
        }

        // 重置武器分配状态
        for (auto& platform : platforms) {
            for (auto& weapon : platform.getWeapons()) {
                weapon.setAssigned(false);
            }
        }

        // 构建成本矩阵（我们要最小化目标存活价值，所以取负值）
        int weaponCount = 0;
        for (const auto& platform : platforms) {
            weaponCount += platform.getWeapons().size();
        }

        std::vector<std::vector<double>> costMatrix(
            weaponCount, std::vector<double>(targets.size(), 0.0));

        int weaponIndex = 0;
        for (const auto& platform : platforms) {
            for (const auto& weapon : platform.getWeapons()) {
                for (size_t j = 0; j < targets.size(); ++j) {
                    // 成本 = -目标价值 * 武器效能（我们要最小化，所以取负值）
                    costMatrix[weaponIndex][j] = -targets[j].getValue() *
                        effectivenessMatrix[weaponIndex][j];
                }
                weaponIndex++;
            }
        }

        // 匈牙利算法
        std::vector<int> matching(targets.size(), -1);

        for (int i = 0; i < weaponCount; ++i) {
            std::vector<bool> visited(targets.size(), false);
            hungarianDFS(i, visited, matching, costMatrix);
        }

        // 应用分配结果
        for (size_t j = 0; j < targets.size(); ++j) {
            if (matching[j] != -1) {
                int globalWeaponIndex = matching[j];

                // 找到对应的武器
                int currentWeapon = 0;
                Weapon* assignedWeapon = nullptr;

                for (auto& platform : platforms) {
                    for (auto& weapon : platform.getWeapons()) {
                        if (currentWeapon == globalWeaponIndex) {
                            assignedWeapon = &weapon;
                            break;
                        }
                        currentWeapon++;
                    }
                    if (assignedWeapon) break;
                }

                if (assignedWeapon) {
                    assignedWeapon->setAssigned(true);
                    targets[j].updateSurvivability(
                        effectivenessMatrix[globalWeaponIndex][j]);

                    std::cout << "武器 " << assignedWeapon->getId()
                        << " (平台 " << assignedWeapon->getPlatformId()
                        << ") 分配给目标 " << targets[j].getId()
                        << " 效能值: " << effectivenessMatrix[globalWeaponIndex][j]
                        << std::endl;
                }
            }
        }

        // 输出结果
        std::cout << "匈牙利算法结果:" << std::endl;
        std::cout << "总目标期望价值: " << calculateExpectedTargetValue() << std::endl;
        std::cout << "各目标存活概率: ";
        for (const auto& target : targets) {
            std::cout << "目标 " << target.getId() << ": "
                << target.getSurvivability() * 100 << "% ";
        }
        std::cout << std::endl;
    }

    // 打印效能矩阵 - 修改此函数以避免使用std::to_string
    void printEffectivenessMatrix() const {
        std::cout << "效能矩阵:" << std::endl;
        std::cout << std::setw(10) << " ";

        for (size_t j = 0; j < targets.size(); ++j) {
            // 使用直接输出而不是std::to_string
            std::cout << std::setw(10) << "目标 " << targets[j].getId();
        }
        std::cout << std::endl;

        int weaponIndex = 0;
        for (const auto& platform : platforms) {
            for (const auto& weapon : platform.getWeapons()) {
                // 使用直接输出而不是std::to_string
                std::cout << std::setw(10) << "武器 " << weapon.getId()
                    << "(" << platform.getId() << ")";

                for (size_t j = 0; j < targets.size(); ++j) {
                    std::cout << std::setw(10) << std::fixed << std::setprecision(2)
                        << effectivenessMatrix[weaponIndex][j];
                }
                std::cout << std::endl;
                weaponIndex++;
            }
        }
    }
};

int main() {
    // 创建WTA问题实例
    WTAProblem wta;

    // 创建3个武器平台
    for (int i = 1; i <= 3; ++i) {
        WeaponPlatform platform(i);

        // 每个平台3个武器
        for (int j = 1; j <= 3; ++j) {
            platform.addWeapon(Weapon(j, i, 0.8)); // 假设基础伤害为0.8
        }

        wta.addPlatform(platform);
    }

    // 创建4个目标
    for (int i = 1; i <= 4; ++i) {
        wta.addTarget(Target(i, 100.0 * i)); // 目标价值随ID增加
    }

    // 初始化效能矩阵
    wta.initializeEffectivenessMatrix();

    // 打印效能矩阵
    wta.printEffectivenessMatrix();

    std::cout << "\n=== 贪心算法 ===" << std::endl;
    wta.solveGreedy();

    std::cout << "\n=== 匈牙利算法 ===" << std::endl;
    wta.solveHungarian();

    return 0;
}