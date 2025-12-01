#ifndef CART_H
#define CART_H

#include "DecisionTree.h"

class CARTTree : public DecisionTree {
private:
    double giniIndex(const std::vector<DataExample>& examples) const;
    double giniGain(const std::vector<DataExample>& examples,
                   const std::string& feature) const;
    
protected:
    double calculateImpurity(const std::vector<DataExample>& examples) const override;
    std::pair<std::string, double> findBestSplit(
        const std::vector<DataExample>& examples,
        const std::vector<std::string>& availableFeatures) const override;
    std::shared_ptr<TreeNode> buildTreeRecursive(
        const std::vector<DataExample>& examples,
        const std::vector<std::string>& availableFeatures,
        int depth) override;
    
public:
    CARTTree() : isClassification(true) {}
    
    void train(const Dataset& dataset) override;
    std::string predict(const DataExample& example) const override;
    
    // Для регрессии
    double predictRegression(const DataExample& example) const;
    
    // Прунинг по стоимости-сложности
    void costComplexityPrune(const Dataset& validationSet);
    
private:
    bool isClassification;  // true для классификации, false для регрессии
    int maxDepth = 10;
    int minSamplesSplit = 2;
    double minImpurityDecrease = 0.0;
    
    // Для регрессии
    double mse(const std::vector<DataExample>& examples) const;
};
#endif // CART_H