#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <random>
#include <chrono>
#include <string>
#include <limits>

struct AlgorithmResult {
    std::string name;
    double value;
    double time;
    std::vector<int> assignment;
    std::vector<double> targetSurvival;
};

struct WeaponTargetValue {
    double value;
    int weapon;
    int target;

    WeaponTargetValue(double v, int w, int t) : value(v), weapon(w), target(t) {}
};

struct TestInstance {
    int dimension;
    std::string name;
};

class WTAProblem {
private:
    std::vector<std::vector<double>> effectiveness;
    std::vector<double> targetValues;
    int weaponCount, targetCount;
    mutable std::mt19937 rng;

    double calculateObjective(const std::vector<int>& assignment) const {
        std::vector<double> survival(targetCount, 1.0);
        for (int i = 0; i < weaponCount; ++i) {
            if (assignment[i] >= 0 && assignment[i] < targetCount) {
                survival[assignment[i]] *= (1.0 - effectiveness[i][assignment[i]]);
            }
        }
        double total = 0.0;
        for (int i = 0; i < targetCount; ++i) {
            total += targetValues[i] * survival[i];
        }
        return total;
    }

    std::vector<double> getSurvivalRates(const std::vector<int>& assignment) const {
        std::vector<double> survival(targetCount, 1.0);
        for (int i = 0; i < weaponCount; ++i) {
            if (assignment[i] >= 0 && assignment[i] < targetCount) {
                survival[assignment[i]] *= (1.0 - effectiveness[i][assignment[i]]);
            }
        }
        return survival;
    }

public:
    WTAProblem(int dimension) : weaponCount(dimension), targetCount(dimension),
        rng(12345) {  // 固定种子确保可重现结果
        generateProblemInstance();
    }

    void generateProblemInstance() {
        effectiveness.resize(weaponCount, std::vector<double>(targetCount));
        targetValues.resize(targetCount);

        // 符合要求：摧毁概率0.60-0.90，目标价值25.0-100.0
        std::uniform_real_distribution<> effDis(0.60, 0.90);
        std::uniform_real_distribution<> valueDis(25.0, 100.0);

        for (int i = 0; i < weaponCount; ++i) {
            for (int j = 0; j < targetCount; ++j) {
                effectiveness[i][j] = effDis(rng);
            }
        }

        for (int i = 0; i < targetCount; ++i) {
            targetValues[i] = valueDis(rng);
        }
    }

