#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

#include "exceptions.hpp"

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
public:
    Map() : comp_(), root_(nullptr), size_(0) {
    }

    Value& operator[](const Key& key) {
        if (root_ == nullptr) {
            root_ = new Node(key);
            ++size_;
            return root_->value_;
        } else if (IsEqual(key, root_->key_)) {
            return root_->value_;
        } else {
            Node* parent = FindParent(root_, key);
            if (comp_(key, parent->key_)) {
                if (parent->left_) {
                    return parent->left_->value_;
                } else {
                    parent->left_ = new Node(key);
                    ++size_;
                    return parent->left_->value_;
                }
            } else {
                if (parent->right_) {
                    return parent->right_->value_;
                } else {
                    parent->right_ = new Node(key);
                    ++size_;
                    return parent->right_->value_;
                }
            }
        }
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(Map& a) {
        static_assert(std::is_same<decltype(this->comp_), decltype(a.comp_)>::value,
                      "The compare function types are different");
        std::swap(root_, a.root_);
        std::swap(size_, a.size_);
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> res;
        if (root_ == nullptr) {
            return res;
        }
        if (is_increase) {
            IncreasingValues(root_, res);
        } else {
            DecreasingValues(root_, res);
        }
        return res;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        if (root_ == nullptr) {
            root_ = new Node(val);
        } else if (IsEqual(root_->key_, val.first)) {
            root_->value_ = val.second;
        } else {
            Node* parent = FindParent(root_, val.first);
            if (comp_(val.first, parent->key_)) {
                if (parent->left_) {
                    parent->left_->value_ = val.second;
                } else {
                    parent->left_ = new Node(val);
                }
            } else {
                if (parent->right_) {
                    parent->right_->value_ = val.second;
                } else {
                    parent->right_ = new Node(val);
                }
            }
        }
        ++size_;
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (std::pair<const Key, Value> val : values) {
            Insert(val);
        }
    }

    void Erase(const Key& key) {
        if (root_ == nullptr) {
            throw KeyIsMissingInMap("Value not found");
        } else if (IsEqual(key, root_->key_)) {
            DeleteRoot();
        } else {
            Node* parent = FindParent(root_, key);
            if (comp_(key, parent->key_)) {
                if (parent->left_) {
                    DeleteNode(parent, parent->left_, false);
                } else {
                    throw KeyIsMissingInMap("Value not found");
                }
            } else {
                if (parent->right_) {
                    DeleteNode(parent, parent->right_, true);
                } else {
                    throw KeyIsMissingInMap("Value not found");
                }
            }
        }
        --size_;
    }

    void Clear() noexcept {
        ClearRecursion(root_);
        root_ = nullptr;
        size_ = 0;
    }

    bool Find(const Key& key) const {
        if (root_ == nullptr) {
            return false;
        } else if (IsEqual(key, root_->key_)) {
            return true;
        } else {
            Node* parent = FindParent(root_, key);
            if (comp_(key, parent->key_)) {
                if (parent->left_) {
                    return true;
                } else {
                    return false;
                }
            } else {
                if (parent->right_) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    ~Map() {
        Clear();
    }

private:
    class Node {
        friend class Map;

    private:
        Key key_;
        Value value_;
        Node* left_;
        Node* right_;

    private:
        Node() : left_(nullptr), right_(nullptr){};

        explicit Node(std::pair<const Key, Value> data) : Node() {
            key_ = data.first;
            value_ = data.second;
        }

        explicit Node(Key key, Value value = 0) : Node() {
            this->value_ = value;
            this->key_ = key;
        }
    };

private:
    void ClearRecursion(Node* iter) {
        if (iter) {
            ClearRecursion(iter->left_);
            ClearRecursion(iter->right_);
            delete iter;
        }
    }

    // На входе структура начальная нода и искомый ключ, на выходе - родитель искомой ноды (даже если она не существует)
    Node* FindParent(Node* start_point, Key key) const {
        Node* parent = start_point;
        Node* current = nullptr;
        if (comp_(key, parent->key_)) {
            current = parent->left_;
        } else {
            current = parent->right_;
        }
        while (current) {
            if (IsEqual(key, current->key_)) {
                return parent;
            }
            parent = current;
            if (comp_(key, current->key_)) {
                current = current->left_;
            } else if (comp_(current->key_, key)) {
                current = current->right_;
            }
        }
        return parent;
    }

    void DeleteNode(Node* parent, Node* current, bool is_right) {
        if (current->left_ == nullptr && current->right_ == nullptr) {
            delete current;
            if (is_right) {
                parent->right_ = nullptr;
            } else {
                parent->left_ = nullptr;
            }
        } else if (current->left_ && current->right_) {
            TwoSons(parent, current, current->left_, is_right);
        } else {
            if (is_right) {
                if (current->left_) {
                    parent->right_ = current->left_;
                } else {
                    parent->right_ = current->right_;
                }
            } else {
                if (current->left_) {
                    parent->left_ = current->left_;
                } else {
                    parent->left_ = current->right_;
                }
            }
            delete current;
        }
    }

    void DeleteRoot() {
        // 0 childs
        if (root_->left_ == nullptr && root_->right_ == nullptr) {
            delete root_;
            root_ = nullptr;
        } else if (root_->left_ && root_->right_) {  // Если 2 ребенка
            TwoSonsRoot(root_->left_);
        } else {
            if (root_->left_) {
                Node* current = root_;
                root_ = root_->left_;
                delete current;
            } else if (root_->right_) {
                Node* current = root_;
                root_ = root_->right_;
                delete current;
            }
        }
    }

    void TwoSonsRoot(Node* head) {
        Node* stop_node = head->right_;
        Node* head_parent = head;
        if (head->right_) {
            head = head->right_;
            stop_node = head->right_;
            while (stop_node) {
                stop_node = stop_node->right_;
                head = head->right_;
                head_parent = head_parent->right_;
            }
            head_parent->right_ = head->left_;
            head->right_ = root_->right_;
            head->left_ = root_->left_;
            delete root_;
            root_ = head;
        } else {
            head->right_ = root_->right_;
            delete root_;
            root_ = head;
        }
    }

    void TwoSons(Node* parent, Node* current, Node* head, bool is_right) {
        Node* stop_node = head->right_;
        Node* head_parent = head;
        if (head->right_) {
            head = head->right_;
            stop_node = head->right_;
            while (stop_node) {
                stop_node = stop_node->right_;
                head = head->right_;
                head_parent = head_parent->right_;
            }
            head_parent->right_ = head->left_;
            head->right_ = current->right_;
            head->left_ = current->left_;
            delete current;
            if (is_right) {
                parent->right_ = head;
            } else {
                parent->left_ = head;
            }
        } else {
            head->right_ = current->right_;
            if (is_right) {
                parent->right_ = head;
            } else {
                parent->left_ = head;
            }
            delete current;
        }
    }

    void IncreasingValues(Node* iter, std::vector<std::pair<const Key, Value>>& vec) const noexcept {
        if (iter->left_) {
            this->IncreasingValues(iter->left_, vec);
            vec.push_back(std::make_pair(iter->key_, iter->value_));
            if (iter->right_) {
                this->IncreasingValues(iter->right_, vec);
            }
        } else if (iter->right_) {
            vec.push_back(std::make_pair(iter->key_, iter->value_));
            this->IncreasingValues(iter->right_, vec);
        } else {
            vec.push_back(std::make_pair(iter->key_, iter->value_));
        }
    }

    void DecreasingValues(Node* iter, std::vector<std::pair<const Key, Value>>& vec) const noexcept {
        if (iter->right_) {
            this->DecreasingValues(iter->right_, vec);
            vec.push_back(std::make_pair(iter->key_, iter->value_));
            if (iter->left_) {
                this->DecreasingValues(iter->left_, vec);
            }
        } else if (iter->left_) {
            vec.push_back(std::make_pair(iter->key_, iter->value_));
            this->DecreasingValues(iter->left_, vec);
        } else {
            vec.push_back(std::make_pair(iter->key_, iter->value_));
        }
    }

    bool IsEqual(Key a, Key b) const {
        return !comp_(a, b) && !comp_(b, a);
    }

private:
    Compare comp_;
    Node* root_;
    size_t size_;
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std