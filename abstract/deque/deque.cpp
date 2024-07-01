#include "deque.hpp"

template <typename T, class Allocator>
Deque<T, Allocator>::Deque() : chunks(nullptr), start(0), sz(0), cap(0) {
}

template <typename T, class Allocator>
Deque<T, Allocator>::Deque(const Deque& other) : start(other.start), sz(other.sz), cap(other.cap) {
    chunks = aloc.allocate(other.cap);
    std::uninitialized_fill(chunks, chunks + cap, nullptr);
    for (size_t i = 0; i < cap; ++i) {
        if (other.chunks[i]) {
            chunks[i] = (T*)malloc(sizeof(T) * CHUNK_SZ);
            std::memcpy(chunks[i], other.chunks[i], sizeof(T) * CHUNK_SZ);
        }
    }
}

template <typename T, class Allocator>
Deque<T, Allocator>::Deque(Deque&& other) noexcept : start(other.start), sz(other.sz), cap(other.cap) {
    chunks = other.chunks;
    other.chunks = nullptr;
    other.start = 0;
    other.sz = 0;
    other.cap = 0;
}

template <typename T, class Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(const Deque& other) {
    if (this != &other) {
        this->clear();
        start = other.start;
        sz = other.sz;
        cap = other.cap;
        chunks = aloc.allocate(other.cap);
        std::uninitialized_fill(chunks, chunks + cap, nullptr);
        for (size_t i = 0; i < cap; ++i) {
            if (other.chunks[i]) {
                chunks[i] = (T*)malloc(sizeof(T) * CHUNK_SZ);
                std::memcpy(chunks[i], other.chunks[i], sizeof(T) * CHUNK_SZ);
            }
        }
    }
    return *this;
}

template <typename T, class Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(Deque&& other) {
    if (this != &other) {
        this->clear();
        start = other.start;
        sz = other.sz;
        cap = other.cap;
        chunks = other.chunks;
        other.chunks = nullptr;
        other.start = 0;
        other.sz = 0;
        other.cap = 0;
    }
    return *this;
}

template <typename T, class Allocator>
void Deque<T, Allocator>::push_back(const T& value) {
    this->emplace_back(value);
}

template <typename T, class Allocator>
void Deque<T, Allocator>::push_back(T&& value) {
    this->emplace_back(std::move(value));
}

template <typename T, class Allocator>
template <class... Args>
void Deque<T, Allocator>::emplace_back(Args&&... args) {
    if (chunks == nullptr) {
        this->reallocate(DEFAULT_CAP);
        chunks[cap / 2] = (T*)malloc(sizeof(T*) * CHUNK_SZ);
        new (&chunks[cap / 2][0]) T(std::forward<Args>(args)...);
        start = (cap / 2 + 1) * CHUNK_SZ - 1;
        sz = 1;
        return;
    } else if (start + sz == cap * CHUNK_SZ) {
        if (sz / CHUNK_SZ < cap / 3) {
            this->reallocate(cap);
        } else {
            this->reallocate(sz / CHUNK_SZ * 3);
        }
    }
    if ((start + sz) % CHUNK_SZ == 0) {
        chunks[(start + sz) / CHUNK_SZ] = (T*)malloc(sizeof(T*) * CHUNK_SZ);
    }
    new (&chunks[(start + sz) / CHUNK_SZ][CHUNK_SZ - ((start + sz) % CHUNK_SZ) - 1]) T(std::forward<Args>(args)...);
    ++sz;
}

template <typename T, class Allocator>
void Deque<T, Allocator>::push_front(T&& value) {
    this->emplace_front(std::move(value));
}

template <typename T, class Allocator>
void Deque<T, Allocator>::push_front(const T& value) {
    this->emplace_front(value);
}

template <typename T, class Allocator>
template <class... Args>
void Deque<T, Allocator>::emplace_front(Args&&... args) {
    if (chunks == nullptr) {
        this->reallocate(DEFAULT_CAP);
        chunks[cap / 2] = (T*)malloc(sizeof(T*) * CHUNK_SZ);
        new (&chunks[cap / 2][0]) T(std::forward<Args>(args)...);
        start = (cap / 2 + 1) * CHUNK_SZ - 1;
        sz = 1;
        return;
    } else if (start == 0) {
        if (sz / CHUNK_SZ < cap / 3) {
            this->reallocate(cap);
        } else {
            this->reallocate(sz / CHUNK_SZ * 3);
        }
    }
    if (start % CHUNK_SZ == 0) {
        chunks[(start - 1) / CHUNK_SZ] = (T*)malloc(sizeof(T*) * CHUNK_SZ);
    }
    new (&chunks[(start - 1) / CHUNK_SZ][CHUNK_SZ - ((start - 1) % CHUNK_SZ) - 1]) T(std::forward<Args>(args)...);
    --start;
    ++sz;
}

