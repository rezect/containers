#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class List {
private:
    class Node {
        friend class ListIterator;
        friend class List;

    private:
        T data_;
        Node* next_ = nullptr;
        Node* prev_ = nullptr;

    private:
        Node() = default;
        explicit Node(const T& data) : data_(data){};
    };

public:
    class ListIterator {
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
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator& other) const {
            return this->current_ == other.current_;
        };
        inline bool operator!=(const ListIterator& other) const {
            return this->current_ != other.current_;
        };

        inline reference_type operator*() const {
            return this->current_->data_;
        };

        ListIterator& operator++() {
            this->current_ = this->current_->next_;
            return *this;
        };

        ListIterator operator++(int) {
            ListIterator new_iter(this->current_);
            this->current_ = this->current_->next_;
            return new_iter;
        };

        ListIterator& operator--() {
            this->current_ = this->current_->prev_;
            return *this;
        };

        ListIterator operator--(int) {
            ListIterator new_iter(this->current_);
            this->current_ = this->current_->prev_;
            return new_iter;
        };

        inline pointer_type operator->() const {
            if (current_ != nullptr) {
                return &(current_->data_);
            } else {
                return nullptr;
            }
        };

    private:
        friend class List<T>;
        explicit ListIterator(Node* node_ptr) : current_(node_ptr) {
        }

    private:
        Node* current_;
    };

public:
    List() : head_(nullptr), tail_(nullptr), size_(0) {
    }

    explicit List(size_t sz) : List() {
        if (sz != 0) {
            Node* begin_node = new Node();
            Node* end_node = new Node();
            head_ = begin_node;
            tail_ = end_node;
            head_->next_ = tail_;
            tail_->prev_ = head_;
            ++size_;
            for (size_t i = 1; i < sz; ++i) {
                Node* new_node = new Node();
                tail_->next_ = new_node;
                new_node->prev_ = tail_;
                tail_ = new_node;
                ++size_;
            }
        }
    }

    List(const std::initializer_list<T>& values) : List() {
        for (const auto& value : values) {
            PushBack(value);
        }
    }

    List(const List& other) : List() {
        Node* iter = other.head_;
        while (iter != other.tail_) {
            this->PushBack(iter->data_);
            iter = iter->next_;
        }
    }

    List& operator=(const List& other) {
        if (this == &other) {
            return *this;
        }
        this->Clear();
        Node* iter = other.head_;
        while (iter != other.tail_) {
            PushBack(iter->data_);
            iter = iter->next_;
        }
        return *this;
    }

    ListIterator Begin() const noexcept {
        return ListIterator(head_);
    }

    ListIterator End() const noexcept {
        return ListIterator(tail_);
    }

    inline T& Front() const {
        return head_->data_;
    }

    inline T& Back() const {
        return tail_->prev_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(List& other) {
        std::swap(this->head_, other.head_);
        std::swap(this->tail_, other.tail_);
        std::swap(this->size_, other.size_);
    }

    ListIterator Find(const T& value) const {
        Node* iter = head_;
        while (iter != tail_) {
            if (iter->data_ == value) {
                break;
            }
            iter = iter->next_;
        }
        return ListIterator(iter);
    }

    void Erase(ListIterator pos) {
        if (head_ == nullptr) {
            throw ListIsEmptyException("List is empty");
        } else if (pos.current_ != head_) {
            pos.current_->prev_->next_ = pos.current_->next_;
            pos.current_->next_->prev_ = pos.current_->prev_;
            delete pos.current_;
        } else {
            head_ = head_->next_;
            delete head_->prev_;
            head_->prev_ = nullptr;
        }
        --size_;
    }

    void Insert(ListIterator pos, const T& value) {
        Node* new_node = new Node(value);
        new_node->next_ = pos.current_;
        new_node->prev_ = pos.current_->prev_;
        new_node->prev_->next_ = new_node;
        new_node->next_->prev_ = new_node;
        ++size_;
    }

    void Clear() noexcept {
        while (head_ != tail_) {
            head_ = head_->next_;
            delete head_->prev_;
        }
        if (tail_ != nullptr) {
            delete tail_;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    void PushBack(const T& value) {
        if (head_ == nullptr) {
            head_ = new Node(value);
            tail_ = new Node();
            head_->next_ = tail_;
            tail_->prev_ = head_;
            ++size_;
        } else {
            Node* new_node = new Node();
            tail_->data_ = value;
            tail_->next_ = new_node;
            new_node->prev_ = tail_;
            tail_ = new_node;
            ++size_;
        }
    }

    void PushFront(const T& value) {
        Node* new_node = new Node(value);
        new_node->next_ = head_;
        head_->prev_ = new_node;
        head_ = new_node;
        ++size_;
    }

    void PopBack() {
        if (head_ == nullptr) {
            throw ListIsEmptyException("List is empty");
        }
        if (size_ == 1) {
            delete head_;
            delete tail_;
            head_ = nullptr;
            tail_ = nullptr;
        } else {
            tail_->prev_ = tail_->prev_->prev_;
            delete tail_->prev_->next_;
            tail_->prev_->next_ = tail_;
        }
        --size_;
    }

    void PopFront() {
        if (head_ == nullptr) {
            throw ListIsEmptyException("List is empty");
        }
        if (size_ == 1) {
            delete head_;
            delete tail_;
            head_ = nullptr;
            tail_ = nullptr;
        } else {
            head_ = head_->next_;
            delete head_->prev_;
            head_->prev_ = nullptr;
        }
        --size_;
    }

    ~List() {
        Clear();
    }

private:
    Node* head_;
    Node* tail_;
    size_t size_;
};

namespace std {
// Global swap overloading
template <typename T>
// NOLINTNEXTLINE
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std