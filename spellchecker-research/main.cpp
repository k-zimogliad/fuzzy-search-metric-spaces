#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif
#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include "Tester.hpp"
#include "BaselineChecker.hpp"
#include "TrieChecker.hpp"
#include "BKTreeChecker.hpp"

// Вспомогательная функция для чистой загрузки словаря из файла
std::vector<std::string> loadDictionary(const std::string& filename) {
    std::vector<std::string> dict;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Ошибка] Не удалось открыть файл словаря: " << filename << std::endl;
        return dict;
    }

    std::string word;
    while (file >> word) {
        if (!word.empty()) {
            dict.push_back(word);
        }
    }
    return dict;
}

int main() {

#ifdef _WIN32
    // Принудительно устанавливаем кодировку UTF-8 для ввода и вывода в консоль Windows
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif


    Tester tester;

    // 1. Пути к твоим словарям разного объема
    std::vector<std::string> dictPaths = {
        "data/small.txt",
        "data/medium.txt",
        "data/large.txt"
    };

    // Параметры тестирования в соответствии с разработанной методикой
    std::vector<int> target_k = { 1, 2, 3, 4 }; // Радиусы ошибок
    const size_t numQueries = 1000;           // Размер пула тестовых запросов

    std::cout << "=== НАЧАЛО КОМПЛЕКСНОГО ТЕСТИРОВАНИЯ ===" << std::endl << std::endl;

    // Итерируемся по каждому словарю
    for (const auto& path : dictPaths) {
        std::cout << "Загрузка словаря: " << path << "..." << std::endl;
        auto dictionary = loadDictionary(path);

        if (dictionary.empty()) {
            std::cerr << "Пропуск тестов для файла: " << path << std::endl;
            continue;
        }

        std::cout << "Успешно загружено слов: " << dictionary.size() << std::endl;
        std::cout << "==================================================" << std::endl;

        // --- ТЕСТ 1: Baseline (Линейный поиск) ---
        // Для Baseline замеряем k = 1 и k = 2, так как на k = 3,4 он будет работать слишком долго
        for (int k : {1, 2}) {
            // Используем умные указатели для автоматического контроля памяти между итерациями
            std::unique_ptr<ISpellChecker> baseline = std::make_unique<BaselineChecker>();
            tester.executeFullTest(baseline.get(), dictionary, numQueries, k, "Baseline (Linear)");
        }

        // --- ТЕСТ 2: Trie (Префиксное дерево) ---
        for (int k : target_k) {
            std::unique_ptr<ISpellChecker> trie = std::make_unique<TrieChecker>();
            tester.executeFullTest(trie.get(), dictionary, numQueries, k, "Trie Tree");
        }

        // --- ТЕСТ 3: BK-Tree (Дерево Буркхарда-Келлера) ---
        for (int k : target_k) {
            std::unique_ptr<ISpellChecker> bkTree = std::make_unique<BKTreeChecker>();
            tester.executeFullTest(bkTree.get(), dictionary, numQueries, k, "BK-Tree");
        }

        std::cout << std::endl << "=== Тесты для словаря " << path << " завершены ===" << std::endl << std::endl;
    }

    std::cout << "=== ВСЕ ТЕСТЫ ВЫПОЛНЕНЫ ===" << std::endl;
    return 0;
}