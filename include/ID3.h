#ifndef ID3_H
#define ID3_H

#include "DecisionTree.h"

class ID3Tree : public DecisionTree {
private:
    double entropy(const std::vector<DataExample>& examples) const;
    double informationGain(const std::vector<DataExample>& examples,
                          const std::string& feature,
                          double parentEntropy) const;
    
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
    ID3Tree() = default;
    
    void train(const Dataset& dataset) override;
    std::string predict(const DataExample& example) const override;
    
    // Специфичные для ID3 методы
    void setMaxDepth(int depth) { maxDepth = depth; }
    void setMinSamplesSplit(int minSamples) { minSamplesSplit = minSamples; }
    
private:
    int maxDepth = 10;
    int minSamplesSplit = 2;
    
    std::string getMajorityClass(const std::vector<DataExample>& examples) const;
    std::vector<std::string> getFeatureValues(const std::string& feature,
                                             const std::vector<DataExample>& examples) const;
};
#endif // ID3_H