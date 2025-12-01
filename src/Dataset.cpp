#include "Dataset.h"
#include <iomanip>
#include <random>
#include <algorithm>

std::pair<Dataset, Dataset> Dataset::split(double trainRatio) const {
    Dataset trainSet, testSet;
    trainSet.featureNames = featureNames;
    trainSet.targetName = targetName;
    testSet.featureNames = featureNames;
    testSet.targetName = targetName;
    
    std::vector<DataExample> shuffled = examples;
    
    // Используем современный shuffle вместо устаревшего random_shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffled.begin(), shuffled.end(), g);
    
    size_t trainSize = static_cast<size_t>(examples.size() * trainRatio);
    
    for (size_t i = 0; i < shuffled.size(); i++) {
        if (i < trainSize) {
            trainSet.examples.push_back(shuffled[i]);
        } else {
            testSet.examples.push_back(shuffled[i]);
        }
    }
    
    return {trainSet, testSet};
}