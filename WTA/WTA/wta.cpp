#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <map>
#include <set>

using namespace std;
using namespace std::chrono;

// WTA问题结构定义
struct WTAProblem {
    int numWeapons;
    int numTargets;
    vector<double> targetValues;     // 目标价值
    vector<double> threatValues;     // 威胁值
    vector<vector<double>> killProb; // 武器对目标的摧毁概率

    WTAProblem(int w, int t) : numWeapons(w), numTargets(t) {
        targetValues.resize(t);
        threatValues.resize(t);
        killProb.resize(w, vector<double>(t));
    }
};

// 解结构定义
struct WTASolution {
    vector<int> assignment;  // assignment[i] 表示武器i分配给的目标
    double fitness;
    double totalValue;
    double totalThreatReduction;

    WTASolution(int size) : assignment(size, -1), fitness(0), totalValue(0), totalThreatReduction(0) {}
};

// WTA问题生成器
class WTAGenerator {
private:
    random_device rd;
    mt19937 gen;

public:
    WTAGenerator() : gen(rd()) {}

    WTAProblem generateProblem(int weapons, int targets) {
        WTAProblem problem(weapons, targets);

        uniform_real_distribution<double> valueDist(25.0, 100.0);
        uniform_real_distribution<double> probDist(0.60, 0.90);

        // 生成目标价值和威胁值
        for (int i = 0; i < targets; i++) {
            problem.targetValues[i] = valueDist(gen);
            problem.threatValues[i] = valueDist(gen);
        }

        // 生成武器对目标的摧毁概率
        for (int i = 0; i < weapons; i++) {
            for (int j = 0; j < targets; j++) {
                problem.killProb[i][j] = probDist(gen);
            }
        }

        return problem;
    }
};

// 适应度评估器
class FitnessEvaluator {
public:
    static double evaluate(const WTASolution& solution, const WTAProblem& problem) {
        vector<double> targetSurvivalProb(problem.numTargets, 1.0);

        // 计算每个目标的存活概率
        for (int w = 0; w < problem.numWeapons; w++) {
            if (solution.assignment[w] != -1) {
                int target = solution.assignment[w];
                targetSurvivalProb[target] *= (1.0 - problem.killProb[w][target]);
            }
        }

        double totalValue = 0.0;
        double totalThreatReduction = 0.0;

        for (int t = 0; t < problem.numTargets; t++) {
            double destructionProb = 1.0 - targetSurvivalProb[t];
            totalValue += destructionProb * problem.targetValues[t];
            totalThreatReduction += destructionProb * problem.threatValues[t];
        }

        return totalValue + totalThreatReduction; // 综合适应度
    }

    static void updateSolutionMetrics(WTASolution& solution, const WTAProblem& problem) {
        vector<double> targetSurvivalProb(problem.numTargets, 1.0);

        for (int w = 0; w < problem.numWeapons; w++) {
            if (solution.assignment[w] != -1) {
                int target = solution.assignment[w];
                targetSurvivalProb[target] *= (1.0 - problem.killProb[w][target]);
            }
        }

        solution.totalValue = 0.0;
        solution.totalThreatReduction = 0.0;

        for (int t = 0; t < problem.numTargets; t++) {
            double destructionProb = 1.0 - targetSurvivalProb[t];
            solution.totalValue += destructionProb * problem.targetValues[t];
            solution.totalThreatReduction += destructionProb * problem.threatValues[t];
        }

        solution.fitness = solution.totalValue + solution.totalThreatReduction;
    }
};

// 遗传算法求解器
class GeneticAlgorithm {
private:
    int populationSize;
    int maxGenerations;
    double mutationRate;
    double crossoverRate;
    random_device rd;
    mt19937 gen;

public:
    GeneticAlgorithm(int popSize = 100, int maxGen = 1000, double mutRate = 0.1, double crossRate = 0.8)
        : populationSize(popSize), maxGenerations(maxGen), mutationRate(mutRate), crossoverRate(crossRate), gen(rd()) {}

