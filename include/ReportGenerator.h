#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include <vector>
#include <string>
#include <fstream>
#include "DecisionTree.h"
#include "Dataset.h"

struct AlgorithmResult {
    std::string algorithmName;
    double accuracy;
    double precision;
    double recall;
    double f1Score;
    int treeDepth;
    int nodeCount;
    double trainingTime;
    std::string dotFilePath;
};

class ReportGenerator {
public:
    // Генерация полного отчета
    static void generateFullReport(
        const std::vector<AlgorithmResult>& results,
        const Dataset& dataset,
        const std::string& filename);
    
    // Генерация сравнительной таблицы
    static std::string generateComparisonTable(
        const std::vector<AlgorithmResult>& results);
    
    // Генерация секции с описанием алгоритмов
    static std::string generateAlgorithmsDescription();
    
    // Генерация секции с данными
    static std::string generateDatasetDescription(const Dataset& dataset);
    
    // Создание интерактивной HTML страницы
    static void createInteractiveReport(
        const std::vector<AlgorithmResult>& results,
        const Dataset& dataset,
        const std::string& filename);
};
#endif // REPORT_GENERATOR_H