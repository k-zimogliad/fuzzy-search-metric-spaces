#ifndef ISPELL_CHECKER_HPP
#define ISPELL_CHECKER_HPP

#include <string>
#include <vector>

class ISpellChecker {
public:
    virtual ~ISpellChecker() = default;

    virtual void addWord(const std::string& word) = 0;

    virtual std::vector<std::string> search(const std::string& query, int k) = 0;
};

#endif