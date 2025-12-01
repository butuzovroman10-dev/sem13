#include "CHAID.h"
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>

CHAIDTree::ChiSquareResult CHAIDTree::chiSquareTest(
    const std::vector<DataExample>& examples,
    const std::string& feature) const {
    
    ChiSquareResult result;
    result.value = 0.0;
    result.pValue = 1.0;
    result.degreesOfFreedom = 0;
    
    if (examples.empty()) return result;
    
    // Собираем уникальные значения целевой переменной и признака
    std::vector<std::string> targetValues;
    std::vector<std::string> featureValues;
    
    for (const auto& ex : examples) {
        targetValues.push_back(ex.target);
        featureValues.push_back(ex.features.at(feature));
    }
    
    std::sort(targetValues.begin(), targetValues.end());
    targetValues.erase(std::unique(targetValues.begin(), targetValues.end()), targetValues.end());
    
    std::sort(featureValues.begin(), featureValues.end());
    featureValues.erase(std::unique(featureValues.begin(), featureValues.end()), featureValues.end());
    
    if (targetValues.size() < 2 || featureValues.size() < 2) {
        return result;
    }
    
    // Создаем таблицу сопряженности
    std::vector<std::vector<int>> observed(targetValues.size(), 
                                          std::vector<int>(featureValues.size(), 0));
    
    // Заполняем таблицу наблюдаемых частот
    for (const auto& ex : examples) {
        auto targetIt = std::find(targetValues.begin(), targetValues.end(), ex.target);
        auto featureIt = std::find(featureValues.begin(), featureValues.end(), 
                                  ex.features.at(feature));
        
        if (targetIt != targetValues.end() && featureIt != featureValues.end()) {
            int row = std::distance(targetValues.begin(), targetIt);
            int col = std::distance(featureValues.begin(), featureIt);
            observed[row][col]++;
        }
    }
    
    // Рассчитываем ожидаемые частоты
    std::vector<int> rowSums(targetValues.size(), 0);
    std::vector<int> colSums(featureValues.size(), 0);
    int total = examples.size();
    
    for (size_t i = 0; i < targetValues.size(); ++i) {
        for (size_t j = 0; j < featureValues.size(); ++j) {
            rowSums[i] += observed[i][j];
            colSums[j] += observed[i][j];
        }
    }
    
    // Рассчитываем статистику хи-квадрат
    double chiSquare = 0.0;
    for (size_t i = 0; i < targetValues.size(); ++i) {
        for (size_t j = 0; j < featureValues.size(); ++j) {
            double expected = static_cast<double>(rowSums[i] * colSums[j]) / total;
            if (expected > 0) {
                double diff = observed[i][j] - expected;
                chiSquare += (diff * diff) / expected;
            }
        }
    }
    
    result.value = chiSquare;
    result.degreesOfFreedom = (targetValues.size() - 1) * (featureValues.size() - 1);
    
    // Упрощенный расчет p-value (в реальной реализации нужно использовать таблицы или функции)
    if (result.degreesOfFreedom > 0) {
        // Аппроксимация: чем больше хи-квадрат, тем меньше p-value
        result.pValue = std::exp(-chiSquare / (2 * result.degreesOfFreedom));
    }
    
    return result;
}

double CHAIDTree::calculateImpurity(const std::vector<DataExample>& examples) const {
    // Для CHAID используем p-value как меру неоднородности
    if (examples.empty()) return 1.0;
    
    // Чем меньше p-value, тем больше неоднородность
    // Преобразуем так, чтобы большие значения соответствовали большей неоднородности
    return 0.0; // Упрощенная реализация
}

std::pair<std::string, double> CHAIDTree::findBestSplit(
    const std::vector<DataExample>& examples,
    const std::vector<std::string>& availableFeatures) const {
    
    if (examples.empty() || availableFeatures.empty()) {
        return {"", 0.0};
    }
    
    std::string bestFeature;
    double bestChiSquare = -1.0;
    
    for (const auto& feature : availableFeatures) {
        ChiSquareResult result = chiSquareTest(examples, feature);
        
        // Используем хи-квадрат статистику как меру важности
        // и проверяем значимость
        if (result.pValue < significanceLevel && result.value > bestChiSquare) {
            bestChiSquare = result.value;
            bestFeature = feature;
        }
    }
    
    return {bestFeature, bestChiSquare};
}

