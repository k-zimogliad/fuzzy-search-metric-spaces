#include "TrieChecker.hpp"
#include <algorithm>

TrieChecker::TrieChecker() : root(new TrieNode()) {}

TrieChecker::~TrieChecker() {
    delete root; 
}

void TrieChecker::addWord(const std::string& word) {
    TrieNode* curr = root;
    for (char ch : word) {
        if (curr->children.find(ch) == curr->children.end()) {
            curr->children[ch] = new TrieNode();
        }
        curr = curr->children[ch];
    }
    curr->isEndOfWord = true;
}

std::vector<std::string> TrieChecker::search(const std::string& query, int k) {
    std::vector<std::string> results;
    std::string currentWord = "";

    // Первая строка матрицы Левенштейна
    std::vector<size_t> firstRow(query.size() + 1);
    for (size_t i = 0; i <= query.size(); ++i) firstRow[i] = i;

    // Запускаем поиск для каждого ребенка корня
    for (auto const& [ch, child] : root->children) {
        searchRecursive(child, ch, query, firstRow, k, currentWord, results);
    }

    return results;
}

void TrieChecker::searchRecursive(TrieNode* node, char letter, const std::string& query,
    const std::vector<size_t>& prevRow, int k,
    std::string& currentWord, std::vector<std::string>& results) {
    size_t columns = query.size() + 1;
    std::vector<size_t> currentRow(columns);
    currentRow[0] = prevRow[0] + 1;

    currentWord.push_back(letter);

    size_t minElement = currentRow[0];

    for (size_t i = 1; i < columns; ++i) {
        size_t cost = (query[i - 1] == letter) ? 0 : 1;
        currentRow[i] = std::min({
            currentRow[i - 1] + 1,        // Вставка
            prevRow[i] + 1,               // Удаление
            prevRow[i - 1] + cost         // Замена
            });
        minElement = std::min(minElement, currentRow[i]);
    }

    // Если слово закончено и расстояние в пределах k — добавляем в результат
    if (currentRow[query.size()] <= static_cast<size_t>(k) && node->isEndOfWord) {
        results.push_back(currentWord);
    }

    // Отсечение: если минимальное значение в строке > k, в этом поддереве больше нет смысла искать.
    if (minElement <= static_cast<size_t>(k)) {
        for (auto const& [ch, child] : node->children) {
            searchRecursive(child, ch, query, currentRow, k, currentWord, results);
        }
    }

    currentWord.pop_back(); // Возврат для обхода других ветвей 
}