#ifndef BKTREE_CHECKER_HPP
#define BKTREE_CHECKER_HPP

#include "ISpellChecker.hpp"
#include <map>
#include <string>
#include <vector>

class BKTreeChecker : public ISpellChecker {
private:
    struct BKNode {
        std::string word;
        std::map<int, BKNode*> children;

        BKNode(const std::string& w);
        ~BKNode(); 
    };

    BKNode* root;

public:
    BKTreeChecker();
    ~BKTreeChecker();

    void addWord(const std::string& word) override;
    std::vector<std::string> search(const std::string& query, int k) override;
};

#endif
