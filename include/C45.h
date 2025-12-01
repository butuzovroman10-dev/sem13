#ifndef C45_H
#define C45_H

#include "DecisionTree.h"

class C45Tree : public DecisionTree {
private:
    double entropy(const std::vector<DataExample>& examples) const;
    double splitInfo(const std::vector<DataExample>& examples,
                    const std::string& feature) const;
    double gainRatio(const std::vector<DataExample>& examples,
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
    C45Tree() = default;
    
    void train(const Dataset& dataset) override;
    std::string predict(const DataExample& example) const override;
    
    // Поддержка непрерывных признаков
    void handleContinuousFeature(const std::string& feature,
                                const std::vector<DataExample>& examples);
    
    // Прунинг
    void pruneTree(std::shared_ptr<TreeNode> node,
                  const Dataset& validationSet);
    
private:
    int maxDepth = 10;
    int minSamplesSplit = 2;
    double minGainRatio = 0.01;
    
    // Для непрерывных признаков
    std::map<std::string, bool> isFeatureContinuous;
    std::map<std::string, double> splitThresholds;
};
#endif // C45_H