#include "BaselineChecker.hpp"
#include "Distance.hpp" 

void BaselineChecker::addWord(const std::string& word) {
    dictionary.push_back(word);
}

std::vector<std::string> BaselineChecker::search(const std::string& query, int k) {
    std::vector<std::string> results;

    for (const auto& word : dictionary) {
        if (levenshtein_distance(query, word) <= static_cast<size_t>(k)) {
            results.push_back(word);
        }
    }

    return results;
}