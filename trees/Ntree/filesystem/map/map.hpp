#pragma once

#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

#include "exceptions.hpp"

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {

    class Node;

public:
    class MapIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = std::pair<const Key, Value>;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::forward_iterator_tag;

        inline bool operator==(const MapIterator& other) const {
            return current_ == other.current_;
        };

        inline bool operator!=(const MapIterator& other) const {
            return current_ != other.current_;
        };

        inline reference_type operator*() const {
            return current_->data_;
        };

        MapIterator& operator++() {
            if (current_->is_right_thread_) {
                current_ = current_->right_;
                return *this;
            } else {
                current_ = current_->right_;
                while (current_->left_) {
                    current_ = current_->left_;
                }
                return *this;
            }
        };

        MapIterator operator++(int) {
            MapIterator tmp(this->current_);
            current_ = current_->right_;
            return tmp;
        };

        inline pointer_type operator->() const {
            return &current_->data_;
        };

    private:
        explicit MapIterator(Node* current) {
            current_ = current;
        }

    private:
        Node* current_;
        friend class Map;
    };

    inline MapIterator Begin() const noexcept {
        Node* iter = root_;
        while (iter->left_) {
            iter = iter->left_;
        }
        return MapIterator(iter);
    }

    inline MapIterator End() const noexcept {
        return MapIterator(root_);
    }

    Map() : comp_(), root_(new Node()), size_(0) {
        root_->right_ = root_;
        root_->is_right_thread_ = true;
    }

    explicit Map(const Compare& comp) : comp_(comp), root_(new Node()), size_(0) {
        root_->right_ = root_;
        root_->is_right_thread_ = true;
    }

    Value& operator[](const Key& key) {
        Node* parent = FindParent(key);
        if (parent->right_ == parent) {
            if (parent->left_) {
                return parent->left_->data_.second;
            } else {
                parent->left_ = new Node(std::make_pair(key, Value()));
                ++size_;
                return parent->left_->data_.second;
            }
        } else if (comp_(key, parent->data_.first)) {
            if (parent->left_) {
                return parent->left_->data_.second;
            } else {
                parent->left_ = new Node(std::make_pair(key, Value()));
                ++size_;
                return parent->left_->data_.second;
            }
        } else {
            if (parent->right_) {
                return parent->right_->data_.second;
            } else {
                parent->right_ = new Node(std::make_pair(key, Value()));
                ++size_;
                return parent->right_->data_.second;
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
        if (size_ == 0) {
            return res;
        } else if (is_increase) {
            IncreasingValues(root_->left_, res);
        } else {
            DecreasingValues(root_->left_, res);
        }
        return res;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        Node* parent = FindParent(val.first);
        if (parent->right_ == parent) {
            if (parent->left_) {
                parent->left_->data_.second = val.second;
            } else {
                Node* new_node = new Node(val);
                parent->left_ = new_node;
                new_node->is_right_thread_ = true;
                new_node->right_ = parent;
                ++size_;
            }
        } else if (comp_(val.first, parent->data_.first)) {
            if (parent->left_) {
                parent->left_->data_.second = val.second;
            } else {
                Node* new_node = new Node(val);
                new_node->right_ = parent;
                new_node->is_right_thread_ = true;
                parent->left_ = new_node;
                ++size_;
            }
        } else {
            if (!parent->is_right_thread_) {
                parent->right_->data_.second = val.second;
            } else {
                Node* new_node = new Node(val);
                new_node->right_ = parent->right_;
                new_node->is_right_thread_ = true;
                parent->right_ = new_node;
                parent->is_right_thread_ = false;
                ++size_;
            }
        }
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (std::pair<const Key, Value> val : values) {
            Insert(val);
        }
    }

    void Erase(const Key& key) {
        Node* parent = FindParent(key);
        if (parent == root_) {
            DeleteNode(parent, parent->left_, false);
        } else if (comp_(key, parent->data_.first)) {
            if (parent->left_) {
                DeleteNode(parent, parent->left_, false);
            } else {
                throw KeyIsMissingInMap("No such key");
            }
        } else {
            if (!parent->is_right_thread_) {
                DeleteNode(parent, parent->right_, true);
            } else {
                throw KeyIsMissingInMap("No such key");
            }
        }
        --size_;
    }

    void Clear() noexcept {
        RecursionClear(root_->left_);
        root_->left_ = nullptr;
        size_ = 0;
    }

    MapIterator Find(const Key& key) const {
        Node* parent = FindParent(key);
        if (parent->right_ == parent) {
            if (parent->left_ == nullptr) {
                return MapIterator(root_);
            } else if (parent->left_->data_.first == key) {
                return MapIterator(parent->left_);
            } else {
                throw KeyIsMissingInMap("No such key");
            }
        } else if (comp_(key, parent->data_.first)) {
            if (parent->left_) {
                return MapIterator(parent->left_);
            } else {
                return MapIterator(root_);
            }
        } else {
            if (!parent->is_right_thread_) {
                return MapIterator(parent->right_);
            } else {
                return MapIterator(root_);
            }
        }
    }

    ~Map() {
        this->Clear();
        delete root_;
    }

private:
    class Node {
        friend class MapIterator;
        friend class Map;

    private:
        std::pair<const Key, Value> data_;
        Node* right_;
        Node* left_;
        bool is_right_thread_;

        Node() : data_(), right_(nullptr), left_(nullptr), is_right_thread_(false) {
        }

        explicit Node(std::pair<const Key, Value> data)
            : data_(data), right_(nullptr), left_(nullptr), is_right_thread_(false) {
        }
    };

private:
    Compare comp_;
    Node* root_;
    size_t size_;

private:
    Node* FindParent(const Key& key) const {
        Node* parent = root_;
        Node* current = parent->left_;
        Node* iter = nullptr;
        while (current) {
            iter = current;
            if (comp_(key, current->data_.first)) {
                current = current->left_;
            } else if (comp_(current->data_.first, key)) {
                if (!current->is_right_thread_) {
                    current = current->right_;
                } else {
                    return current;
                }
            } else {
                return parent;
            }
            parent = iter;
        }
        return parent;
    }

    void DeleteNode(Node* parent, Node* current, bool is_right) {
        if (current->left_ == nullptr && current->is_right_thread_) {
            if (is_right) {
                parent->right_ = current->right_;
                parent->is_right_thread_ = true;
                delete current;
            } else {
                delete current;
                parent->left_ = nullptr;
            }
        } else if (current->left_ && !current->is_right_thread_) {
            if (current->left_->is_right_thread_) {
                current->left_->is_right_thread_ = false;
                current->left_->right_ = current->right_;
                if (is_right) {
                    parent->right_ = current->left_;
                } else {
                    parent->left_ = current->left_;
                }
                delete current;
            } else {
                Node* iter = current->left_->right_;
                Node* parent_iter = current->left_;
                while (!iter->is_right_thread_) {
                    parent_iter = iter;
                    iter = iter->right_;
                }
                if (iter->left_) {
                    parent_iter->right_ = iter->left_;
                }
                iter->left_ = current->left_;
                iter->right_ = current->right_;
                iter->is_right_thread_ = false;
                parent_iter->is_right_thread_ = true;
                if (is_right) {
                    parent->right_ = iter;
                } else {
                    parent->left_ = iter;
                }
                delete current;
            }
        } else {
            if (current->left_) {
                Node* iter = current->left_;
                while (!iter->is_right_thread_) {
                    iter = iter->right_;
                }
                iter->right_ = current->right_;
                if (is_right) {
                    parent->right_ = current->left_;
                } else {
                    parent->left_ = current->left_;
                }
                delete current;
            } else {
                if (is_right) {
                    parent->right_ = current->right_;
                } else {
                    parent->left_ = current->right_;
                }
                delete current;
            }
        }
    }

    void RecursionClear(Node* iter) {
        if (iter && iter != root_) {
            if (iter->is_right_thread_) {
                RecursionClear(iter->left_);
            } else {
                RecursionClear(iter->left_);
                RecursionClear(iter->right_);
            }
            delete iter;
        }
    }

    void IncreasingValues(Node* iter, std::vector<std::pair<const Key, Value>>& vec) const noexcept {
        if (iter->left_) {
            this->IncreasingValues(iter->left_, vec);
            vec.push_back(std::make_pair(iter->data_.first, iter->data_.second));
            if (!iter->is_right_thread_) {
                this->IncreasingValues(iter->right_, vec);
            }
        } else if (!iter->is_right_thread_) {
            vec.push_back(std::make_pair(iter->data_.first, iter->data_.second));
            this->IncreasingValues(iter->right_, vec);
        } else {
            vec.push_back(std::make_pair(iter->data_.first, iter->data_.second));
        }
    }

    void DecreasingValues(Node* iter, std::vector<std::pair<const Key, Value>>& vec) const noexcept {
        if (!iter->is_right_thread_) {
            this->DecreasingValues(iter->right_, vec);
            vec.push_back(std::make_pair(iter->data_.first, iter->data_.second));
            if (iter->left_) {
                this->DecreasingValues(iter->left_, vec);
            }
        } else if (iter->left_) {
            vec.push_back(std::make_pair(iter->data_.first, iter->data_.second));
            this->DecreasingValues(iter->left_, vec);
        } else {
            vec.push_back(std::make_pair(iter->data_.first, iter->data_.second));
        }
    }
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std