    WTASolution solve(const WTAProblem& problem) {
        vector<WTASolution> population = initializePopulation(problem);

        for (int gen = 0; gen < maxGenerations; gen++) {
            // 评估适应度
            for (auto& individual : population) {
                FitnessEvaluator::updateSolutionMetrics(individual, problem);
            }

            // 排序
            sort(population.begin(), population.end(),
                [](const WTASolution& a, const WTASolution& b) {
                    return a.fitness > b.fitness;
                });

            // 输出进度
            if (gen % 100 == 0) {
                cout << "Generation " << gen << ", Best Fitness: " << population[0].fitness << endl;
            }

            // 生成新一代
            vector<WTASolution> newPopulation;

            // 保留最优个体
            newPopulation.push_back(population[0]);

            // 交叉和变异
            while (newPopulation.size() < populationSize) {
                auto parent1 = selectParent(population);
                auto parent2 = selectParent(population);

                auto offspring = crossover(parent1, parent2);
                mutate(offspring, problem);

                newPopulation.push_back(offspring);
            }

            population = newPopulation;
        }

        // 最终评估
        for (auto& individual : population) {
            FitnessEvaluator::updateSolutionMetrics(individual, problem);
        }

        sort(population.begin(), population.end(),
            [](const WTASolution& a, const WTASolution& b) {
                return a.fitness > b.fitness;
            });

        return population[0];
    }

private:
    vector<WTASolution> initializePopulation(const WTAProblem& problem) {
        vector<WTASolution> population;
        uniform_int_distribution<int> targetDis(0, problem.numTargets - 1);
        uniform_real_distribution<double> assignDis(0.0, 1.0);

        for (int i = 0; i < populationSize; i++) {
            WTASolution individual(problem.numWeapons);

            for (int w = 0; w < problem.numWeapons; w++) {
                if (assignDis(gen) < 0.8) { // 80%概率分配武器
                    individual.assignment[w] = targetDis(gen);
                }
                else {
                    individual.assignment[w] = -1; // 不分配
                }
            }

            population.push_back(individual);
        }

        return population;
    }

    WTASolution selectParent(const vector<WTASolution>& population) {
        // 锦标赛选择
        uniform_int_distribution<int> dis(0, population.size() - 1);
        int tournamentSize = 3;

        WTASolution best = population[dis(gen)];
        for (int i = 1; i < tournamentSize; i++) {
            WTASolution candidate = population[dis(gen)];
            if (candidate.fitness > best.fitness) {
                best = candidate;
            }
        }

        return best;
    }

    WTASolution crossover(const WTASolution& parent1, const WTASolution& parent2) {
        WTASolution offspring(parent1.assignment.size());
        uniform_real_distribution<double> dis(0.0, 1.0);

        for (int i = 0; i < offspring.assignment.size(); i++) {
            if (dis(gen) < crossoverRate) {
                offspring.assignment[i] = parent1.assignment[i];
            }
            else {
                offspring.assignment[i] = parent2.assignment[i];
            }
        }

        return offspring;
    }

    void mutate(WTASolution& individual, const WTAProblem& problem) {
        uniform_real_distribution<double> mutDis(0.0, 1.0);
        uniform_int_distribution<int> targetDis(0, problem.numTargets - 1);
        uniform_real_distribution<double> assignDis(0.0, 1.0);

        for (int i = 0; i < individual.assignment.size(); i++) {
            if (mutDis(gen) < mutationRate) {
                if (assignDis(gen) < 0.8) {
                    individual.assignment[i] = targetDis(gen);
                }
                else {
                    individual.assignment[i] = -1;
                }
            }
        }
    }
};

// 模拟退火算法求解器
class SimulatedAnnealing {
private:
    double initialTemp;
    double finalTemp;
    double coolingRate;
    int maxIterations;
    random_device rd;
    mt19937 gen;

public:
    SimulatedAnnealing(double initTemp = 1000.0, double finTemp = 0.01, double coolRate = 0.95, int maxIter = 10000)
        : initialTemp(initTemp), finalTemp(finTemp), coolingRate(coolRate), maxIterations(maxIter), gen(rd()) {}

    WTASolution solve(const WTAProblem& problem) {
        WTASolution current = generateRandomSolution(problem);
        FitnessEvaluator::updateSolutionMetrics(current, problem);

        WTASolution best = current;
        double temperature = initialTemp;

        uniform_real_distribution<double> acceptDis(0.0, 1.0);

        for (int iter = 0; iter < maxIterations && temperature > finalTemp; iter++) {
            WTASolution neighbor = generateNeighbor(current, problem);
            FitnessEvaluator::updateSolutionMetrics(neighbor, problem);

            double deltaE = neighbor.fitness - current.fitness;

            if (deltaE > 0 || acceptDis(gen) < exp(deltaE / temperature)) {
                current = neighbor;

                if (current.fitness > best.fitness) {
                    best = current;
                }
            }

            temperature *= coolingRate;

            if (iter % 1000 == 0) {
                cout << "Iteration " << iter << ", Best Fitness: " << best.fitness
                    << ", Temperature: " << temperature << endl;
            }
        }

        return best;
    }

private:
    WTASolution generateRandomSolution(const WTAProblem& problem) {
        WTASolution solution(problem.numWeapons);
        uniform_int_distribution<int> targetDis(0, problem.numTargets - 1);
        uniform_real_distribution<double> assignDis(0.0, 1.0);

        for (int i = 0; i < problem.numWeapons; i++) {
            if (assignDis(gen) < 0.8) {
                solution.assignment[i] = targetDis(gen);
            }
            else {
                solution.assignment[i] = -1;
            }
        }

        return solution;
    }

