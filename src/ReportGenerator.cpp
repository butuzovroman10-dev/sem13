#include "ReportGenerator.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;

void ReportGenerator::generateFullReport(
    const vector<AlgorithmResult>& results,
    const Dataset& dataset,
    const string& filename) {
    
    ofstream reportFile(filename);
    if (!reportFile.is_open()) {
        cerr << "Ошибка открытия файла отчета: " << filename << endl;
        return;
    }
    
    reportFile << "<!DOCTYPE html>\n";
    reportFile << "<html>\n";
    reportFile << "<head>\n";
    reportFile << "    <meta charset='UTF-8'>\n";
    reportFile << "    <title>Сравнение алгоритмов деревьев решений - Банковский кредит</title>\n";
    reportFile << "    <style>\n";
    reportFile << "        body { font-family: 'Segoe UI', Arial, sans-serif; margin: 40px; line-height: 1.6; background-color: #f5f5f5; }\n";
    reportFile << "        .container { max-width: 1200px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 20px rgba(0,0,0,0.1); }\n";
    reportFile << "        h1, h2, h3 { color: #2c3e50; }\n";
    reportFile << "        h1 { border-bottom: 3px solid #3498db; padding-bottom: 15px; text-align: center; }\n";
    reportFile << "        h2 { border-bottom: 2px solid #3498db; padding-bottom: 10px; margin-top: 40px; color: #2980b9; }\n";
    reportFile << "        h3 { color: #34495e; margin-top: 25px; }\n";
    reportFile << "        table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n";
    reportFile << "        th, td { border: 1px solid #ddd; padding: 12px; text-align: center; }\n";
    reportFile << "        th { background-color: #3498db; color: white; font-weight: bold; }\n";
    reportFile << "        tr:nth-child(even) { background-color: #f9f9f9; }\n";
    reportFile << "        tr:hover { background-color: #f5f5f5; }\n";
    reportFile << "        .best { background-color: #d4edda !important; font-weight: bold; }\n";
    reportFile << "        .metric-value { font-weight: bold; }\n";
    reportFile << "        .algorithm-card { background-color: #f8f9fa; padding: 20px; border-radius: 5px; margin: 20px 0; border-left: 5px solid #3498db; }\n";
    reportFile << "        .dataset-info { background-color: #e8f4f8; padding: 20px; border-radius: 5px; margin: 20px 0; }\n";
    reportFile << "        .footer { text-align: center; margin-top: 50px; color: #7f8c8d; font-size: 14px; padding-top: 20px; border-top: 1px solid #eee; }\n";
    reportFile << "        .success { color: #27ae60; font-weight: bold; }\n";
    reportFile << "        .warning { color: #e74c3c; font-weight: bold; }\n";
    reportFile << "        .info-box { background-color: #e3f2fd; padding: 15px; border-radius: 5px; margin: 15px 0; }\n";
    reportFile << "        .data-table { font-size: 12px; }\n";
    reportFile << "        .data-table th { background-color: #2c3e50; }\n";
    reportFile << "        .conclusion { background-color: #fff3cd; padding: 20px; border-radius: 5px; margin: 30px 0; border-left: 5px solid #ffc107; }\n";
    reportFile << "    </style>\n";
    reportFile << "</head>\n";
    reportFile << "<body>\n";
    reportFile << "    <div class='container'>\n";
    
    // Заголовок
    reportFile << "    <h1>Отчет по сравнению алгоритмов деревьев решений</h1>\n";
    reportFile << "    <h2 style='text-align: center; color: #16a085;'>Предметная область: Выбор банка для кредита под бизнес</h2>\n";
    reportFile << "    <p><strong>Сгенерировано:</strong> " << __DATE__ << " " << __TIME__ << "</p>\n";
    
    // Раздел 1: Описание набора данных
    reportFile << "    <h2>1. Описание набора данных</h2>\n";
    reportFile << "    <div class='dataset-info'>\n";
    reportFile << "        <p><strong>Целевой атрибут:</strong> Решение по кредиту (Одобрить/Отказать)</p>\n";
    reportFile << "        <p><strong>Количество примеров:</strong> " << dataset.getExamples().size() << "</p>\n";
    reportFile << "        <p><strong>Количество признаков:</strong> " << dataset.getFeatureNames().size() << "</p>\n";
    
    reportFile << "        <h3>Описание признаков:</h3>\n";
    reportFile << "        <table>\n";
    reportFile << "            <tr><th>Признак</th><th>Значения</th><th>Описание</th></tr>\n";
    reportFile << "            <tr><td>Ставка</td><td>Высокая, Средняя, Низкая</td><td>Процентная ставка по кредиту</td></tr>\n";
    reportFile << "            <tr><td>Срок_рассмотрения</td><td>Длительный, Средний, Быстрый</td><td>Время рассмотрения заявки</td></tr>\n";
    reportFile << "            <tr><td>Требования_к_залогу</td><td>Высокие, Средние, Низкие</td><td>Требуемое обеспечение</td></tr>\n";
    reportFile << "            <tr><td>Репутация_банка</td><td>Хорошая, Средняя, Плохая</td><td>Репутация банка на рынке</td></tr>\n";
    reportFile << "        </table>\n";
    reportFile << "    </div>\n";
    
    // Большая таблица с данными
    reportFile << "    <h3>Полная таблица данных:</h3>\n";
    reportFile << "    <div style='overflow-x: auto;'>\n";
    reportFile << dataset.getDataTableHTML();
    reportFile << "    </div>\n";
    
    // Раздел 2: Сравнительная таблица результатов
    reportFile << "    <h2>2. Сравнение производительности алгоритмов</h2>\n";
    
    if (results.empty()) {
        reportFile << "<p class='warning'>Нет результатов для отображения</p>\n";
    } else {
        // Найдем лучшие значения для каждой метрики
        vector<string> bestAlgorithms;
        if (!results.empty()) {
            // Лучшая точность
            auto bestAccuracy = max_element(results.begin(), results.end(),
                [](const AlgorithmResult& a, const AlgorithmResult& b) {
                    return a.accuracy < b.accuracy;
                });
            bestAlgorithms.push_back(bestAccuracy->algorithmName);
        }
        
        reportFile << "    <div class='info-box'>\n";
        reportFile << "        <p><strong>Метрики качества:</strong></p>\n";
        reportFile << "        <ul>\n";
        reportFile << "            <li><strong>Точность (Accuracy)</strong> - доля правильных предсказаний</li>\n";
        reportFile << "            <li><strong>Precision</strong> - точность предсказаний класса 'Одобрить'</li>\n";
        reportFile << "            <li><strong>Recall</strong> - полнота предсказаний класса 'Одобрить'</li>\n";
        reportFile << "            <li><strong>F1-Score</strong> - гармоническое среднее precision и recall</li>\n";
        reportFile << "        </ul>\n";
        reportFile << "    </div>\n";
        
        reportFile << "    <table>\n";
        reportFile << "        <tr>\n";
        reportFile << "            <th>Алгоритм</th>\n";
        reportFile << "            <th>Точность</th>\n";
        reportFile << "            <th>Precision</th>\n";
        reportFile << "            <th>Recall</th>\n";
        reportFile << "            <th>F1-Score</th>\n";
        reportFile << "            <th>Время обучения (с)</th>\n";
        reportFile << "            <th>Глубина дерева</th>\n";
        reportFile << "            <th>Количество узлов</th>\n";
        reportFile << "        </tr>\n";
        
        for (const auto& result : results) {
            bool isBest = find(bestAlgorithms.begin(), bestAlgorithms.end(), 
                               result.algorithmName) != bestAlgorithms.end();
            
            reportFile << "        <tr" << (isBest ? " class='best'" : "") << ">\n";
            reportFile << "            <td><strong>" << result.algorithmName << "</strong></td>\n";
            reportFile << "            <td class='metric-value'>" 
                      << fixed << setprecision(4) << result.accuracy << "</td>\n";
            reportFile << "            <td>" << fixed << setprecision(4) << result.precision << "</td>\n";
            reportFile << "            <td>" << fixed << setprecision(4) << result.recall << "</td>\n";
            reportFile << "            <td>" << fixed << setprecision(4) << result.f1Score << "</td>\n";
            reportFile << "            <td>" << fixed << setprecision(3) << result.trainingTime << "</td>\n";
            reportFile << "            <td>" << result.treeDepth << "</td>\n";
            reportFile << "            <td>" << result.nodeCount << "</td>\n";
            reportFile << "        </tr>\n";
        }
        
        reportFile << "    </table>\n";
        
        // Выводы
        if (!results.empty()) {
            auto bestResult = max_element(results.begin(), results.end(),
                [](const AlgorithmResult& a, const AlgorithmResult& b) {
                    return a.accuracy < b.accuracy;
                });
            
            reportFile << "    <div class='conclusion'>\n";
            reportFile << "        <h3>Выводы:</h3>\n";
            reportFile << "        <p><strong>Лучший алгоритм:</strong> " << bestResult->algorithmName 
                      << " с точностью " << fixed << setprecision(2) 
                      << bestResult->accuracy * 100 << "%</p>\n";
            
            // Анализ сложности деревьев
            auto simplestTree = min_element(results.begin(), results.end(),
                [](const AlgorithmResult& a, const AlgorithmResult& b) {
                    return a.nodeCount < b.nodeCount;
                });
            
            reportFile << "        <p><strong>Самое простое дерево:</strong> " << simplestTree->algorithmName 
                      << " (узлов: " << simplestTree->nodeCount << ", глубина: " 
                      << simplestTree->treeDepth << ")</p>\n";
            
            reportFile << "        <p><strong>Самое быстрое обучение:</strong> ";
            auto fastest = min_element(results.begin(), results.end(),
                [](const AlgorithmResult& a, const AlgorithmResult& b) {
                    return a.trainingTime < b.trainingTime;
                });
            reportFile << fastest->algorithmName << " (" << fixed << setprecision(3) 
                      << fastest->trainingTime << " сек.)</p>\n";
            reportFile << "    </div>\n";
        }
    }
    
    // Раздел 3: Описание алгоритмов
    reportFile << "    <h2>3. Описание алгоритмов</h2>\n";
    reportFile << generateAlgorithmsDescription();
    
    // Раздел 4: Визуализация
    reportFile << "    <h2>4. Визуализация деревьев решений</h2>\n";
    reportFile << "    <div class='info-box'>\n";
    reportFile << "        <p>Для генерации изображений деревьев используйте Graphviz:</p>\n";
    reportFile << "        <pre style='background-color: #f4f4f4; padding: 10px; border-radius: 5px; overflow-x: auto;'>\n";
    if (!results.empty()) {
        for (const auto& result : results) {
            reportFile << "dot -Tpng " << result.dotFilePath << " -o " 
                      << result.algorithmName << "_tree.png\n";
        }
    } else {
        reportFile << "dot -Tpng output/trees/ID3_tree.dot -o ID3_tree.png\n";
        reportFile << "dot -Tpng output/trees/C4.5_tree.dot -o C4.5_tree.png\n";
        reportFile << "dot -Tpng output/trees/CART_tree.dot -o CART_tree.png\n";
        reportFile << "dot -Tpng output/trees/CHAID_tree.dot -o CHAID_tree.png\n";
    }
    reportFile << "        </pre>\n";
    reportFile << "        <p>Или SVG для лучшего качества:</p>\n";
    reportFile << "        <pre style='background-color: #f4f4f4; padding: 10px; border-radius: 5px;'>\n";
    reportFile << "dot -Tsvg tree.dot -o tree.svg\n";
    reportFile << "        </pre>\n";
    reportFile << "    </div>\n";
    
    // Раздел 5: Рекомендации
    reportFile << "    <h2>5. Рекомендации для бизнеса</h2>\n";
    reportFile << "    <div class='algorithm-card'>\n";
    reportFile << "        <p>На основе анализа данных для выбора банка под бизнес-кредит:</p>\n";
    reportFile << "        <ol>\n";
    reportFile << "            <li><strong>Ставка</strong> - ключевой фактор: низкие ставки чаще приводят к одобрению</li>\n";
    reportFile << "            <li><strong>Срок рассмотрения</strong> - быстрые решения ассоциируются с одобрением</li>\n";
    reportFile << "            <li><strong>Требования к залогу</strong> - низкие требования повышают шансы</li>\n";
    reportFile << "            <li><strong>Репутация банка</strong> - важный, но не решающий фактор</li>\n";
    reportFile << "        </ol>\n";
    reportFile << "        <p><strong>Идеальный сценарий для одобрения:</strong> Низкая ставка + Быстрое рассмотрение + Низкие требования к залогу + Хорошая репутация</p>\n";
    reportFile << "    </div>\n";
    
    // Подвал
    reportFile << "    <div class='footer'>\n";
    reportFile << "        <p>Отчет сгенерирован системой сравнения алгоритмов деревьев решений</p>\n";
    reportFile << "        <p>Предметная область: Выбор банка для кредита под бизнес</p>\n";
    reportFile << "        <p>Используйте Graphviz для визуализации деревьев</p>\n";
    reportFile << "    </div>\n";
    
    reportFile << "    </div>\n"; // закрываем container
    reportFile << "</body>\n";
    reportFile << "</html>\n";
    
    reportFile.close();
}

