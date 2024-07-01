#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class ForwardList {
private:
    class Node {
        friend class ForwardListIterator;
        friend class ForwardList;

    private:
        T data_;
        Node* next_;

        Node() : next_(nullptr) {
        }

        explicit Node(T data) : Node() {
            this->data_ = data;
        }

        Node(T data, Node* next) : Node(data) {
            this->next_ = next;
        }
    };

public:
    class ForwardListIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using IteratorCategory = std::forward_iterator_tag;

        inline bool operator==(const ForwardListIterator& other) const {
            return this->current_ == other.current_;
        };

        inline bool operator!=(const ForwardListIterator& other) const {
            return this->current_ != other.current_;
        };

        inline reference_type operator*() const {
            return this->current_->data_;
        };

        ForwardListIterator& operator++() {
            this->current_ = this->current_->next_;
            return *this;
        };

        ForwardListIterator operator++(int) {
            this->current_ = this->current_->next_;
            return ForwardListIterator(this->current_);
        };

        pointer_type operator->() const {
            if (current_ != nullptr) {
                return &(current_->data_);
            } else {
                return nullptr;
            }
        };

    private:
        friend class ForwardList<T>;
        explicit ForwardListIterator(Node* node_ptr) : current_(node_ptr) {
        }

    private:
        Node* current_;
    };

public:
    ForwardList() : head_(nullptr), size_(0) {
    }

    explicit ForwardList(size_t sz) : ForwardList() {
        if (sz != 0) {
            head_ = new Node();
            ++size_;
            Node* iter = head_;
            for (size_t i = 1; i < sz; ++i) {
                iter->next_ = new Node();
                iter = iter->next_;
                ++size_;
            }
        }
    }

    ForwardList(const std::initializer_list<T>& values) : ForwardList() {
        Node* iter = nullptr;
        for (const T& value : values) {
            if (size_ == 0) {
                head_ = new Node(value);
                iter = head_;
            } else {
                iter->next_ = new Node(value);
                iter = iter->next_;
            }
            ++size_;
        }
    }

    ForwardList(const ForwardList& other) : ForwardList() {
        if (other.size_ > 0) {
            head_ = new Node(other.head_->data_);
            Node* self_iter = head_;
            Node* other_iter = other.head_->next_;
            ++size_;
            while (other_iter != nullptr) {
                self_iter->next_ = new Node(other_iter->data_);
                self_iter = self_iter->next_;
                other_iter = other_iter->next_;
                ++size_;
            }
        }
    }

    ForwardList& operator=(const ForwardList& other) {
        if (this != &other) {
            Clear();
            if (other.size_ > 0) {
                head_ = new Node(other.head_->data_);
                Node* self_iter = head_;
                Node* other_iter = other.head_->next_;
                ++size_;
                while (other_iter != nullptr) {
                    self_iter->next_ = new Node(other_iter->data_);
                    self_iter = self_iter->next_;
                    other_iter = other_iter->next_;
                    ++size_;
                }
            }
        }
        return *this;
    }

    ForwardListIterator Begin() const noexcept {
        return ForwardListIterator(head_);
    }

    ForwardListIterator End() const noexcept {
        Node* iter = nullptr;
        if (size_ != 0) {
            iter = head_;
            while (iter != nullptr) {
                iter = iter->next_;
            }
        }
        return ForwardListIterator(iter);
    }

    inline T& Front() const {
        return head_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(ForwardList& a) {
        std::swap(head_, a.head_);
        std::swap(size_, a.size_);
    }

    void EraseAfter(ForwardListIterator pos) {
        if (size_ > 1) {
            Node* iter = head_;
            while (iter != pos.current_) {
                iter = iter->next_;
            }
            Node* cur_node = iter->next_;
            iter->next_ = cur_node->next_;
            delete cur_node;
            --size_;
        } else {
            throw ListIsEmptyException("Value doesn`t exist");
        }
    }

    void InsertAfter(ForwardListIterator pos, const T& value) {
        Node* iter = head_;
        while (iter != pos.current_) {
            iter = iter->next_;
        }
        iter->next_ = new Node(value, iter->next_);

        ++size_;
    }

    ForwardListIterator Find(const T& value) const {
        Node* iter = head_;
        while (iter->data_ != value) {
            iter = iter->next_;
        }
        return ForwardListIterator(iter);
    }

    void Clear() noexcept {
        if (size_ != 0) {
            Node* cur_node = head_->next_;
            Node* nex_node = cur_node;
            delete head_;
            while (cur_node != nullptr) {
                cur_node = cur_node->next_;
                delete nex_node;
                nex_node = cur_node;
            }
            head_ = nullptr;
            size_ = 0;
        }
    }

    void PushFront(const T& value) {
        head_ = new Node(value, head_);
        ++size_;
    }

    void PopFront() {
        if (size_ > 1) {
            Node* cur_node = head_;
            head_ = head_->next_;
            delete cur_node;
            --size_;
        } else if (size_ == 1) {
            delete head_;
            head_ = nullptr;
            --size_;
        } else {
            throw ListIsEmptyException("Value doesn`t exist");
        }
    }

    ~ForwardList() {
        Clear();
    }

private:
    Node* head_;
    size_t size_;
};

namespace std {
// Global swap overloading
template <typename T>
void Swap(ForwardList<T>& a, ForwardList<T>& b) {
    a.Swap(b);
}
}  // namespace std