    WTASolution generateNeighbor(const WTASolution& current, const WTAProblem& problem) {
        WTASolution neighbor = current;
        uniform_int_distribution<int> weaponDis(0, problem.numWeapons - 1);
        uniform_int_distribution<int> targetDis(0, problem.numTargets - 1);
        uniform_real_distribution<double> operationDis(0.0, 1.0);

        int weapon = weaponDis(gen);

        if (operationDis(gen) < 0.5) {
            // 重新分配武器
            if (operationDis(gen) < 0.8) {
                neighbor.assignment[weapon] = targetDis(gen);
            }
            else {
                neighbor.assignment[weapon] = -1;
            }
        }
        else {
            // 交换两个武器的分配
            int weapon2 = weaponDis(gen);
            swap(neighbor.assignment[weapon], neighbor.assignment[weapon2]);
        }

        return neighbor;
    }
};

// Unity3D接口类
class Unity3DInterface {
public:
    struct VisualizationData {
        vector<pair<double, double>> weaponPositions;
        vector<pair<double, double>> targetPositions;
        vector<pair<int, int>> assignments; // (weapon_id, target_id)
        map<int, double> targetValues;
        map<int, double> threatValues;
        map<pair<int, int>, double> killProbabilities;
    };

    static VisualizationData prepareVisualizationData(const WTAProblem& problem, const WTASolution& solution) {
        VisualizationData data;

        // 生成武器和目标的位置（用于3D可视化）
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> posDis(-50.0, 50.0);

        // 武器位置（底部平台）
        for (int i = 0; i < problem.numWeapons; i++) {
            data.weaponPositions.push_back({ posDis(gen), -10.0 });
        }

        // 目标位置（上方空域）
        for (int i = 0; i < problem.numTargets; i++) {
            data.targetPositions.push_back({ posDis(gen), 20.0 });
        }

        // 分配关系
        for (int w = 0; w < problem.numWeapons; w++) {
            if (solution.assignment[w] != -1) {
                data.assignments.push_back({ w, solution.assignment[w] });
            }
        }

        // 目标数据
        for (int t = 0; t < problem.numTargets; t++) {
            data.targetValues[t] = problem.targetValues[t];
            data.threatValues[t] = problem.threatValues[t];
        }

        // 摧毁概率
        for (int w = 0; w < problem.numWeapons; w++) {
            for (int t = 0; t < problem.numTargets; t++) {
                data.killProbabilities[{w, t}] = problem.killProb[w][t];
            }
        }

        return data;
    }

    static void exportToJson(const VisualizationData& data, const string& filename) {
        ofstream file(filename);
        file << "{\n";

        // 武器位置
        file << "  \"weapons\": [\n";
        for (int i = 0; i < data.weaponPositions.size(); i++) {
            file << "    {\"id\": " << i
                << ", \"x\": " << data.weaponPositions[i].first
                << ", \"y\": " << data.weaponPositions[i].second << "}";
            if (i < data.weaponPositions.size() - 1) file << ",";
            file << "\n";
        }
        file << "  ],\n";

        // 目标位置
        file << "  \"targets\": [\n";
        for (int i = 0; i < data.targetPositions.size(); i++) {
            file << "    {\"id\": " << i
                << ", \"x\": " << data.targetPositions[i].first
                << ", \"y\": " << data.targetPositions[i].second
                << ", \"value\": " << data.targetValues.at(i)
                << ", \"threat\": " << data.threatValues.at(i) << "}";
            if (i < data.targetPositions.size() - 1) file << ",";
            file << "\n";
        }
        file << "  ],\n";

        // 分配关系
        file << "  \"assignments\": [\n";
        for (int i = 0; i < data.assignments.size(); i++) {
            file << "    {\"weapon\": " << data.assignments[i].first
                << ", \"target\": " << data.assignments[i].second << "}";
            if (i < data.assignments.size() - 1) file << ",";
            file << "\n";
        }
        file << "  ]\n";

        file << "}\n";
        file.close();
    }
};

// 性能测试器
class PerformanceTester {
public:
    struct TestResult {
        string algorithmName;
        int problemSize;
        double fitness;
        double executionTime;
        double totalValue;
        double totalThreatReduction;
    };

