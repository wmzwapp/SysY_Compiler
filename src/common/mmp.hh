#pragma once

#include <cstddef>
#include <vector>

class MArena {
    struct DestructorEntry {
        void (*dtor)(void*);
        void* obj;
    };


public:
    MArena(std::size_t blockSize = 4096) : blockSize_(blockSize) {
        allocate(blockSize_);
    }

    ~MArena() {
        for (auto it = dtorStack_.rbegin(); it != dtorStack_.rend(); ++it) {
            it->dtor(it->obj);
        }
        for (auto ptr : blocks_) {
            ::operator delete(ptr);
        }
    }

    template<typename T, typename... Args>
    T* make(Args&&... args) {
        void* mem = allocate(sizeof(T));
        T* obj = new (mem) T(std::forward<Args>(args)...);

        if constexpr (!std::is_trivially_destructible_v<T>) {
            dtorStack_.push_back({
                [](void*ptr) { static_cast<T*>(ptr)->~T(); },
                obj
            });
        }

        return obj;
    }

private:
    void* allocate(std::size_t size, std::size_t alignment = 8) {
        size = (size + alignment - 1) & ~(alignment - 1);
        if (curPtr_ + size > endPtr_) {
            std::size_t allocSize = std::max(size, blockSize_);
            curPtr_ = static_cast<char*>(::operator new(allocSize));
            endPtr_ = curPtr_ + allocSize;
            blocks_.push_back(curPtr_);
        }
        void* result = curPtr_;
        curPtr_ += size;
        return result;
    }

private:
    std::size_t blockSize_;
    char* curPtr_   { nullptr};
    char* endPtr_   { nullptr};
    std::vector<void*> blocks_;
    std::vector<DestructorEntry> dtorStack_;
};

inline MArena mmpool_;