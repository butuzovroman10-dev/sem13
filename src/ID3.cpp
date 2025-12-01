#include "ID3.h"
#include <iomanip>
#include <map>
#include <algorithm>
#include <cmath>

double ID3Tree::entropy(const std::vector<DataExample>& examples) const {
    if (examples.empty()) return 0.0;
    
    std::map<std::string, int> classCounts;
    for (const auto& ex : examples) {
        classCounts[ex.target]++;
    }
    
    double entropy = 0.0;
    for (const auto& [className, count] : classCounts) {
        double probability = static_cast<double>(count) / examples.size();
        entropy -= probability * log2(probability);
    }
    
    return entropy;
}

double ID3Tree::informationGain(const std::vector<DataExample>& examples,
                               const std::string& feature,
                               double parentEntropy) const {
    std::map<std::string, std::vector<DataExample>> subsets;
    
    for (const auto& ex : examples) {
        std::string value = ex.features.at(feature);
        subsets[value].push_back(ex);
    }
    
    double weightedEntropy = 0.0;
    for (const auto& [value, subset] : subsets) {
        if (subset.empty()) continue;
        double subsetWeight = static_cast<double>(subset.size()) / examples.size();
        weightedEntropy += subsetWeight * entropy(subset);
    }
    
    return parentEntropy - weightedEntropy;
}

std::string ID3Tree::getMajorityClass(const std::vector<DataExample>& examples) const {
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
    
    return majorityClass;
}

std::vector<std::string> ID3Tree::getFeatureValues(const std::string& feature,
                                                  const std::vector<DataExample>& examples) const {
    std::vector<std::string> values;
    for (const auto& ex : examples) {
        values.push_back(ex.features.at(feature));
    }
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    return values;
}

double ID3Tree::calculateImpurity(const std::vector<DataExample>& examples) const {
    return entropy(examples);
}

std::pair<std::string, double> ID3Tree::findBestSplit(
    const std::vector<DataExample>& examples,
    const std::vector<std::string>& availableFeatures) const {
    
    if (examples.empty() || availableFeatures.empty()) {
        return {"", 0.0};
    }
    
    double parentEntropy = entropy(examples);
    std::string bestFeature;
    double bestGain = -1.0;
    
    for (const auto& feature : availableFeatures) {
        double gain = informationGain(examples, feature, parentEntropy);
        if (gain > bestGain) {
            bestGain = gain;
            bestFeature = feature;
        }
    }
    
    return {bestFeature, bestGain};
}

std::shared_ptr<TreeNode> ID3Tree::buildTreeRecursive(
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
    for (const auto& ex : examples) {
        if (ex.target != firstClass) {
            allSameClass = false;
            break;
        }
    }
    
    if (allSameClass || availableFeatures.empty() || depth >= maxDepth) {
        node->isLeaf = true;
        node->decision = getMajorityClass(examples);
        node->confidence = 1.0; // Упрощенный расчет уверенности
        return node;
    }
    
    // Поиск лучшего признака для разделения
    auto [bestFeature, bestGain] = findBestSplit(examples, availableFeatures);
    
    if (bestFeature.empty() || bestGain < 0.001) {
        node->isLeaf = true;
        node->decision = getMajorityClass(examples);
        return node;
    }
    
    node->isLeaf = false;
    node->feature = bestFeature;
    
    // Создание подмножеств
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
        if (subset.empty()) {
            auto leafNode = std::make_shared<TreeNode>();
            leafNode->isLeaf = true;
            leafNode->decision = getMajorityClass(examples);
            leafNode->samples = 0;
            node->children[value] = leafNode;
        } else {
            node->children[value] = buildTreeRecursive(subset, newFeatures, depth + 1);
        }
    }
    
    return node;
}

void ID3Tree::train(const Dataset& dataset) {
    features = dataset.getFeatureNames();
    targetName = dataset.getTargetName();
    
    root = buildTreeRecursive(dataset.getExamples(), features, 0);
}

std::string ID3Tree::predict(const DataExample& example) const {
    std::shared_ptr<TreeNode> currentNode = root;
    
    while (currentNode && !currentNode->isLeaf) {
        std::string featureValue = example.features.at(currentNode->feature);
        
        if (currentNode->children.find(featureValue) == currentNode->children.end()) {
            // Значение не найдено в обучающих данных
            return "Unknown";
        }
        
        currentNode = currentNode->children.at(featureValue);
    }
    
    return currentNode ? currentNode->decision : "Unknown";
}