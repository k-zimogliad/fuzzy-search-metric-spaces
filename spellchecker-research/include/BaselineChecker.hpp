#ifndef BASELINE_CHECKER_HPP
#define BASELINE_CHECKER_HPP

#include "ISpellChecker.hpp"
#include <vector>
#include <string>

class BaselineChecker : public ISpellChecker {
private:
    // Простое хранилище для всех слов словаря
    std::vector<std::string> dictionary;

public:
    // Переопределяем методы интерфейса
    void addWord(const std::string& word) override;
    std::vector<std::string> search(const std::string& query, int k) override;
};

#endif