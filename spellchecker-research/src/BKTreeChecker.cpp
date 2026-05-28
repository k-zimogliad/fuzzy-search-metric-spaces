#include "BKTreeChecker.hpp"
#include "Distance.hpp"
#include <stack>

// Реализация узла
BKTreeChecker::BKNode::BKNode(const std::string& w) : word(w) {}

BKTreeChecker::BKNode::~BKNode() {
    for (auto& pair : children) {
        delete pair.second;
    }
}

// Реализация класса
BKTreeChecker::BKTreeChecker() : root(nullptr) {}

BKTreeChecker::~BKTreeChecker() {
    delete root;
}

void BKTreeChecker::addWord(const std::string& word) {
    if (!root) {
        root = new BKNode(word);
        return;
    }

    BKNode* curr = root;
    while (true) {
        int dist = static_cast<int>(levenshtein_distance(word, curr->word));

        if (dist == 0) return; // Слово уже есть в дереве

        if (curr->children.find(dist) != curr->children.end()) {
            curr = curr->children[dist];
        }
        else {
            curr->children[dist] = new BKNode(word);
            break;
        }
    }
}

std::vector<std::string> BKTreeChecker::search(const std::string& query, int k) {
    std::vector<std::string> results;
    if (!root) return results;

    std::stack<BKNode*> s;
    s.push(root);

    while (!s.empty()) {
        BKNode* node = s.top();
        s.pop();

        int dist = static_cast<int>(levenshtein_distance(query, node->word));

        // Если само слово в узле подходит
        if (dist <= k) {
            results.push_back(node->word);
        }

        // Неравенство треугольника
        int min_dist = dist - k;
        int max_dist = dist + k;

        auto it_start = node->children.lower_bound(min_dist);
        auto it_end = node->children.upper_bound(max_dist);

        for (auto it = it_start; it != it_end; ++it) {
            s.push(it->second);
        }
    }

    return results;
}