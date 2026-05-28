#ifndef TRIE_CHECKER_HPP
#define TRIE_CHECKER_HPP

#include "ISpellChecker.hpp"
#include <map>
#include <string>
#include <vector>

class TrieChecker : public ISpellChecker {
private:
    struct TrieNode {
        std::map<char, TrieNode*> children;
        bool isEndOfWord = false;

        ~TrieNode() {
            for (auto& pair : children) {
                delete pair.second;
            }
        }
    };

    TrieNode* root;

    void searchRecursive(TrieNode* node, char letter, const std::string& query,
        const std::vector<size_t>& prevRow, int k,
        std::string& currentWord, std::vector<std::string>& results);

public:
    TrieChecker();
    ~TrieChecker();

    void addWord(const std::string& word) override;
    std::vector<std::string> search(const std::string& query, int k) override;
};

#endif
