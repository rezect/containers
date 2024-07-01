#include "stack.hpp"

#include <iostream>

template<typename T, class Container>
Stack<T, Container>::Stack() {
    con = Container();
}

template<typename T, class Container>
Stack<T, Container>::Stack(const Stack& other) {
    con = Container(other.con);
}

template<typename T, class Container>
Stack<T, Container>::Stack(Stack&& other) noexcept {
    con = Container(std::move(other.con));
}

template <typename T, class Container>
Stack<T, Container>& Stack<T, Container>::operator=(const Stack& other) {
    if (this != &other) {
        con.clear();
    }
    con = other.con;
    return *this;
}

template <typename T, class Container>
Stack<T, Container>& Stack<T, Container>::operator=(Stack&& other) {
    if (this != &other) {
        con.clear();
    }
    con = std::move(other.con);
    return *this;
}

template <typename T, class Container>
void Stack<T, Container>::Push(const T& value) {
    con.push_back(value);
}

template <typename T, class Container>
void Stack<T, Container>::Push(T&& value) {
    con.push_back(std::move(value));
}

template <typename T, class Container>
T& Stack<T, Container>::Pop() {
    T back_element = con.back();
    con.pop_back();
    return back_element;
}

template <typename T, class Container>
T& Stack<T, Container>::Top() {
    return con.back();
}

template <typename T, class Container>
Stack<T, Container>::~Stack() {
    con.clear();
}