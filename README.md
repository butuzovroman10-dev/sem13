# Decision Tree Algorithms Comparison

## Описание проекта
Сравнение алгоритмов построения деревьев решений: ID3, C4.5, CART, CHAID
Предметная область: Выбор банка для бизнес-кредита

## Структура проекта

## Требования
- C++17 компилятор
- CMake 3.10+
- Graphviz (для визуализации)

## Сборка и запуск
```bash
# Клонирование и подготовка
mkdir build
cd build

# Сборка проекта
cmake ..
make

# Запуск программы
./DecisionTreeComparison

# Визуализация деревьев
dot -Tpng output/trees/ID3_tree.dot -o trees/ID3_tree.png