#pragma once

#include <cstring>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <utility>

#include "exceptions.hpp"

const int CHUNK_SZ = 10;
const int DEFAULT_CAP = 10;

template <typename T, class Allocator = std::allocator<T*>>
class Deque {
public:
    Deque();

    Deque(const Deque& other);

    Deque(Deque&& other) noexcept;

    Deque& operator=(const Deque& other);

    Deque& operator=(Deque&& other);

    ~Deque();

public:
    void push_back(const T& value);

    void push_back(T&& value);

    template <class... Args>
    void emplace_back(Args&&... args);

    void push_front(const T& value);

    void push_front(T&& value);

    template <class... Args>
    void emplace_front(Args&&... args);

    void pop_back();

    void pop_front();

    T& back();

    T& front();

    T& operator[](size_t pos);

    void clear();

    size_t size();

    size_t capacity();

    size_t starts();

private:
    void reallocate(size_t new_cap);

private:
    Allocator aloc;
    T** chunks;
    size_t start;
    size_t sz;
    size_t cap;
};