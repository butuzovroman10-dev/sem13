#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <cstdlib>

#include "Dataset.h"
#include "ID3.h"
#include "C45.h"
#include "CART.h"
#include "CHAID.h"
#include "ReportGenerator.h"

using namespace std;
using namespace std::chrono;

// Функция для расчета метрик
AlgorithmResult evaluateAlgorithm(DecisionTree& tree,
                                 const Dataset& trainSet,
                                 const Dataset& testSet,
                                 const string& algorithmName) {
    
    AlgorithmResult result;
    result.algorithmName = algorithmName;
    
    auto start = high_resolution_clock::now();
    
    // Обучение
    tree.train(trainSet);
    
    auto end = high_resolution_clock::now();
    result.trainingTime = duration_cast<milliseconds>(end - start).count() / 1000.0;
    
    // Тестирование
    int correct = 0;
    int total = 0;
    int truePositives = 0, falsePositives = 0, falseNegatives = 0;
    
    for (const auto& example : testSet.getExamples()) {
        string prediction = tree.predict(example);
        string actual = example.target;
        
        if (prediction == actual) {
            correct++;
        }
        
        // Для бинарной классификации
        if (prediction == "Одобрить" && actual == "Одобрить") truePositives++;
        if (prediction == "Одобрить" && actual == "Отказать") falsePositives++;
        if (prediction == "Отказать" && actual == "Одобрить") falseNegatives++;
        
        total++;
    }
    
    result.accuracy = static_cast<double>(correct) / total;
    
    // Вычисление метрик
    if (truePositives + falsePositives > 0) {
        result.precision = static_cast<double>(truePositives) / (truePositives + falsePositives);
    } else {
        result.precision = 0.0;
    }
    
    if (truePositives + falseNegatives > 0) {
        result.recall = static_cast<double>(truePositives) / (truePositives + falseNegatives);
    } else {
        result.recall = 0.0;
    }
    
    if (result.precision + result.recall > 0) {
        result.f1Score = 2 * result.precision * result.recall / (result.precision + result.recall);
    } else {
        result.f1Score = 0.0;
    }
    
    // Характеристики дерева
    result.treeDepth = tree.getTreeDepth(tree.getRoot());
    result.nodeCount = tree.countNodes(tree.getRoot());
    
    // Генерация DOT файла
    string dotFilename = "output/trees/" + algorithmName + "_tree.dot";
    tree.saveToDot(dotFilename);
    result.dotFilePath = dotFilename;
    
    return result;
}

// Функция для генерации изображений деревьев
void generateTreeImages() {
    cout << "\nГенерация изображений деревьев..." << endl;
    
    vector<string> algorithms = {"ID3", "C4.5", "CART", "CHAID"};
    
    for (const auto& algo : algorithms) {
        string dotFile = "output/trees/" + algo + "_tree.dot";
        string pngFile = "output/trees/" + algo + "_tree.png";
        
        // Проверяем существование DOT файла
        ifstream test(dotFile);
        if (!test.good()) {
            cout << "  Предупреждение: " << dotFile << " не найден" << endl;
            continue;
        }
        test.close();
        
        // Генерируем PNG
        string command = "dot -Tpng \"" + dotFile + "\" -o \"" + pngFile + "\" 2>/dev/null";
        int result = system(command.c_str());
        if (result == 0) {
            cout << "  Сгенерировано: " << pngFile << endl;
        } else {
            cout << "  Не удалось сгенерировать PNG для " << algo << endl;
        }
    }
}

