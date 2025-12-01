#ifndef CHAID_H
#define CHAID_H

#include "DecisionTree.h"
#include <map>

class CHAIDTree : public DecisionTree {
private:
    struct ChiSquareResult {
        double value;
        double pValue;
        int degreesOfFreedom;
    };
    
    ChiSquareResult chiSquareTest(
        const std::vector<DataExample>& examples,
        const std::string& feature) const;
    
    // Объединение категорий
    std::vector<std::vector<std::string>> mergeCategories(
        const std::vector<DataExample>& examples,
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
    CHAIDTree() = default;
    
    void train(const Dataset& dataset) override;
    std::string predict(const DataExample& example) const override;
    
    // Настройки CHAID
    void setSignificanceLevel(double alpha) { significanceLevel = alpha; }
    void setMaxMergeIterations(int iterations) { maxMergeIterations = iterations; }
    
private:
    double significanceLevel = 0.05;
    int maxMergeIterations = 50;
    int minParentSize = 50;
    int minChildSize = 10;
    int maxDepth = 3;  // CHAID обычно строит неглубокие деревья
};
#endif // CHAID_H