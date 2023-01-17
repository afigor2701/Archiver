#pragma once

#include "extended_char.h"

#include <unordered_map>
#include <vector>
#include <utility>

class Trie {
private:
    struct Node {
        Node* left = nullptr;
        Node* right = nullptr;
        uint64_t cnt = 0;
        ExtendedChar data;
        bool is_terminated = false;

        Node(ExtendedChar c, uint64_t cnt);
        Node(Node* left, Node* right);

        void Dfs(std::vector<std::pair<ExtendedChar, std::vector<bool>>>& codes, std::vector<bool>& curr_code) const;

        ~Node();
    };

    Node* root_;

public:
    explicit Trie(const std::unordered_map<ExtendedChar, uint64_t>& cnt);

    std::vector<std::pair<ExtendedChar, std::vector<bool>>> AllCodes() const;

    ~Trie();
};
