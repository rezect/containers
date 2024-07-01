#include <stdio.h>

#include <deque>

template <typename T, class Container = std::deque<T>>
class Stack {
public:
    Stack();

    Stack(const Stack& other);

    Stack(Stack&& other) noexcept;

    Stack& operator=(const Stack& other);

    Stack& operator=(Stack&& other);

    ~Stack();

public:
    void Push(const T& value);

    void Push(T&& value);

    T& Pop();

    T& Top();

private:
    Container con;
};