    static vector<TestResult> runComparison(const vector<pair<int, int>>& problemSizes) {
        vector<TestResult> results;
        WTAGenerator generator;

        for (const auto& size : problemSizes) {
            int weapons = size.first;
            int targets = size.second;

            cout << "\n=== Testing WTA" << weapons << " (Weapons: " << weapons
                << ", Targets: " << targets << ") ===" << endl;

            WTAProblem problem = generator.generateProblem(weapons, targets);

            // 测试遗传算法
            {
                cout << "\nTesting Genetic Algorithm..." << endl;
                GeneticAlgorithm ga(50, 500); // 减少参数以加快测试

                auto start = high_resolution_clock::now();
                WTASolution solution = ga.solve(problem);
                auto end = high_resolution_clock::now();

                double duration = duration_cast<milliseconds>(end - start).count() / 1000.0;

                TestResult result;
                result.algorithmName = "Genetic Algorithm";
                result.problemSize = weapons;
                result.fitness = solution.fitness;
                result.executionTime = duration;
                result.totalValue = solution.totalValue;
                result.totalThreatReduction = solution.totalThreatReduction;

                results.push_back(result);
            }

            // 测试模拟退火
            {
                cout << "\nTesting Simulated Annealing..." << endl;
                SimulatedAnnealing sa(1000.0, 0.01, 0.98, 5000); // 减少参数以加快测试

                auto start = high_resolution_clock::now();
                WTASolution solution = sa.solve(problem);
                auto end = high_resolution_clock::now();

                double duration = duration_cast<milliseconds>(end - start).count() / 1000.0;

                TestResult result;
                result.algorithmName = "Simulated Annealing";
                result.problemSize = weapons;
                result.fitness = solution.fitness;
                result.executionTime = duration;
                result.totalValue = solution.totalValue;
                result.totalThreatReduction = solution.totalThreatReduction;

                results.push_back(result);
            }
        }

        return results;
    }

    static void printResults(const vector<TestResult>& results) {
        cout << "\n" << string(120, '=') << endl;
        cout << "PERFORMANCE COMPARISON RESULTS" << endl;
        cout << string(120, '=') << endl;

        cout << setw(20) << left << "Algorithm"
            << setw(12) << "Problem Size"
            << setw(15) << "Fitness"
            << setw(12) << "Time (s)"
            << setw(15) << "Total Value"
            << setw(20) << "Threat Reduction" << endl;
        cout << string(120, '-') << endl;

        for (const auto& result : results) {
            cout << setw(20) << left << result.algorithmName
                << setw(12) << result.problemSize
                << setw(15) << fixed << setprecision(2) << result.fitness
                << setw(12) << setprecision(3) << result.executionTime
                << setw(15) << setprecision(2) << result.totalValue
                << setw(20) << setprecision(2) << result.totalThreatReduction << endl;
        }
    }

    static void exportResultsToCSV(const vector<TestResult>& results, const string& filename) {
        ofstream file(filename);
        file << "Algorithm,Problem Size,Fitness,Execution Time,Total Value,Threat Reduction\n";

        for (const auto& result : results) {
            file << result.algorithmName << ","
                << result.problemSize << ","
                << result.fitness << ","
                << result.executionTime << ","
                << result.totalValue << ","
                << result.totalThreatReduction << "\n";
        }

        file.close();
        cout << "\nResults exported to " << filename << endl;
    }
};

// 主程序
int main() {
    cout << "WTA (Weapon-Target Assignment) Problem Solver" << endl;
    cout << "=============================================" << endl;

    // 定义测试问题规模（根据文档中的WTA1-WTA12）
    vector<pair<int, int>> problemSizes = {
        {5, 5},     // WTA1
        {10, 10},   // WTA2
        {20, 20},   // WTA3
        {30, 30},   // WTA4
        {40, 40},   // WTA5
        {50, 50},   // WTA6
    };

    // 运行性能比较测试
    auto results = PerformanceTester::runComparison(problemSizes);
    PerformanceTester::printResults(results);
    PerformanceTester::exportResultsToCSV(results, "wta_results.csv");

    // 演示Unity3D可视化数据导出
    cout << "\n=== Unity3D Visualization Demo ===" << endl;
    WTAGenerator generator;
    WTAProblem demoProblem = generator.generateProblem(5, 5);

    GeneticAlgorithm ga(30, 200);
    WTASolution demoSolution = ga.solve(demoProblem);

    auto vizData = Unity3DInterface::prepareVisualizationData(demoProblem, demoSolution);
    Unity3DInterface::exportToJson(vizData, "wta_visualization.json");

    cout << "\nVisualization data exported to wta_visualization.json" << endl;
    cout << "This file can be imported into Unity3D for 3D visualization." << endl;

    // 输出最优解详情
    cout << "\n=== Demo Solution Details ===" << endl;
    cout << "Total Fitness: " << demoSolution.fitness << endl;
    cout << "Total Value: " << demoSolution.totalValue << endl;
    cout << "Threat Reduction: " << demoSolution.totalThreatReduction << endl;
    cout << "\nWeapon Assignments:" << endl;
    for (int i = 0; i < demoSolution.assignment.size(); i++) {
        cout << "Weapon " << i << " -> ";
        if (demoSolution.assignment[i] == -1) {
            cout << "Unassigned" << endl;
        }
        else {
            cout << "Target " << demoSolution.assignment[i] << endl;
        }
    }

    return 0;
}