#ifndef __ConcurrentQueue__
#define __ConcurrentQueue__

#include<iostream>
#include <condition_variable>
#include <queue>
#include <mutex>

class ConcurrentQueue {
private:
    std::mutex mutex;
    std::queue<std::string> queue;
public:
    void push(std::string value);

    void pop();

    std::string front();

    int size();
};

#endif //__ConcurrentQueue__