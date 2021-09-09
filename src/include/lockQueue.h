#pragma once
#include<mutex>
#include<condition_variable>
#include<queue>


template<class T>
class lockQueue{
public:
    //写日志
    void Push(T data){
        std::lock_guard<std::mutex> locker(mtx);
        dataQueue.push(data);
        cond.notify_one();
    }

    //取日志
    T Pop(){
        std::lock_guard<std::mutex> locker(mtx);
        //等待日志队列非空
        while(dataQueue.empty()){
            cond.wait();
        }

        T data = dataQueue.front();
        dataQueue.pop();

        return data;
    }
private:
    std::mutex mtx;
    std::condition_variable cond;
    std::queue<T> dataQueue;

};