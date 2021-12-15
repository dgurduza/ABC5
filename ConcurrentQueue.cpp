#include "ConcurrentQueue.h"
#include<string>


void ConcurrentQueue::push(std::string value) {
    std::unique_lock<std::mutex> locker(mutex);
    queue.push(value);
    locker.unlock();
}

void ConcurrentQueue::pop() {
    std::unique_lock<std::mutex> locker(mutex);
    queue.pop();
    locker.unlock();
}

std::string ConcurrentQueue::front() {
    std::unique_lock<std::mutex> locker(mutex);
    std::string result = queue.front();
    locker.unlock();
    return result;
}

int ConcurrentQueue::size() {
    std::unique_lock<std::mutex> locker(mutex);
    int result = queue.size();
    locker.unlock();
    return result;
}