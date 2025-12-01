#include "CART.h"
#include <iomanip>
#include <cmath>
#include <map>
#include <algorithm>

double CARTTree::giniIndex(const std::vector<DataExample>& examples) const {
    if (examples.empty()) return 0.0;
    
    std::map<std::string, int> classCounts;
    for (const auto& ex : examples) {
        classCounts[ex.target]++;
    }
    
    double gini = 1.0;
    for (const auto& [className, count] : classCounts) {
        double probability = static_cast<double>(count) / examples.size();
        gini -= probability * probability;
    }
    
    return gini;
}

double CARTTree::giniGain(const std::vector<DataExample>& examples,
                         const std::string& feature) const {
    double parentGini = giniIndex(examples);
    
    std::map<std::string, std::vector<DataExample>> subsets;
    for (const auto& ex : examples) {
        subsets[ex.features.at(feature)].push_back(ex);
    }
    
    double weightedGini = 0.0;
    for (const auto& [value, subset] : subsets) {
        double weight = static_cast<double>(subset.size()) / examples.size();
        weightedGini += weight * giniIndex(subset);
    }
    
    return parentGini - weightedGini;
}

double CARTTree::calculateImpurity(const std::vector<DataExample>& examples) const {
    return giniIndex(examples);
}

std::pair<std::string, double> CARTTree::findBestSplit(
    const std::vector<DataExample>& examples,
    const std::vector<std::string>& availableFeatures) const {
    
    if (examples.empty() || availableFeatures.empty()) {
        return {"", 0.0};
    }
    
    std::string bestFeature;
    double bestGiniGain = -1.0;
    
    for (const auto& feature : availableFeatures) {
        double gain = giniGain(examples, feature);
        if (gain > bestGiniGain && gain >= minImpurityDecrease) {
            bestGiniGain = gain;
            bestFeature = feature;
        }
    }
    
    return {bestFeature, bestGiniGain};
}

std::shared_ptr<TreeNode> CARTTree::buildTreeRecursive(
    const std::vector<DataExample>& examples,
    const std::vector<std::string>& availableFeatures,
    int depth) {
    
    auto node = std::make_shared<TreeNode>();
    node->samples = examples.size();
    
    if (examples.empty()) {
        node->isLeaf = true;
        node->decision = "Unknown";
        return node;
    }
    
    // Проверка на однородность
    bool allSameClass = true;
    std::string firstClass = examples[0].target;
    for (size_t i = 1; i < examples.size(); ++i) {
        if (examples[i].target != firstClass) {
            allSameClass = false;
            break;
        }
    }
    
    if (allSameClass || availableFeatures.empty() || depth >= maxDepth || 
        examples.size() < minSamplesSplit) {
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
        node->confidence = static_cast<double>(maxCount) / examples.size();
        return node;
    }
    
    // Находим лучший признак для разделения
    auto [bestFeature, bestGain] = findBestSplit(examples, availableFeatures);
    
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
    
    // Для CART создаем бинарные разбиения
    // Упрощенная версия: используем все значения признака как отдельные ветки
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
        if (subset.size() < minSamplesSplit) {
            // Если слишком мало примеров, создаем лист
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

void CARTTree::train(const Dataset& dataset) {
    features = dataset.getFeatureNames();
    targetName = dataset.getTargetName();
    root = buildTreeRecursive(dataset.getExamples(), features, 0);
}

std::string CARTTree::predict(const DataExample& example) const {
    std::shared_ptr<TreeNode> currentNode = root;
    
    while (currentNode && !currentNode->isLeaf) {
        std::string featureValue = example.features.at(currentNode->feature);
        
        if (currentNode->children.find(featureValue) == currentNode->children.end()) {
            // Значение не найдено, возвращаем Unknown
            return "Unknown";
        }
        
        currentNode = currentNode->children.at(featureValue);
    }
    
    return currentNode ? currentNode->decision : "Unknown";
}