    void printProblemInfo() const {
        std::cout << "武器目标分配问题实例 - " << weaponCount << "个武器 × " << targetCount << "个目标 (武器=目标)" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        // 只显示较小维度的详细信息
        if (weaponCount <= 10) {
            std::cout << "\n效能矩阵 (武器摧毁目标的概率):" << std::endl;
            std::cout << std::setw(8) << " ";
            for (int j = 0; j < targetCount; ++j) {
                std::cout << std::setw(10) << ("目标" + std::to_string(j + 1));
            }
            std::cout << std::endl;

            for (int i = 0; i < weaponCount; ++i) {
                std::cout << std::setw(8) << ("武器" + std::to_string(i + 1));
                for (int j = 0; j < targetCount; ++j) {
                    std::cout << std::setw(10) << std::fixed << std::setprecision(3)
                        << effectiveness[i][j];
                }
                std::cout << std::endl;
            }

            std::cout << "\n目标价值: ";
            for (int i = 0; i < targetCount; ++i) {
                std::cout << "目标" << i + 1 << "=" << std::fixed << std::setprecision(1)
                    << targetValues[i] << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "\n维度较大，显示统计信息:" << std::endl;
            double avgEff = 0.0, minEff = 1.0, maxEff = 0.0;
            for (int i = 0; i < weaponCount; ++i) {
                for (int j = 0; j < targetCount; ++j) {
                    avgEff += effectiveness[i][j];
                    minEff = std::min(minEff, effectiveness[i][j]);
                    maxEff = std::max(maxEff, effectiveness[i][j]);
                }
            }
            avgEff /= (weaponCount * targetCount);

            double avgValue = 0.0, minValue = 1000.0, maxValue = 0.0;
            for (int i = 0; i < targetCount; ++i) {
                avgValue += targetValues[i];
                minValue = std::min(minValue, targetValues[i]);
                maxValue = std::max(maxValue, targetValues[i]);
            }
            avgValue /= targetCount;

            std::cout << "效能统计: 平均=" << std::fixed << std::setprecision(3) << avgEff
                << ", 最小=" << minEff << ", 最大=" << maxEff << std::endl;
            std::cout << "价值统计: 平均=" << std::setprecision(1) << avgValue
                << ", 最小=" << minValue << ", 最大=" << maxValue << std::endl;
        }
    }

    AlgorithmResult solveGreedy() {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<int> assignment(weaponCount, -1);
        std::vector<double> survival(targetCount, 1.0);

        for (int i = 0; i < weaponCount; ++i) {
            double bestValue = 0.0;
            int bestTarget = -1;

            for (int j = 0; j < targetCount; ++j) {
                double damage = effectiveness[i][j];
                double value = targetValues[j] * survival[j] * damage;

                if (value > bestValue) {
                    bestValue = value;
                    bestTarget = j;
                }
            }

            if (bestTarget != -1) {
                assignment[i] = bestTarget;
                survival[bestTarget] *= (1.0 - effectiveness[i][bestTarget]);
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(end - start).count();
        double objValue = calculateObjective(assignment);

        AlgorithmResult result;
        result.name = "贪心算法";
        result.value = objValue;
        result.time = time;
        result.assignment = assignment;
        result.targetSurvival = getSurvivalRates(assignment);

        return result;
    }

    AlgorithmResult solveHungarian() {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<int> assignment(weaponCount, -1);
        std::vector<bool> targetUsed(targetCount, false);

        std::vector<WeaponTargetValue> values;
        for (int i = 0; i < weaponCount; ++i) {
            for (int j = 0; j < targetCount; ++j) {
                double value = targetValues[j] * effectiveness[i][j];
                values.push_back(WeaponTargetValue(value, i, j));
            }
        }

        std::sort(values.begin(), values.end(), [](const WeaponTargetValue& a, const WeaponTargetValue& b) {
            return a.value > b.value;
            });

        for (size_t i = 0; i < values.size(); ++i) {
            int weapon = values[i].weapon;
            int target = values[i].target;
            if (assignment[weapon] == -1 && !targetUsed[target]) {
                assignment[weapon] = target;
                targetUsed[target] = true;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(end - start).count();
        double objValue = calculateObjective(assignment);

        AlgorithmResult result;
        result.name = "匈牙利算法";
        result.value = objValue;
        result.time = time;
        result.assignment = assignment;
        result.targetSurvival = getSurvivalRates(assignment);

        return result;
    }

    AlgorithmResult solveGenetic() {
        auto start = std::chrono::high_resolution_clock::now();

        int popSize = std::min(50, std::max(20, weaponCount * 2));
        int maxGen = std::min(200, std::max(50, weaponCount * 5));
        double mutationRate = 0.15;

        std::vector<std::vector<int>> population(popSize, std::vector<int>(weaponCount));
        std::uniform_int_distribution<> targetDis(0, targetCount - 1);

        for (int i = 0; i < popSize; ++i) {
            for (int j = 0; j < weaponCount; ++j) {
                population[i][j] = targetDis(rng);
            }
        }

        std::vector<int> bestSolution;
        double bestFitness = std::numeric_limits<double>::max();

        for (int gen = 0; gen < maxGen; ++gen) {
            std::vector<double> fitness(popSize);

            for (int i = 0; i < popSize; ++i) {
                fitness[i] = calculateObjective(population[i]);
                if (fitness[i] < bestFitness) {
                    bestFitness = fitness[i];
                    bestSolution = population[i];
                }
            }

            std::vector<std::vector<int>> newPop;
            for (int i = 0; i < popSize; ++i) {
                int p1 = std::uniform_int_distribution<>(0, popSize - 1)(rng);
                int p2 = std::uniform_int_distribution<>(0, popSize - 1)(rng);
                int parent = (fitness[p1] < fitness[p2]) ? p1 : p2;

                std::vector<int> offspring = population[parent];

                if (std::uniform_real_distribution<>(0.0, 1.0)(rng) < mutationRate) {
                    int pos = std::uniform_int_distribution<>(0, weaponCount - 1)(rng);
                    offspring[pos] = targetDis(rng);
                }

                newPop.push_back(offspring);
            }
            population = newPop;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(end - start).count();

        AlgorithmResult result;
        result.name = "遗传算法";
        result.value = bestFitness;
        result.time = time;
        result.assignment = bestSolution;
        result.targetSurvival = getSurvivalRates(bestSolution);

        return result;
    }

    AlgorithmResult solveSimulatedAnnealing() {
        auto start = std::chrono::high_resolution_clock::now();

        double initTemp = 100.0;
        double finalTemp = 0.1;
        double coolingRate = 0.98;
        int maxIter = std::min(3000, std::max(1000, weaponCount * 50));

        std::vector<int> current(weaponCount);
        std::uniform_int_distribution<> targetDis(0, targetCount - 1);

        for (int i = 0; i < weaponCount; ++i) {
            current[i] = targetDis(rng);
        }

        double currentValue = calculateObjective(current);
        std::vector<int> best = current;
        double bestValue = currentValue;

        double temp = initTemp;

        for (int iter = 0; iter < maxIter; ++iter) {
            std::vector<int> neighbor = current;
            int changeCount = std::uniform_int_distribution<>(1, 2)(rng);

            for (int i = 0; i < changeCount; ++i) {
                int pos = std::uniform_int_distribution<>(0, weaponCount - 1)(rng);
                neighbor[pos] = targetDis(rng);
            }

            double neighborValue = calculateObjective(neighbor);
            double delta = neighborValue - currentValue;

            bool accept = false;
            if (delta < 0) {
                accept = true;
            }
            else if (temp > finalTemp) {
                double prob = exp(-delta / temp);
                if (std::uniform_real_distribution<>(0.0, 1.0)(rng) < prob) {
                    accept = true;
                }
            }

            if (accept) {
                current = neighbor;
                currentValue = neighborValue;

                if (neighborValue < bestValue) {
                    best = neighbor;
                    bestValue = neighborValue;
                }
            }

            temp *= coolingRate;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(end - start).count();

        AlgorithmResult result;
        result.name = "模拟退火";
        result.value = bestValue;
        result.time = time;
        result.assignment = best;
        result.targetSurvival = getSurvivalRates(best);

        return result;
    }

    void compareResults(const std::vector<AlgorithmResult>& results, int dimension) {
        std::cout << "\n算法性能比较 (维度: " << dimension << ")" << std::endl;
        std::cout << std::string(70, '-') << std::endl;

        auto bestResult = std::min_element(results.begin(), results.end(),
            [](const AlgorithmResult& a, const AlgorithmResult& b) {
                return a.value < b.value;
            });

        std::cout << std::left << std::setw(12) << "算法"
            << std::setw(15) << "剩余价值"
            << std::setw(12) << "时间(ms)"
            << std::setw(12) << "相对差异" << std::endl;
        std::cout << std::string(70, '-') << std::endl;

        for (const auto& result : results) {
            double relativePerf = (result.value / bestResult->value - 1.0) * 100.0;
            std::cout << std::left << std::setw(12) << result.name
                << std::setw(15) << std::fixed << std::setprecision(2) << result.value
                << std::setw(12) << std::setprecision(1) << result.time
                << std::setw(12) << std::setprecision(1) << relativePerf << "%" << std::endl;
        }
    }
};

int main() {
    std::cout << "武器目标分配问题 - 12个测试实例性能评估" << std::endl;
    std::cout << "每个实例的武器数量等于目标数量，符合标准测试要求" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    // 定义12个测试实例的维度 (符合5-200范围要求)
    std::vector<TestInstance> testInstances = {
        {5, "WTA1"},   {8, "WTA2"},   {10, "WTA3"},  {15, "WTA4"},
        {20, "WTA5"},  {25, "WTA6"},  {30, "WTA7"},  {40, "WTA8"},
        {50, "WTA9"},  {75, "WTA10"}, {100, "WTA11"}, {150, "WTA12"}
    };

    std::vector<std::vector<AlgorithmResult>> allResults;

    for (size_t i = 0; i < testInstances.size(); ++i) {
        const auto& instance = testInstances[i];
        std::cout << "\n测试实例 " << instance.name << " (维度: " << instance.dimension << ")" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        WTAProblem wta(instance.dimension);

        // 只对小维度显示详细信息
        if (instance.dimension <= 10) {
            wta.printProblemInfo();
        }
        else {
            wta.printProblemInfo();
        }

        std::vector<AlgorithmResult> results;

        std::cout << "\n执行算法..." << std::endl;
        results.push_back(wta.solveGreedy());
        results.push_back(wta.solveHungarian());
        results.push_back(wta.solveGenetic());
        results.push_back(wta.solveSimulatedAnnealing());

        wta.compareResults(results, instance.dimension);
        allResults.push_back(results);
    }

    // 总体性能分析
    std::cout << "\n\n" << std::string(80, '=') << std::endl;
    std::cout << "12个测试实例总体性能分析" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    std::vector<std::string> algorithmNames = { "贪心算法", "匈牙利算法", "遗传算法", "模拟退火" };
    std::vector<std::vector<double>> algorithmTimes(4);
    std::vector<std::vector<double>> algorithmValues(4);

    for (size_t i = 0; i < testInstances.size(); ++i) {
        for (size_t j = 0; j < 4; ++j) {
            algorithmTimes[j].push_back(allResults[i][j].time);
            algorithmValues[j].push_back(allResults[i][j].value);
        }
    }

    std::cout << "\n平均执行时间分析:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    for (size_t i = 0; i < 4; ++i) {
        double avgTime = 0.0;
        for (double time : algorithmTimes[i]) {
            avgTime += time;
        }
        avgTime /= algorithmTimes[i].size();
        std::cout << std::setw(12) << algorithmNames[i] << ": "
            << std::fixed << std::setprecision(2) << avgTime << " ms" << std::endl;
    }

    std::cout << "\n各维度最优算法统计:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::vector<int> bestCount(4, 0);
    for (size_t i = 0; i < testInstances.size(); ++i) {
        auto& results = allResults[i];
        auto bestResult = std::min_element(results.begin(), results.end(),
            [](const AlgorithmResult& a, const AlgorithmResult& b) {
                return a.value < b.value;
            });

        int bestIdx = std::distance(results.begin(), bestResult);
        bestCount[bestIdx]++;

        std::cout << testInstances[i].name << " (维度" << std::setw(3) << testInstances[i].dimension
            << "): " << bestResult->name << std::endl;
    }

    std::cout << "\n算法获胜次数统计:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    for (size_t i = 0; i < 4; ++i) {
        std::cout << std::setw(12) << algorithmNames[i] << ": " << bestCount[i] << " 次" << std::endl;
    }

    return 0;
}