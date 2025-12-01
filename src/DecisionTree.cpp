#include "DecisionTree.h"
#include <fstream>
#include <queue>
#include <iomanip>
#include <sstream>

using namespace std;

int DecisionTree::getTreeDepth(shared_ptr<TreeNode> node) const {
    if (!node || node->isLeaf) return 0;
    
    int maxDepth = 0;
    for (const auto& child : node->children) {
        int childDepth = getTreeDepth(child.second);
        if (childDepth > maxDepth) {
            maxDepth = childDepth;
        }
    }
    return maxDepth + 1;
}

int DecisionTree::countNodes(shared_ptr<TreeNode> node) const {
    if (!node) return 0;
    if (node->isLeaf) return 1;
    
    int count = 1; // текущий узел
    for (const auto& child : node->children) {
        count += countNodes(child.second);
    }
    return count;
}

double DecisionTree::evaluate(const Dataset& testSet) const {
    int correct = 0;
    int total = 0;
    
    for (const auto& example : testSet.getExamples()) {
        string prediction = predict(example);
        if (prediction == example.target) {
            correct++;
        }
        total++;
    }
    
    return static_cast<double>(correct) / total;
}

// Вспомогательная функция для экранирования строк в DOT
string escapeDotString(const string& str) {
    string result;
    for (char c : str) {
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else if (c == '\n') result += "\\n";
        else if (c == '\r') result += "\\r";
        else if (c == '\t') result += "\\t";
        else result += c;
    }
    return result;
}

void DecisionTree::saveToDot(const string& filename) const {
    ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        cerr << "Error opening DOT file: " << filename << endl;
        return;
    }
    
    dotFile << "digraph DecisionTree {\n";
    dotFile << "    node [fontname=\"Arial\", fontsize=10];\n";
    dotFile << "    edge [fontname=\"Arial\", fontsize=9];\n\n";
    
    if (!root) {
        dotFile << "    empty [label=\"Empty tree\", shape=box, color=red];\n";
        dotFile << "}\n";
        dotFile.close();
        return;
    }
    
    // Используем BFS для обхода дерева
    int nodeCounter = 0;
    queue<pair<shared_ptr<TreeNode>, int>> nodesQueue;
    nodesQueue.push({root, nodeCounter++});
    
    // Мапа для хранения ID узлов
    map<shared_ptr<TreeNode>, int> nodeIds;
    nodeIds[root] = 0;
    
    while (!nodesQueue.empty()) {
        auto [currentNode, currentId] = nodesQueue.front();
        nodesQueue.pop();
        
        // Определяем стиль узла
        string nodeShape, nodeColor, nodeStyle;
        string label;
        
        if (currentNode->isLeaf) {
            nodeShape = "box";
            nodeColor = "lightgreen";
            nodeStyle = "filled";
            label = "Decision: " + currentNode->decision + "\\n";
            label += "Samples: " + to_string(currentNode->samples) + "\\n";
            label += "Confidence: " + to_string(currentNode->confidence).substr(0, 4);
        } else {
            nodeShape = "ellipse";
            nodeColor = "lightblue";
            nodeStyle = "filled";
            label = currentNode->feature + "\\n";
            label += "Samples: " + to_string(currentNode->samples);
        }
        
        // Записываем узел
        dotFile << "    node" << currentId << " [label=\"" << escapeDotString(label) << "\"";
        dotFile << ", shape=" << nodeShape;
        dotFile << ", style=" << nodeStyle;
        dotFile << ", fillcolor=" << nodeColor;
        dotFile << "];\n";
        
        // Добавляем дочерние узлы
        if (!currentNode->isLeaf) {
            for (const auto& childPair : currentNode->children) {
                const string& edgeLabel = childPair.first;
                auto childNode = childPair.second;
                
                // Проверяем, есть ли уже ID для этого узла
                if (nodeIds.find(childNode) == nodeIds.end()) {
                    nodeIds[childNode] = nodeCounter++;
                }
                
                int childId = nodeIds[childNode];
                
                // Добавляем ребро
                dotFile << "    node" << currentId << " -> node" << childId;
                dotFile << " [label=\"" << escapeDotString(edgeLabel) << "\"];\n";
                
                // Добавляем дочерний узел в очередь, если еще не обработан
                if (!childNode->isLeaf) {
                    nodesQueue.push({childNode, childId});
                }
            }
        }
    }
    
    // Добавляем листовые узлы, которые еще не были добавлены
    for (const auto& [node, id] : nodeIds) {
        if (node->isLeaf) {
            // Уже добавлены в основном цикле
            continue;
        }
        
        // Проверяем дочерние листовые узлы
        for (const auto& childPair : node->children) {
            auto childNode = childPair.second;
            if (childNode->isLeaf && nodeIds.find(childNode) == nodeIds.end()) {
                int childId = nodeCounter++;
                nodeIds[childNode] = childId;
                
                string label = "Decision: " + childNode->decision + "\\n";
                label += "Samples: " + to_string(childNode->samples) + "\\n";
                label += "Confidence: " + to_string(childNode->confidence).substr(0, 4);
                
                dotFile << "    node" << childId << " [label=\"" << escapeDotString(label) << "\"";
                dotFile << ", shape=box";
                dotFile << ", style=filled";
                dotFile << ", fillcolor=lightgreen";
                dotFile << "];\n";
            }
        }
    }
    
    dotFile << "}\n";
    dotFile.close();
}

void DecisionTree::printTree(shared_ptr<TreeNode> node, int depth) const {
    if (!node) {
        node = root;
        if (!node) {
            cout << "Empty tree" << endl;
            return;
        }
    }
    
    string indent(depth * 4, ' ');
    
    if (node->isLeaf) {
        cout << indent << "Leaf: " << node->decision 
             << " (samples: " << node->samples 
             << ", conf: " << fixed << setprecision(2) 
             << node->confidence << ")" << endl;
    } else {
        cout << indent << "Node: " << node->feature 
             << " (samples: " << node->samples << ")" << endl;
        
        for (const auto& child : node->children) {
            cout << indent << "  └── " << child.first << ":" << endl;
            printTree(child.second, depth + 2);
        }
    }
}