#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>

template <typename T>
class Queue
{
private:
    T *v_;
    size_t head_;
    size_t tail_;
    size_t capacity_;
    size_t size_;

    // Resizes to new capacity.
    void resize(size_t capacity);

public:
    // Constructs an empty queue.
    Queue(size_t capacity = 16)
        : head_(0), tail_(0), capacity_(capacity), size_(0)
    {
        v_ = new T[capacity];
    }
    // Deconstructs the queue.
    ~Queue() { delete v_; }

    // Pushes an element to the back of the queue.
    void push(const T &elem);
    void push(T &&elem);
    // Pops an element from the front of the queue.
    T pop();

    // Returns the front of the queue.
    const T &front() const { return v_[head_]; };
    // Returns the back of the queue.
    const T &back() const { return v_[tail_]; };

    // Returns the size of the queue.
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
};

template <typename T>
void Queue<T>::push(const T &elem)
{
    if (size_ == capacity_)
    {
        resize(capacity_ * 2);
    }
    tail_ = ++tail_ % capacity_;
    v_[tail_] = elem;
    ++size_;
}

template <typename T>
void Queue<T>::push(T &&elem)
{
    if (size_ == capacity_)
    {
        resize(capacity_ * 2);
    }
    tail_ = ++tail_ % capacity_;
    v_[tail_] = elem;
    ++size_;
}

template <typename T>
T Queue<T>::pop()
{
    T elem(std::move(v_[head_]));
    head_ = ++head_ % capacity_;
    --size_;
    return elem;
}

template <typename T>
void Queue<T>::resize(size_t new_capacity)
{
    T *nv_ = new T[new_capacity];
    for (size_t i = 0; i < size_; ++i)
    {
        size_t index = (head_ + i) % capacity_;
        nv_[i] = std::move(v_[index]);
    }
    delete v_;
    v_ = nv_;
    head_ = 0;
    tail_ = size_ - 1;
    capacity_ = new_capacity;
}

#endif