template <typename T, class Allocator>
void Deque<T, Allocator>::pop_back() {
    if (sz == 0) {
        throw DequeIsEmptyException("Ebanat?");
    }
    size_t end_index = start + sz - 1;
    ~T(chunks[end_index / CHUNK_SZ][CHUNK_SZ - (end_index % CHUNK_SZ) - 1]);
    --sz;
    if ((start + sz) % CHUNK_SZ == 0) {
        free(chunks[(start + sz) / CHUNK_SZ]);
        chunks[(start + sz) / CHUNK_SZ] = nullptr;
    }
}

template <typename T, class Allocator>
void Deque<T, Allocator>::pop_front() {
    if (sz == 0) {
        throw DequeIsEmptyException("Ebanat?");
    }
    ~T(chunks[start / CHUNK_SZ][CHUNK_SZ - (start % CHUNK_SZ) - 1]);
    ++start;
    --sz;
    if (start % CHUNK_SZ == 0) {
        free(chunks[(start - 1) / CHUNK_SZ]);
        chunks[(start - 1) / CHUNK_SZ] = nullptr;
    }
}

template <typename T, class Allocator>
T& Deque<T, Allocator>::back() {
    if (sz == 0) {
        throw DequeIsEmptyException("Ebanat?");
    }
    return chunks[(start + sz - 1) / CHUNK_SZ][CHUNK_SZ - ((start + sz - 1) % CHUNK_SZ) - 1];
}

template <typename T, class Allocator>
T& Deque<T, Allocator>::front() {
    if (sz == 0) {
        throw DequeIsEmptyException("Ebanat?");
    }
    return chunks[start / CHUNK_SZ][CHUNK_SZ - (start % CHUNK_SZ) - 1];
}

template <typename T, class Allocator>
T& Deque<T, Allocator>::operator[](size_t pos) {
    return chunks[pos / CHUNK_SZ][CHUNK_SZ - (pos % CHUNK_SZ) - 1];
}

template <typename T, class Allocator>
void Deque<T, Allocator>::clear() {
    for (size_t i = 0; i < cap; ++i) {
        if (chunks[i]) {
            for (size_t j = 0; j < CHUNK_SZ; ++j) {
                ~T(chunks[i][j]);
            }
        }
        free(chunks[i]);
    }
    aloc.deallocate(chunks, cap);
    chunks = nullptr;
    start = 0;
    sz = 0;
    cap = 0;
}

template <typename T, class Allocator>
size_t Deque<T, Allocator>::size() {
    return sz;
}

template <typename T, class Allocator>
Deque<T, Allocator>::~Deque() {
    this->clear();
}

template <typename T, class Allocator>
void Deque<T, Allocator>::reallocate(size_t new_cap) {
    size_t chunks_quantity = cap - start / CHUNK_SZ - (cap * CHUNK_SZ - (start + sz)) / CHUNK_SZ;
    T** new_chunks = aloc.allocate(new_cap);
    std::uninitialized_fill(new_chunks, new_chunks + new_cap, nullptr);
    size_t new_chunks_start = (new_cap - chunks_quantity) / 2;
    size_t amount_of_nullptrs = start / CHUNK_SZ;
    if (chunks != nullptr) {
        for (size_t i = 0; chunks[i] != nullptr && i + amount_of_nullptrs <= cap; ++i) {
            new_chunks[new_chunks_start + i] = chunks[amount_of_nullptrs + i];
        }
    }
    aloc.deallocate(chunks, cap);
    chunks = new_chunks;
    cap = new_cap;
    start += (new_chunks_start - amount_of_nullptrs) * CHUNK_SZ;
}

template <typename T, class Allocator>
size_t Deque<T, Allocator>::capacity() {
    return cap;
}

template <typename T, class Allocator>
size_t Deque<T, Allocator>::starts() {
    return start;
}