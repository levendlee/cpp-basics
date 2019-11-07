// Header file of median problem
#include <stdlib.h>

#ifndef __HEAP_H__
#define __HEAP_H__

struct Less {
    template <typename T>
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};


struct Greater {
    template <typename T>
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs > rhs;
    }
};


template <typename T, typename Comp = Less>
class Heap 
{
    private:
        T* v_;
        size_t size_;
        size_t capacity_;
        Comp comparer_;

        // Allocate memorry
        inline T*   allocate(size_t num_elements) const;
        // Change the capacity of the heap. 
        inline void resize(size_t new_capacity);
        // Compare two elements
        inline bool compare(const size_t i, const size_t j) const;
        // Swap two elements
        inline void swap(const size_t i, const size_t j);

    public:
        // Construct an empty heap
        Heap(const size_t capacity = 16);
        // Decontruct the heap
        ~Heap() { free(v_); }
        // Return the capacity of the heap
        size_t capacity() const { return capacity_; }
        // Return the size of the heap
        size_t size() const { return size_; }
        // Return if the heap is empty
        bool empty() const { return size_ == 0UL; }
        // Return constant reference to the smallest element to the heap
        const T& top() const;
        // Push an element to the heap
        void push(const T& value);
        // Pop an element from the heap
        T pop();
};

template <typename T, typename Comp>
Heap<T, Comp>::Heap(const size_t capacity) : v_(allocate(capacity))
                                           , size_(0UL)
                                           , capacity_(capacity)
                                           , comparer_()
{}

template <typename T, typename Comp>
inline T* Heap<T, Comp>::allocate(size_t num_elements) const {
    return static_cast<T*>(malloc(sizeof(T) * num_elements));
}

template <typename T, typename Comp>
inline void Heap<T, Comp>::resize(size_t new_capacity) 
{
    T* new_v = allocate(new_capacity);
    memcpy(new_v, v_, sizeof(T) * size_);
    free(v_); 
    v_ = new_v;
    capacity_ = new_capacity;
}

template <typename T, typename Comp>
inline bool Heap<T, Comp>::compare(const size_t i, const size_t j) const {
    return comparer_(v_[i], v_[j]);
}

template <typename T, typename Comp>
inline void Heap<T, Comp>::swap(const size_t i, const size_t j) {
    std::swap(v_[i], v_[j]);
}

template <typename T, typename Comp>
inline const T& Heap<T, Comp>::top() const {
    if (empty()) {
        throw "Heap is empty!";
    }
    return v_[0];
}

template <typename T, typename Comp>
void Heap<T, Comp>:: push(const T& value) {
    if (size_ == capacity_) {
        resize(capacity_ * 2 + 1);
    }
    v_[size_] = value;

    size_t current = size_;
    while (current > 0UL) {
        size_t parent = (current - 1) >> 1;
        if (compare(current, parent)) {
            swap(current, parent);
            current = parent;
        } else {
            break;
        }
    }

    ++size_;
}


template <typename T, typename Comp>
T Heap<T, Comp>::pop() {
    T ret = top();  // check non empty in top();

    swap(0, size_ - 1);
    --size_;
    if (size_ == capacity_ / 2) {
        resize(capacity_ / 2 + 1);
    }

    size_t current = 0;
    while (current < size_) {
        size_t left_child = (current << 1) + 1;
        size_t right_child = (current << 1) + 2;
        size_t largest = current;
        if ((left_child < size_) && compare(left_child, largest)) {
            largest = left_child;
        }
        if ((right_child < size_) && compare(right_child, largest)) {
            largest = right_child;
        }
        if (largest == current) break;
        swap(current, largest);
        current = largest;
    }

    return ret;
}

#endif