std::shared_ptr<TreeNode> CHAIDTree::buildTreeRecursive(
    const std::vector<DataExample>& examples,
    const std::vector<std::string>& availableFeatures,
    int depth) {
    
    auto node = std::make_shared<TreeNode>();
    node->samples = examples.size();
    
    if (examples.empty() || depth >= maxDepth) {
        node->isLeaf = true;
        node->decision = "Unknown";
        return node;
    }
    
    // Проверка на минимальный размер узла
    if (examples.size() < minParentSize) {
        node->isLeaf = true;
        
        // Определяем большинство класса
        std::map<std::string, int> classCounts;
        for (const auto& ex : examples) {
            classCounts[ex.target]++;
        }
        
        std::string majorityClass;
        int maxCount = -1;
        for (const auto& [className, count] : classCounts) {
            if (count > maxCount) {
                maxCount = count;
                majorityClass = className;
            }
        }
        
        node->decision = majorityClass;
        return node;
    }
    
    // Находим лучший признак для разделения
    auto [bestFeature, bestChiSquare] = findBestSplit(examples, availableFeatures);
    
    if (bestFeature.empty()) {
        node->isLeaf = true;
        
        std::map<std::string, int> classCounts;
        for (const auto& ex : examples) {
            classCounts[ex.target]++;
        }
        
        std::string majorityClass;
        int maxCount = -1;
        for (const auto& [className, count] : classCounts) {
            if (count > maxCount) {
                maxCount = count;
                majorityClass = className;
            }
        }
        
        node->decision = majorityClass;
        return node;
    }
    
    node->isLeaf = false;
    node->feature = bestFeature;
    
    // Создаем подмножества по значениям признака
    std::map<std::string, std::vector<DataExample>> subsets;
    for (const auto& ex : examples) {
        std::string value = ex.features.at(bestFeature);
        subsets[value].push_back(ex);
    }
    
    // Новый список признаков (без использованного)
    std::vector<std::string> newFeatures;
    for (const auto& feature : availableFeatures) {
        if (feature != bestFeature) {
            newFeatures.push_back(feature);
        }
    }
    
    // Рекурсивное построение поддеревьев
    for (const auto& [value, subset] : subsets) {
        if (subset.size() < minChildSize) {
            // Слишком мало примеров, создаем лист
            auto leafNode = std::make_shared<TreeNode>();
            leafNode->isLeaf = true;
            
            // Используем большинство класс родителя
            std::map<std::string, int> parentClassCounts;
            for (const auto& ex : examples) {
                parentClassCounts[ex.target]++;
            }
            
            std::string majorityClass;
            int maxCount = -1;
            for (const auto& [className, count] : parentClassCounts) {
                if (count > maxCount) {
                    maxCount = count;
                    majorityClass = className;
                }
            }
            
            leafNode->decision = majorityClass;
            leafNode->samples = subset.size();
            node->children[value] = leafNode;
        } else {
            node->children[value] = buildTreeRecursive(subset, newFeatures, depth + 1);
        }
    }
    
    return node;
}

void CHAIDTree::train(const Dataset& dataset) {
    features = dataset.getFeatureNames();
    targetName = dataset.getTargetName();
    root = buildTreeRecursive(dataset.getExamples(), features, 0);
}

std::string CHAIDTree::predict(const DataExample& example) const {
    std::shared_ptr<TreeNode> currentNode = root;
    
    while (currentNode && !currentNode->isLeaf) {
        std::string featureValue = example.features.at(currentNode->feature);
        
        if (currentNode->children.find(featureValue) == currentNode->children.end()) {
            // Значение не найдено
            return "Unknown";
        }
        
        currentNode = currentNode->children.at(featureValue);
    }
    
    return currentNode ? currentNode->decision : "Unknown";
}