/*
 * 消息队列类模板，主要用途是：用来存储数据；
 * 截屏对象截屏后将数据存入该消息队列中，而
 * 编码对象与视频显示对象将会在相应消息队列
 * 中获取数据；而当编码对象编码成功后，也会
 * 将编码数据存储到对应消息队列中，推流对象
 * 以及生成本地文件的对象则会在对应队列中获
 * 取数据
 */

#ifndef _DATABUFFER_QUEUE_H
#define _DATABUFFER_QUEUE_H

#include <map>
#include <mutex>
#include <deque>
#include <memory>
#include <typeinfo>

struct NetworkParament;

template<class T>
class DataBufferQueue
{
public:
    DataBufferQueue() {}
    ~DataBufferQueue() {}

    void RegistCustomer(const std::string& customer);
    void PushBack(const std::shared_ptr<T>& creator);
    std::shared_ptr<T> PopTop(const std::string& customer);

    bool HaveData(const std::string& customer)
    {
        return (bool)Size(customer);
    }

private:
    std::size_t Size(const std::string& customer)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data_deque.at(customer).size();
    }

private:
    std::mutex m_mutex;
    // 用string标记消费者，string是每个模块中框架的私有变量_class_name，每个调用者都有一个队列保存对应的数据
    std::map<std::string, std::deque<std::shared_ptr<T>> > m_data_deque;
};

template<class T>
void DataBufferQueue<T>::RegistCustomer(const std::string& customer)
{
    // 消费者类初始化时先在对应消息队列注册
    // 访问互斥变量，需要上锁s
    std::lock_guard<std::mutex> lock(m_mutex);
    std::deque<std::shared_ptr<T> > deq;
    deq.clear();
    m_data_deque[customer] = deq;
}

template<class T>
void DataBufferQueue<T>::PushBack(const std::shared_ptr<T>& creator)
{
    // 给map中每一个映射关系队列中添加元素
    // 访问互斥变量，需要上锁
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_data_deque.begin();
    for (; it != m_data_deque.end(); ++it)
        (*it).second.push_back(creator);
}

template<class T>
std::shared_ptr<T> DataBufferQueue<T>::PopTop(const std::string& customer)
{
    /*
     * 先找到调用者caller属于哪个模块，找到在map中是否存储了相应的队列，
     * 查到有就从对应的队列中拿出第一个数据出来
     */

    // 访问互斥变量，需要上锁
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_data_deque.find(customer);
    if (m_data_deque.end() != it)
    {
        std::shared_ptr<T> result;
        // 当当前队列存的是网络质量参数类型的时候，则会取队列尾部元素，并清除整条队列
        if (typeid(NetworkParament) == typeid(T))
        {
            result = m_data_deque.at(customer).back();
            m_data_deque.at(customer).clear();
        }
        else
        {
            result = m_data_deque.at(customer).front();
            m_data_deque.at(customer).pop_front();
        }

        return result;
    }
}

#endif // _DATABUFFER_QUEUE_H
