#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

template<typename Data>
class cqueue
{
private:
    queue<Data> fifo;
    mutable mutex my_mutex;
    condition_variable cv_in;
    condition_variable cv_out;
    size_t capacity;
public:
    cqueue() : capacity(35) {}
    cqueue(int size) : capacity(size) {}

    void push(Data const& data)
    {
        unique_lock lock(my_mutex);
        while (fifo.size()==capacity)
        {
            cv_in.wait(lock);
        }
        bool const was_empty = fifo.empty();
        fifo.push(data);
        lock.unlock(); // unlock the mutex
        if (was_empty)
        {
            cv_out.notify_one();
        }
    }
    bool empty() const
    {
        scoped_lock lock(my_mutex);
        return fifo.empty();
    }

    Data& front()
    {
        scoped_lock lock(my_mutex);
        return fifo.front();
    }

    Data const& front() const
    {
        scoped_lock lock(my_mutex);
        return fifo.front();
    }

    void pop(Data& popped_value)
    {
        unique_lock lock(my_mutex);
        while (fifo.empty())
        {
            cv_out.wait(lock);
        }
        bool const was_full = fifo.size()==capacity;

        popped_value = fifo.front();
        fifo.pop();
        lock.unlock(); // unlock the mutex
        if (was_full)
        {
            cv_in.notify_one();
        }
    }
};