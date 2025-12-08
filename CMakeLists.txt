cmake_minimum_required(VERSION 3.10)
project(DecisionTreeComparison)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/DecisionTree.cpp
    src/ID3.cpp
    src/C45.cpp
    src/CART.cpp
    src/CHAID.cpp
    src/ReportGenerator.cpp
)

# Заголовочные файлы
set(HEADERS
    include/Dataset.h
    include/DecisionTree.h
    include/ID3.h
    include/C45.h
    include/CART.h
    include/CHAID.h
    include/ReportGenerator.h
)

# Создание исполняемого файла
add_executable(DecisionTreeComparison ${SOURCES} ${HEADERS})

# Установка директорий
target_include_directories(DecisionTreeComparison PRIVATE include)

# Создание выходных директорий
add_custom_command(TARGET DecisionTreeComparison POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output/trees
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output/reports
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output/visualization
    COMMENT "Creating output directories"
)