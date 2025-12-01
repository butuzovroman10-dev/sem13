#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include "Dataset.h"
#include <memory>

// Структура узла дерева
struct TreeNode {
    bool isLeaf;
    std::string feature;          // Признак для разделения
    std::string decision;         // Решение (если лист)
    double splitValue;            // Значение разделения для непрерывных признаков
    std::map<std::string, std::shared_ptr<TreeNode>> children;
    double confidence;            // Уверенность в решении
    int samples;                  // Количество примеров в узле
    
    TreeNode() : isLeaf(false), splitValue(0.0), confidence(1.0), samples(0) {}
    
    std::string toString() const {
        if (isLeaf) {
            return "Leaf: " + decision + " (conf: " + std::to_string(confidence) 
                   + ", samples: " + std::to_string(samples) + ")";
        }
        return "Node: " + feature + " (samples: " + std::to_string(samples) + ")";
    }
};

// Абстрактный базовый класс для деревьев решений
class DecisionTree {
protected:
    std::shared_ptr<TreeNode> root;
    std::vector<std::string> features;
    std::string targetName;
    
    // Чистые виртуальные методы для реализации в дочерних классах
    virtual double calculateImpurity(const std::vector<DataExample>& examples) const = 0;
    virtual std::pair<std::string, double> findBestSplit(
        const std::vector<DataExample>& examples,
        const std::vector<std::string>& availableFeatures) const = 0;
    virtual std::shared_ptr<TreeNode> buildTreeRecursive(
        const std::vector<DataExample>& examples,
        const std::vector<std::string>& availableFeatures,
        int depth) = 0;
    
public:
    DecisionTree() = default;
    virtual ~DecisionTree() = default;
    
    virtual void train(const Dataset& dataset) = 0;
    virtual std::string predict(const DataExample& example) const = 0;
    
    // Общие методы
    std::shared_ptr<TreeNode> getRoot() const { return root; }
    int getTreeDepth(std::shared_ptr<TreeNode> node) const;
    int countNodes(std::shared_ptr<TreeNode> node) const;
    
    // Валидация
    double evaluate(const Dataset& testSet) const;
    
    // Визуализация
    virtual void saveToDot(const std::string& filename) const;
    virtual void printTree(std::shared_ptr<TreeNode> node = nullptr, 
                          int depth = 0) const;
};

#endif // DECISION_TREE_H