string ReportGenerator::generateAlgorithmsDescription() {
    stringstream ss;
    
    ss << "<div class='algorithm-card'>\n";
    
    ss << "<h3>ID3 (Iterative Dichotomiser 3)</h3>\n";
    ss << "<ul>\n";
    ss << "<li><strong>Критерий разделения:</strong> Прирост информации (Information Gain)</li>\n";
    ss << "<li><strong>Тип дерева:</strong> Многовариантное разделение</li>\n";
    ss << "<li><strong>Преимущества:</strong> Прост в понимании, быстрый</li>\n";
    ss << "<li><strong>Недостатки:</strong> Нет упрощения дерева, чувствителен к шуму</li>\n";
    ss << "</ul>\n";
    
    ss << "<h3>C4.5 (улучшенная версия ID3)</h3>\n";
    ss << "<ul>\n";
    ss << "<li><strong>Критерий разделения:</strong> Коэффициент усиления (Gain Ratio)</li>\n";
    ss << "<li><strong>Тип дерева:</strong> Многовариантное разделение</li>\n";
    ss << "<li><strong>Преимущества:</strong> Работает с непрерывными признаками, выполняет упрощение</li>\n";
    ss << "<li><strong>Недостатки:</strong> Может переобучаться на маленьких наборах</li>\n";
    ss << "</ul>\n";
    
    ss << "<h3>CART (Classification and Regression Trees)</h3>\n";
    ss << "<ul>\n";
    ss << "<li><strong>Критерий разделения:</strong> Индекс Джини (Gini Index)</li>\n";
    ss << "<li><strong>Тип дерева:</strong> Бинарное разделение</li>\n";
    ss << "<li><strong>Преимущества:</strong> Работает и для классификации, и для регрессии</li>\n";
    ss << "<li><strong>Недостатки:</strong> Может создавать смещенные деревья</li>\n";
    ss << "</ul>\n";
    
    ss << "<h3>CHAID (Chi-squared Automatic Interaction Detection)</h3>\n";
    ss << "<ul>\n";
    ss << "<li><strong>Критерий разделения:</strong> Хи-квадрат тест</li>\n";
    ss << "<li><strong>Тип дерева:</strong> Многовариантное разделение</li>\n";
    ss << "<li><strong>Преимущества:</strong> Проверка статистической значимости</li>\n";
    ss << "<li><strong>Недостатки:</strong> Вычислительно сложный</li>\n";
    ss << "</ul>\n";
    
    ss << "</div>\n";
    
    return ss.str();
}