int main() {
    cout << "================================================" << endl;
    cout << "Сравнение алгоритмов деревьев решений" << endl;
    cout << "Предметная область: Выбор банка для кредита под бизнес" << endl;
    cout << "================================================" << endl << endl;
    
    // Создание набора данных
    Dataset dataset;
    dataset.createBankLoanData();
    
    // Разделение на обучающую и тестовую выборки
    auto [trainSet, testSet] = dataset.split(0.7);
    
    cout << "Размер обучающей выборки: " << trainSet.getExamples().size() << endl;
    cout << "Размер тестовой выборки: " << testSet.getExamples().size() << endl << endl;
    
    vector<AlgorithmResult> results;
    
    // 1. ID3 Algorithm
    cout << "1. Обучение дерева ID3..." << endl;
    {
        ID3Tree id3Tree;
        auto result = evaluateAlgorithm(id3Tree, trainSet, testSet, "ID3");
        results.push_back(result);
        cout << "   Точность: " << fixed << setprecision(2) 
             << result.accuracy * 100 << "%" << endl;
        cout << "   Глубина дерева: " << result.treeDepth << endl;
        id3Tree.printTree();
    }
    
    // 2. C4.5 Algorithm
    cout << "\n2. Обучение дерева C4.5..." << endl;
    {
        C45Tree c45Tree;
        auto result = evaluateAlgorithm(c45Tree, trainSet, testSet, "C4.5");
        results.push_back(result);
        cout << "   Точность: " << fixed << setprecision(2) 
             << result.accuracy * 100 << "%" << endl;
        cout << "   Глубина дерева: " << result.treeDepth << endl;
        c45Tree.printTree();
    }
    
    // 3. CART Algorithm
    cout << "\n3. Обучение дерева CART..." << endl;
    {
        CARTTree cartTree;
        auto result = evaluateAlgorithm(cartTree, trainSet, testSet, "CART");
        results.push_back(result);
        cout << "   Точность: " << fixed << setprecision(2) 
             << result.accuracy * 100 << "%" << endl;
        cout << "   Глубина дерева: " << result.treeDepth << endl;
        cartTree.printTree();
    }
    
    // 4. CHAID Algorithm
    cout << "\n4. Обучение дерева CHAID..." << endl;
    {
        CHAIDTree chaidTree;
        auto result = evaluateAlgorithm(chaidTree, trainSet, testSet, "CHAID");
        results.push_back(result);
        cout << "   Точность: " << fixed << setprecision(2) 
             << result.accuracy * 100 << "%" << endl;
        cout << "   Глубина дерева: " << result.treeDepth << endl;
        chaidTree.printTree();
    }
    
    // Генерация отчетов
    cout << "\n5. Генерация отчетов..." << endl;
    
    // Создание директорий
    system("mkdir -p output/trees output/reports output/visualization");
    
    // Полный отчет на русском
    ReportGenerator::generateFullReport(results, dataset, "output/reports/full_report.html");
    cout << "   Сгенерирован: output/reports/full_report.html" << endl;
    
    // Интерактивный отчет
    ReportGenerator::createInteractiveReport(results, dataset, 
                                           "output/reports/interactive_report.html");
    cout << "   Сгенерирован: output/reports/interactive_report.html" << endl;
    
    // Генерация изображений деревьев
    generateTreeImages();
    
    // Итоговая таблица
    cout << "\n================================================" << endl;
    cout << "Итоговые результаты" << endl;
    cout << "================================================" << endl;
    cout << left << setw(12) << "Алгоритм" 
         << setw(12) << "Точность" 
         << setw(12) << "F1-Score" 
         << setw(12) << "Время(с)" 
         << setw(10) << "Глубина" << endl;
    cout << string(58, '-') << endl;
    
    for (const auto& result : results) {
        cout << left << setw(12) << result.algorithmName
             << setw(12) << fixed << setprecision(4) << result.accuracy
             << setw(12) << fixed << setprecision(4) << result.f1Score
             << setw(12) << fixed << setprecision(2) << result.trainingTime
             << setw(10) << result.treeDepth << endl;
    }
    
    // Нахождение лучшего алгоритма
    if (!results.empty()) {
        auto bestResult = max_element(results.begin(), results.end(),
            [](const AlgorithmResult& a, const AlgorithmResult& b) {
                return a.accuracy < b.accuracy;
            });
        
        cout << "\nЛучший алгоритм: " << bestResult->algorithmName 
             << " (Точность: " << fixed << setprecision(2) 
             << bestResult->accuracy * 100 << "%)" << endl;
    }
    
    // Инструкции
    cout << "\n================================================" << endl;
    cout << "Как просмотреть отчеты:" << endl;
    cout << "================================================" << endl;
    cout << "1. Откройте HTML отчет в браузере:" << endl;
    cout << "   firefox output/reports/full_report.html" << endl;
    cout << "   или" << endl;
    cout << "   xdg-open output/reports/full_report.html" << endl;
    cout << "\n2. Для визуализации деревьев:" << endl;
    cout << "   output/trees/*.png - изображения деревьев" << endl;
    cout << "   output/trees/*.svg - SVG версии (лучшее качество)" << endl;
    cout << "\n3. Для генерации SVG:" << endl;
    cout << "   dot -Tsvg output/trees/ID3_tree.dot -o output/trees/ID3_tree.svg" << endl;
    
    // Создание простого превью
    ofstream preview("output/visualization/preview.html");
    preview << "<!DOCTYPE html>\n";
    preview << "<html>\n<head>\n<meta charset='UTF-8'>\n";
    preview << "<title>Превью деревьев решений</title>\n";
    preview << "<style>body{font-family:Arial;margin:20px;text-align:center;}</style>\n";
    preview << "</head>\n<body>\n";
    preview << "<h1>Визуализация деревьев решений</h1>\n";
    preview << "<p><a href='../reports/full_report.html'>Полный отчет</a></p>\n";
    preview << "<div style='display:flex;flex-wrap:wrap;justify-content:center;gap:20px;'>\n";
    
    vector<string> algs = {"ID3", "C4.5", "CART", "CHAID"};
    for (const auto& alg : algs) {
        preview << "<div style='border:1px solid #ddd;padding:10px;border-radius:5px;'>\n";
        preview << "<h3>" << alg << " Дерево</h3>\n";
        preview << "<img src='../trees/" << alg << "_tree.png' style='max-width:300px;'>\n";
        preview << "</div>\n";
    }
    
    preview << "</div>\n</body>\n</html>\n";
    preview.close();
    
    cout << "\n4. Быстрый просмотр:" << endl;
    cout << "   xdg-open output/visualization/preview.html" << endl;
    
    return 0;
}