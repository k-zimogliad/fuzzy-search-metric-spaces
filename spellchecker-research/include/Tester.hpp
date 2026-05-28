#ifndef TESTER_HPP
#define TESTER_HPP

#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include "ISpellChecker.hpp"

// Заголовки для работы с системной памятью
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif

class Tester {
private:
    std::mt19937 gen;

    // 1. Внутреннее измерение текущего потребления RAM процессом (в байтах)
    size_t getMemoryUsage() {
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS memCounter;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter))) {
            return (size_t)memCounter.WorkingSetSize;
        }
#else
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            // ru_maxrss на Linux в килобайтах, на macOS в байтах
#ifdef __APPLE__
            return (size_t)usage.ru_maxrss;
#else
            return (size_t)(usage.ru_maxrss * 1024);
#endif
        }
#endif
        return 0;
    }

    // 2. Внутренняя генерация опечаток
    std::string introduceError(std::string word, int k) {
        if (word.empty()) return word;

        for (int i = 0; i < k; ++i) {
            int type = gen() % 3;
            int pos = gen() % word.length();

            if (type == 0) {
                word[pos] = 'a' + (gen() % 26);
            }
            else if (type == 1 && word.length() > 1) {
                word.erase(pos, 1);
            }
            else if (type == 2) {
                std::string c(1, 'a' + (gen() % 26));
                word.insert(pos, c);
            }
        }
        return word;
    }

    // 3. Предварительная генерация пула тестовых запросов, исключающая накладные расходы во время замеров
    std::vector<std::string> generateTestQueries(const std::vector<std::string>& dictionary, size_t numQueries, int k) {
        std::vector<std::string> queries;
        queries.reserve(numQueries);

        for (size_t i = 0; i < numQueries; ++i) {
            size_t idx = gen() % dictionary.size();
            queries.push_back(introduceError(dictionary[idx], k));
        }
        return queries;
    }

public:
    // Конструктор с инициализацией генератора случайных чисел
    Tester() : gen(std::random_device{}()) {}

    // 4. Высокоуровневый запуск комплексного бенчмарка (Время сборки, Память, Время поиска)
    void executeFullTest(ISpellChecker* checker,
        const std::vector<std::string>& dictionary,
        size_t numQueries,
        int k,
        const std::string& label)
    {
        std::cout << "=== Тестирование: " << label << " (k = " << k << ", слов: " << dictionary.size() << ") ===" << std::endl;

        // Фиксация базового объема RAM до построения структуры данных
        size_t memBefore = getMemoryUsage();

        // Замер времени индексации / наполнения дерева
        auto buildStart = std::chrono::high_resolution_clock::now();
        for (const auto& word : dictionary) {
            checker->addWord(word);
        }
        auto buildEnd = std::chrono::high_resolution_clock::now();
        auto buildDuration = std::chrono::duration_cast<std::chrono::milliseconds>(buildEnd - buildStart).count();

        // Расчет чистой памяти, выделенной под структуру данных
        size_t memAfter = getMemoryUsage();
        double memoryUsedMb = static_cast<double>(memAfter - memBefore) / (1024.0 * 1024.0);
        if (memoryUsedMb < 0) memoryUsedMb = 0.0; // Корректировка при незначительных системных колебаниях

        // Формирование вектора запросов с ошибками
        auto queries = generateTestQueries(dictionary, numQueries, k);

        // Стадия прогрева кэша процессора для стабилизации замеров
        size_t warmUpCount = queries.size() / 20;
        for (size_t i = 0; i < warmUpCount; ++i) {
            volatile auto trash = checker->search(queries[i], k);
            (void)trash;
        }

        // Хронометрирование фазы нечеткого поиска
        auto searchStart = std::chrono::high_resolution_clock::now();
        for (const auto& q : queries) {
            auto res = checker->search(q, k);
            // Чтение размера результата, чтобы компилятор гарантированно не вырезал вызов метода search
            volatile size_t dummy = res.size();
            (void)dummy;
        }
        auto searchEnd = std::chrono::high_resolution_clock::now();
        auto searchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEnd - searchStart).count();

        double avgLatency = static_cast<double>(searchDuration) / queries.size();

        // Вывод структурированных метрик
        std::cout << "Время построения индекса : " << buildDuration << " мс" << std::endl;
        std::cout << "Потребление памяти (RAM) : " << memoryUsedMb << " МБ" << std::endl;
        std::cout << "Среднее время на запрос  : " << avgLatency << " мкс/запрос" << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
    }
};

#endif 