void ReportGenerator::createInteractiveReport(
    const vector<AlgorithmResult>& results,
    const Dataset& dataset,
    const string& filename) {
    
    ofstream htmlFile(filename);
    
    htmlFile << "<!DOCTYPE html>\n";
    htmlFile << "<html>\n";
    htmlFile << "<head>\n";
    htmlFile << "    <meta charset='UTF-8'>\n";
    htmlFile << "    <title>Интерактивный отчет - Сравнение алгоритмов</title>\n";
    htmlFile << "    <script src='https://cdn.plot.ly/plotly-latest.min.js'></script>\n";
    htmlFile << "    <style>\n";
    htmlFile << "        body { font-family: Arial, sans-serif; margin: 40px; }\n";
    htmlFile << "        h1 { color: #2c3e50; text-align: center; }\n";
    htmlFile << "        .container { max-width: 1200px; margin: 0 auto; }\n";
    htmlFile << "        .chart-container { margin: 30px 0; }\n";
    htmlFile << "        .data-table { width: 100%; border-collapse: collapse; margin: 20px 0; }\n";
    htmlFile << "        .data-table th, .data-table td { border: 1px solid #ddd; padding: 8px; }\n";
    htmlFile << "        .data-table th { background-color: #4CAF50; color: white; }\n";
    htmlFile << "    </style>\n";
    htmlFile << "</head>\n";
    htmlFile << "<body>\n";
    htmlFile << "    <div class='container'>\n";
    htmlFile << "        <h1>Интерактивный отчет: Выбор банка для кредита под бизнес</h1>\n";
    
    htmlFile << "        <h2>Данные для анализа</h2>\n";
    htmlFile << "        <div style='overflow-x: auto;'>\n";
    htmlFile << dataset.getDataTableHTML();
    htmlFile << "        </div>\n";
    
    htmlFile << "        <h2>Сравнение алгоритмов</h2>\n";
    htmlFile << "        <div id='accuracyChart' class='chart-container'></div>\n";
    
    htmlFile << "        <h2>Результаты</h2>\n";
    if (!results.empty()) {
        htmlFile << "        <table class='data-table'>\n";
        htmlFile << "            <tr><th>Алгоритм</th><th>Точность</th><th>F1-Score</th><th>Время (с)</th><th>Узлы</th></tr>\n";
        for (const auto& result : results) {
            htmlFile << "            <tr>\n";
            htmlFile << "                <td>" << result.algorithmName << "</td>\n";
            htmlFile << "                <td>" << fixed << setprecision(4) << result.accuracy << "</td>\n";
            htmlFile << "                <td>" << fixed << setprecision(4) << result.f1Score << "</td>\n";
            htmlFile << "                <td>" << fixed << setprecision(3) << result.trainingTime << "</td>\n";
            htmlFile << "                <td>" << result.nodeCount << "</td>\n";
            htmlFile << "            </tr>\n";
        }
        htmlFile << "        </table>\n";
    }
    
    htmlFile << "        <h2>Визуализация деревьев</h2>\n";
    htmlFile << "        <p>Для просмотра деревьев сгенерируйте изображения:</p>\n";
    htmlFile << "        <pre>\n";
    htmlFile << "cd ~/sem13/build\n";
    htmlFile << "dot -Tpng output/trees/ID3_tree.dot -o output/trees/ID3_tree.png\n";
    htmlFile << "dot -Tpng output/trees/C4.5_tree.dot -o output/trees/C4.5_tree.png\n";
    htmlFile << "dot -Tpng output/trees/CART_tree.dot -o output/trees/CART_tree.png\n";
    htmlFile << "dot -Tpng output/trees/CHAID_tree.dot -o output/trees/CHAID_tree.png\n";
    htmlFile << "        </pre>\n";
    
    // JavaScript для графиков
    htmlFile << "        <script>\n";
    htmlFile << "            // Данные для графика\n";
    htmlFile << "            var algorithms = [];\n";
    htmlFile << "            var accuracies = [];\n";
    htmlFile << "            var f1Scores = [];\n";
    
    if (!results.empty()) {
        for (const auto& result : results) {
            htmlFile << "            algorithms.push('" << result.algorithmName << "');\n";
            htmlFile << "            accuracies.push(" << result.accuracy << ");\n";
            htmlFile << "            f1Scores.push(" << result.f1Score << ");\n";
        }
    }
    
    htmlFile << "            \n";
    htmlFile << "            // Создаем график\n";
    htmlFile << "            var trace1 = {\n";
    htmlFile << "                x: algorithms,\n";
    htmlFile << "                y: accuracies,\n";
    htmlFile << "                name: 'Точность',\n";
    htmlFile << "                type: 'bar',\n";
    htmlFile << "                marker: {color: '#3498db'}\n";
    htmlFile << "            };\n";
    htmlFile << "            \n";
    htmlFile << "            var trace2 = {\n";
    htmlFile << "                x: algorithms,\n";
    htmlFile << "                y: f1Scores,\n";
    htmlFile << "                name: 'F1-Score',\n";
    htmlFile << "                type: 'bar',\n";
    htmlFile << "                marker: {color: '#2ecc71'}\n";
    htmlFile << "            };\n";
    htmlFile << "            \n";
    htmlFile << "            var data = [trace1, trace2];\n";
    htmlFile << "            \n";
    htmlFile << "            var layout = {\n";
    htmlFile << "                title: 'Сравнение точности алгоритмов',\n";
    htmlFile << "                xaxis: {title: 'Алгоритм'},\n";
    htmlFile << "                yaxis: {title: 'Значение', range: [0, 1]},\n";
    htmlFile << "                barmode: 'group'\n";
    htmlFile << "            };\n";
    htmlFile << "            \n";
    htmlFile << "            Plotly.newPlot('accuracyChart', data, layout);\n";
    htmlFile << "        </script>\n";
    
    htmlFile << "    </div>\n";
    htmlFile << "</body>\n";
    htmlFile << "</html>\n";
    
    htmlFile.close();
}