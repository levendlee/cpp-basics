#include "heap.hpp"
#include "queue.hpp"

#include <assert.h>

void test_heap()
{
    Heap<int> heap(1);
    heap.push(1);
    assert(heap.top() == 1);
    heap.push(10);
    assert(heap.top() == 1);
    heap.push(100);
    assert(heap.top() == 1);
    heap.push(2);
    assert(heap.top() == 1);
    heap.pop();
    assert(heap.top() == 2);
    heap.pop();
    assert(heap.top() == 10);
    heap.pop();
    assert(heap.top() == 100);
    heap.pop();
    assert(heap.empty());
}

void test_queue()
{
    Queue<int> queue(1);
    queue.push(1);
    assert(queue.front() == 1);
    assert(queue.back() == 1);
    queue.push(10);
    assert(queue.front() == 1);
    assert(queue.back() == 10);
    queue.push(100);
    assert(queue.front() == 1);
    assert(queue.back() == 100);
    queue.push(2);
    assert(queue.front() == 1);
    assert(queue.back() == 2);
    queue.pop();
    assert(queue.front() == 10);
    assert(queue.back() == 2);
    queue.pop();
    assert(queue.front() == 100);
    assert(queue.back() == 2);
    queue.pop();
    assert(queue.front() == 2);
    assert(queue.back() == 2);
    queue.pop();
    assert(queue.empty());
}

int main()
{
    test_heap();
    test_queue();
    return 0;
}