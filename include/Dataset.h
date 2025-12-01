#ifndef DATASET_H
#define DATASET_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>  // Добавляем этот include

// Структура для примера данных
struct DataExample {
    std::map<std::string, std::string> features;  // Признаки
    std::string target;                           // Целевая переменная
    int id;                                       // Идентификатор
    
    DataExample() : id(0) {}
    
    std::string toString() const {
        std::string result = "ID: " + std::to_string(id) + " -> ";
        for (const auto& [key, value] : features) {
            result += key + ": " + value + ", ";
        }
        result += "Целевой: " + target;
        return result;
    }
};

// Класс для работы с набором данных
class Dataset {
private:
    std::vector<DataExample> examples;
    std::vector<std::string> featureNames;
    std::string targetName;
    std::map<std::string, std::vector<std::string>> featureValues;
    
public:
    Dataset() = default;
    
    // Загрузка данных из CSV
    bool loadFromCSV(const std::string& filename, 
                    const std::vector<std::string>& features,
                    const std::string& target) {
        
        featureNames = features;
        targetName = target;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        int lineNum = 0;
        
        // Чтение заголовка
        if (std::getline(file, line)) {
            // Пропускаем заголовок или обрабатываем его
        }
        
        // Чтение данных
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell;
            DataExample example;
            example.id = ++lineNum;
            
            // Простая обработка CSV (без кавычек и escape-символов)
            size_t featureIndex = 0;
            while (std::getline(ss, cell, ',')) {
                if (featureIndex < features.size()) {
                    std::string featureName = features[featureIndex];
                    example.features[featureName] = cell;
                    featureValues[featureName].push_back(cell);
                    featureIndex++;
                } else {
                    example.target = cell;
                }
            }
            
            if (!example.features.empty() && !example.target.empty()) {
                examples.push_back(example);
            }
        }
        
