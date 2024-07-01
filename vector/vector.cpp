#include "vector.hpp"

template <typename T, class Allocate>
Vector<T, Allocate>::Vector() : arr_(nullptr), sz_(0), cap_(0) {
}

template <typename T, class Allocale>
Vector<T, Allocale>::Vector(size_t count, const T& value) : Vector() {
    cap_ = DEFAULT_CAPACITY;
    arr_ = aloc_.allocate(cap_);
    for (size_t i = 0; i < count; ++i) {
        this->PushBack(value);
    }
}

template <typename T, class Allocate>
Vector<T, Allocate>::Vector(const Vector& other) : sz_(other.sz_), cap_(other.cap_) {
    arr_ = aloc_.allocate(other.cap_);
    std::uninitialized_copy(other.arr_, other.arr_ + other.sz_, arr_);
}

template <typename T, class Allocate>
Vector<T, Allocate>::Vector(Vector&& other) noexcept : arr_(other.arr_), sz_(other.sz_), cap_(other.cap_) {
    other.sz_ = 0;
    other.cap_ = 0;
    other.arr_ = nullptr;
}

template <typename T, class Allocate>
Vector<T, Allocate>::Vector(std::initializer_list<T> init) : arr_(nullptr), sz_(0), cap_(DEFAULT_CAPACITY) {
    arr_ = aloc_.allocate(cap_);
    for (const T& val : init) {
        this->PushBack(val);
    }
}

template <typename T, class Allocale>
Vector<T, Allocale>& Vector<T, Allocale>::operator=(const Vector& other) {
    if (this != &other) {
        this->Clear();
        aloc_.deallocate(arr_, cap_);
        sz_ = other.sz_;
        cap_ = other.cap_;
        arr_ = aloc_.allocate(cap_);
        std::uninitialized_copy(other.arr_, other.arr_ + other.sz_, arr_);
    }
    return *this;
}

template <typename T, class Allocale>
Vector<T, Allocale>& Vector<T, Allocale>::operator=(Vector&& other) {
    if (this != &other) {
        this->Clear();
        aloc_.deallocate(arr_, cap_);
        arr_ = other.arr_;
        sz_ = other.sz_;
        cap_ = other.cap_;
        other.cap_ = 0;
        other.sz_ = 0;
        other.arr_ = nullptr;
    }
    return *this;
}

template <typename T, class Allocate>
T& Vector<T, Allocate>::operator[](size_t pos) {
    return arr_[pos];
}

template <typename T, class Allocate>
T& Vector<T, Allocate>::Front() const noexcept {
    return arr_[0];
}

template <typename T, class Allocate>
bool Vector<T, Allocate>::IsEmpty() const noexcept {
    return sz_ == 0;
}

template <typename T, class Allocate>
T& Vector<T, Allocate>::Back() const noexcept {
    return arr_[sz_ - 1];
}

template <typename T, class Allocate>
T* Vector<T, Allocate>::Data() const noexcept {
    return arr_;
}

template <typename T, class Allocate>
size_t Vector<T, Allocate>::Size() const noexcept {
    return sz_;
}

template <typename T, class Allocate>
size_t Vector<T, Allocate>::Capacity() const noexcept {
    return cap_;
}

template <typename T, class Allocale>
void Vector<T, Allocale>::Reserve(size_t new_cap_) {
    if (new_cap_ <= cap_) {
        return;
    }
    T* new_arr_ = aloc_.allocate(new_cap_);
    size_t old_sz = sz_;
    for (size_t i = 0; i < sz_; ++i) {
        new (new_arr_ + i) T(std::move(arr_[i]));
    }
    this->Clear();
    aloc_.deallocate(arr_, cap_);
    sz_ = old_sz;
    arr_ = new_arr_;
    cap_ = new_cap_;
}

template <typename T, class Allocale>
void Vector<T, Allocale>::Clear() noexcept {
    for (size_t i = 0; i < sz_; ++i) {
        (arr_ + i)->~T();
    }
    sz_ = 0;
}

template <typename T, class Allocate>
void Vector<T, Allocate>::Insert(size_t pos, T value) {
    for (size_t i = pos; i < sz_; ++i) {
        std::swap(value, arr_[i]);
    }
    this->PushBack(value);
}

template <typename T, class Allocale>
void Vector<T, Allocale>::Erase(size_t begin_pos, size_t end_pos) {
    if (begin_pos >= end_pos || begin_pos >= sz_) {
        return;
    }
    T* new_arr_ = aloc_.allocate(cap_);
    size_t old_size = sz_;
    size_t j = 0;
    for (size_t i = 0; i < begin_pos; ++i) {
        new (new_arr_ + i) T(std::move(arr_[i]));
        ++j;
    }
    for (size_t i = end_pos; i < sz_; ++i) {
        new (new_arr_ + j) T(std::move(arr_[i]));
        ++j;
    }
    this->Clear();
    aloc_.deallocate(arr_, cap_);
    arr_ = new_arr_;
    sz_ = old_size - (end_pos - begin_pos);
}

template <typename T, class Allocate>
void Vector<T, Allocate>::PushBack(T value) {
    this->EmplaceBack(std::move(value));
}

template <typename T, class Allocate>
template <class... Args>
void Vector<T, Allocate>::EmplaceBack(Args&&... args) {
    if (cap_ == 0) {
        this->Reserve(DEFAULT_CAPACITY);
    } else if (cap_ == sz_) {
        this->Reserve(cap_ * 2);
    }
    new (arr_ + sz_) T(std::forward<Args>(args)...);
    ++sz_;
}

template <typename T, class Allocate>
void Vector<T, Allocate>::PopBack() {
    (arr_ + sz_ - 1)->~T();
    --sz_;
}

template <typename T, class Allocale>
void Vector<T, Allocale>::Resize(size_t count, const T& value) {
    if (count > sz_) {
        for (size_t i = sz_; i < count; ++i) {
            this->PushBack(value);
        }
    } else {
        this->Erase(count, sz_);
    }
}

template <typename T, class Allocale>
Vector<T, Allocale>::~Vector() {
    this->Clear();
    aloc_.deallocate(arr_, cap_);
    cap_ = 0;
}

template <class Allocate>
Vector<void*, Allocate>::Vector() : arr_(nullptr), sz_(0), cap_(0) {
}

template <class Allocate>
void* Vector<void*, Allocate>::Front() const noexcept {
    return arr_[0];
}

template <class Allocate>
void* Vector<void*, Allocate>::Back() const noexcept {
    return arr_[sz_ - 1];
}

template <class Allocate>
void Vector<void*, Allocate>::Reserve(size_t new_cap_) {
    if (new_cap_ <= cap_) {
        return;
    }
    void** new_arr_ = static_cast<void**>(aloc_.allocate(new_cap_));
    for (size_t i = 0; i < sz_; ++i) {
        new_arr_[i] = arr_[i];
    }
    aloc_.deallocate(arr_, cap_);
    arr_ = new_arr_;
    cap_ = new_cap_;
}

template <class Allocate>
void Vector<void*, Allocate>::PushBack(void* value) {
    if (cap_ == 0) {
        this->Reserve(DEFAULT_CAPACITY);
    } else if (cap_ == sz_) {
        this->Reserve(cap_ * 2);
    }
    arr_[sz_] = value;
    ++sz_;
}

template <class Allocate>
Vector<void*, Allocate>::~Vector() {
    for (size_t i = 0; i < sz_; ++i) {
        free(arr_[i]);
    }
    aloc_.deallocate(arr_, cap_);
}