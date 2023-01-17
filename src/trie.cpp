#include "trie.h"

#include "extended_char.h"
#include "priority_queue.h"

#include <queue>

#include <algorithm>
#include <functional>
#include <tuple>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

Trie::Node::Node(ExtendedChar c, uint64_t cnt) : cnt(cnt), data(c), is_terminated(true) {
}

Trie::Node::Node(Node* left, Node* right)
    : left(left),
      right(right),
      cnt(left->cnt + right->cnt),
      data(std::min(left->data, right->data)),
      is_terminated(false) {
}

void Trie::Node::Dfs(std::vector<std::pair<ExtendedChar, std::vector<bool>>>& codes,
                     std::vector<bool>& curr_code) const {
    if (!is_terminated) {
        if (left) {
            curr_code.push_back(false);
            left->Dfs(codes, curr_code);
            curr_code.pop_back();
        }
        if (right) {
            curr_code.push_back(true);
            right->Dfs(codes, curr_code);
            curr_code.pop_back();
        }
    } else {
        codes.push_back({data, curr_code});
    }
}

Trie::Node::~Node() {
    if (left) {
        delete left;
    }
    if (right) {
        delete right;
    }
}

Trie::Trie(const std::unordered_map<ExtendedChar, uint64_t>& cnt) : root_(nullptr) {
    std::function<bool(const Node* const, const Node* const)> cmp = [](const Node* const left,
                                                                       const Node* const right) {
        return std::tie(left->cnt, left->data) > std::tie(right->cnt, right->data);
    };
    std::priority_queue<Node*, std::vector<Node*>, std::function<bool(const Node* const, const Node* const)>> q(cmp);

    for (const auto& curr : cnt) {
        q.push(new Node(curr.first, curr.second));
    }

    while (q.size() > 1) {
        auto curr1 = q.top();
        q.pop();
        auto curr2 = q.top();
        q.pop();

        q.push(new Node(curr1, curr2));
    }

    root_ = q.top();
}

std::vector<std::pair<ExtendedChar, std::vector<bool>>> Trie::AllCodes() const {
    std::vector<std::pair<ExtendedChar, std::vector<bool>>> result;
    if (root_) {
        std::vector<bool> curr_code;
        root_->Dfs(result, curr_code);
    }
    return result;
}

Trie::~Trie() {
    if (root_) {
        delete root_;
    }
}