        file.close();
        std::cout << "Загружено " << examples.size() << " примеров с " 
                  << features.size() << " признаками" << std::endl;
        return true;
    }
    
    // Создание тестовых данных для банковского кредита (расширенный набор)
    void createBankLoanData() {
        featureNames = {"Ставка", "Срок_рассмотрения", "Требования_к_залогу", "Репутация_банка"};
        targetName = "Решение";
        
        // РАСШИРЕННЫЙ НАБОР ДАННЫХ (26 примеров)
        std::vector<std::vector<std::string>> data = {
            // Примеры для ОДОБРЕНИЯ кредита (14 примеров)
            {"Низкая", "Быстрый", "Низкие", "Хорошая", "Одобрить"},
            {"Средняя", "Быстрый", "Средние", "Хорошая", "Одобрить"},
            {"Низкая", "Средний", "Низкие", "Хорошая", "Одобрить"},
            {"Средняя", "Средний", "Средние", "Средняя", "Одобрить"},
            {"Низкая", "Быстрый", "Низкие", "Средняя", "Одобрить"},
            {"Средняя", "Средний", "Низкие", "Хорошая", "Одобрить"},
            {"Низкая", "Средний", "Средние", "Средняя", "Одобрить"},
            {"Низкая", "Быстрый", "Средние", "Хорошая", "Одобрить"},
            {"Средняя", "Быстрый", "Низкие", "Средняя", "Одобрить"},
            {"Низкая", "Средний", "Низкие", "Средняя", "Одобрить"},
            {"Средняя", "Быстрый", "Средние", "Хорошая", "Одобрить"},
            {"Низкая", "Длительный", "Низкие", "Хорошая", "Одобрить"},
            {"Низкая", "Длительный", "Средние", "Средняя", "Одобрить"},
            {"Средняя", "Длительный", "Низкие", "Хорошая", "Одобрить"},
            
            // Примеры для ОТКАЗА в кредите (12 примеров)
            {"Высокая", "Длительный", "Высокие", "Плохая", "Отказать"},
            {"Высокая", "Длительный", "Высокие", "Средняя", "Отказать"},
            {"Высокая", "Длительный", "Средние", "Плохая", "Отказать"},
            {"Средняя", "Быстрый", "Высокие", "Хорошая", "Отказать"},
            {"Низкая", "Длительный", "Низкие", "Плохая", "Отказать"},
            {"Высокая", "Быстрый", "Высокие", "Средняя", "Отказать"},
            {"Высокая", "Быстрый", "Средние", "Плохая", "Отказать"},
            {"Высокая", "Средний", "Высокие", "Средняя", "Отказать"},
            {"Средняя", "Длительный", "Высокие", "Плохая", "Отказать"},
            {"Высокая", "Средний", "Средние", "Плохая", "Отказать"},
            {"Средняя", "Длительный", "Средние", "Хорошая", "Отказать"},
            {"Низкая", "Быстрый", "Высокие", "Хорошая", "Отказать"},
            {"Высокая", "Средний", "Низкие", "Средняя", "Отказать"},
            {"Средняя", "Быстрый", "Высокие", "Средняя", "Отказать"},
            {"Высокая", "Быстрый", "Низкие", "Хорошая", "Отказать"}
        };
        
        for (size_t i = 0; i < data.size(); i++) {
            DataExample example;
            example.id = i + 1;
            
            for (size_t j = 0; j < featureNames.size(); j++) {
                example.features[featureNames[j]] = data[i][j];
                featureValues[featureNames[j]].push_back(data[i][j]);
            }
            
            example.target = data[i][4];
            examples.push_back(example);
        }
        
        std::cout << "Создан набор данных для банковского кредита с " << examples.size() 
                  << " примерами" << std::endl;
    }
    
    // Получить все данные в виде таблицы (для отчета)
    std::string getDataTableHTML() const {
        std::stringstream html;
        html << "<table border='1' style='border-collapse: collapse; width: 100%; margin: 20px 0;'>\n";
        html << "<tr style='background-color: #4CAF50; color: white;'>\n";
        html << "<th>№</th>\n";
        html << "<th>Ставка</th>\n";
        html << "<th>Срок рассмотрения</th>\n";
        html << "<th>Требования к залогу</th>\n";
        html << "<th>Репутация банка</th>\n";
        html << "<th>Решение</th>\n";
        html << "</tr>\n";
        
        for (const auto& example : examples) {
            html << "<tr>\n";
            html << "<td>" << example.id << "</td>\n";
            html << "<td>" << example.features.at("Ставка") << "</td>\n";
            html << "<td>" << example.features.at("Срок_рассмотрения") << "</td>\n";
            html << "<td>" << example.features.at("Требования_к_залогу") << "</td>\n";
            html << "<td>" << example.features.at("Репутация_банка") << "</td>\n";
            html << "<td style='font-weight: bold; color: " 
                 << (example.target == "Одобрить" ? "green" : "red") << "'>"
                 << example.target << "</td>\n";
            html << "</tr>\n";
        }
        
        html << "</table>\n";
        
        // Статистика
        html << "<div style='background-color: #f9f9f9; padding: 15px; border-radius: 5px; margin: 20px 0;'>\n";
        html << "<h3>Статистика набора данных:</h3>\n";
        html << "<ul>\n";
        
        int approveCount = 0, rejectCount = 0;
        for (const auto& ex : examples) {
            if (ex.target == "Одобрить") approveCount++;
            else rejectCount++;
        }
        
        html << "<li>Всего примеров: " << examples.size() << "</li>\n";
        html << "<li>Одобрено кредитов: " << approveCount << " (" 
             << std::fixed << std::setprecision(1) 
             << (static_cast<double>(approveCount) / examples.size() * 100) << "%)</li>\n";
        html << "<li>Отказано в кредите: " << rejectCount << " (" 
             << std::fixed << std::setprecision(1) 
             << (static_cast<double>(rejectCount) / examples.size() * 100) << "%)</li>\n";
        
        // Статистика по признакам
        html << "<li><strong>Распределение по ставкам:</strong> ";
        std::map<std::string, int> rateCount;
        for (const auto& ex : examples) {
            rateCount[ex.features.at("Ставка")]++;
        }
        for (const auto& [rate, count] : rateCount) {
            html << rate << ": " << count << " (" 
                 << std::fixed << std::setprecision(1)
                 << (static_cast<double>(count) / examples.size() * 100) << "%), ";
        }
        html << "</li>\n";
        
        html << "</ul>\n";
        html << "</div>\n";
        
        return html.str();
    }
    
    // Геттеры
    const std::vector<DataExample>& getExamples() const { return examples; }
    const std::vector<std::string>& getFeatureNames() const { return featureNames; }
    const std::string& getTargetName() const { return targetName; }
    
    // Получение уникальных значений признака
    std::vector<std::string> getUniqueValues(const std::string& feature) const {
        if (featureValues.find(feature) == featureValues.end()) {
            return {};
        }
        std::vector<std::string> values = featureValues.at(feature);
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
        return values;
    }
    
    // Разделение данных на обучающие и тестовые
    std::pair<Dataset, Dataset> split(double trainRatio = 0.7) const {
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
};

#endif // DATASET_H