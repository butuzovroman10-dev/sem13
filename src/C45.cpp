#include "C45.h"
#include <iomanip>
#include <cmath>
#include <map>
#include <algorithm>

double C45Tree::entropy(const std::vector<DataExample>& examples) const {
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

double C45Tree::splitInfo(const std::vector<DataExample>& examples,
                         const std::string& feature) const {
    std::map<std::string, int> valueCounts;
    for (const auto& ex : examples) {
        valueCounts[ex.features.at(feature)]++;
    }
    
    double splitInfo = 0.0;
    for (const auto& [value, count] : valueCounts) {
        double probability = static_cast<double>(count) / examples.size();
        splitInfo -= probability * log2(probability);
    }
    
    return splitInfo;
}

double C45Tree::gainRatio(const std::vector<DataExample>& examples,
                         const std::string& feature,
                         double parentEntropy) const {
    // Рассчитываем информацию
    std::map<std::string, std::vector<DataExample>> subsets;
    for (const auto& ex : examples) {
        subsets[ex.features.at(feature)].push_back(ex);
    }
    
    double weightedEntropy = 0.0;
    for (const auto& [value, subset] : subsets) {
        double weight = static_cast<double>(subset.size()) / examples.size();
        weightedEntropy += weight * entropy(subset);
    }
    
    double informationGain = parentEntropy - weightedEntropy;
    double splitInformation = splitInfo(examples, feature);
    
    if (splitInformation == 0.0) return 0.0;
    return informationGain / splitInformation;
}

double C45Tree::calculateImpurity(const std::vector<DataExample>& examples) const {
    return entropy(examples);
}

std::pair<std::string, double> C45Tree::findBestSplit(
    const std::vector<DataExample>& examples,
    const std::vector<std::string>& availableFeatures) const {
    
    if (examples.empty() || availableFeatures.empty()) {
        return {"", 0.0};
    }
    
    double parentEntropy = entropy(examples);
    std::string bestFeature;
    double bestGainRatio = -1.0;
    
    for (const auto& feature : availableFeatures) {
        double ratio = gainRatio(examples, feature, parentEntropy);
        if (ratio > bestGainRatio && ratio > minGainRatio) {
            bestGainRatio = ratio;
            bestFeature = feature;
        }
    }
    
    return {bestFeature, bestGainRatio};
}

std::shared_ptr<TreeNode> C45Tree::buildTreeRecursive(
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
    
    if (allSameClass || availableFeatures.empty() || depth >= maxDepth) {
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
    auto [bestFeature, bestGainRatio] = findBestSplit(examples, availableFeatures);
    
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
    
    // Создаем подмножества
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
            auto leafNode = std::make_shared<TreeNode>();
            leafNode->isLeaf = true;
            
            // Определяем большинство класс родительского узла
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

void C45Tree::train(const Dataset& dataset) {
    features = dataset.getFeatureNames();
    targetName = dataset.getTargetName();
    
    // Инициализация всех признаков как дискретных
    for (const auto& feature : features) {
        isFeatureContinuous[feature] = false;
    }
    
    root = buildTreeRecursive(dataset.getExamples(), features, 0);
}

std::string C45Tree::predict(const DataExample& example) const {
    std::shared_ptr<TreeNode> currentNode = root;
    
    while (currentNode && !currentNode->isLeaf) {
        std::string featureValue = example.features.at(currentNode->feature);
        
        if (currentNode->children.find(featureValue) == currentNode->children.end()) {
            // Если значение не найдено, возвращаем наиболее вероятный класс
            return "Unknown";
        }
        
        currentNode = currentNode->children.at(featureValue);
    }
    
    return currentNode ? currentNode->decision : "Unknown";
}