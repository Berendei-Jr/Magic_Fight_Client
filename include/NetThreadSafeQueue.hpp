#ifndef CLIENT_APP_NETTHREADSAFEQUEUE_HPP
#define CLIENT_APP_NETTHREADSAFEQUEUE_HPP

#include <mutex>
#include <deque>

namespace net
{
    template<class T>
    class tsqueue
    {
    public:
        tsqueue() = default;
        tsqueue(const tsqueue<T>&) = delete;

        const T& front()
        {
            std::scoped_lock lock(_mtx);
            return _data.front();
        }

        const T& back()
        {
            std::scoped_lock lock(_mtx);
            return _data.back();
        }

        void push_back(const T& item)
        {
            std::scoped_lock lock(_mtx);
            _data.push_back(std::move(item));
            _cv.notify_one();
        }

        void push_front(const T& item)
        {
            std::scoped_lock lock(_mtx);
            _data.push_front(std::move(item));
            _cv.notify_one();
        }

        size_t size()
        {
            std::scoped_lock lock(_mtx);
            return _data.size();
        }

        void clear()
        {
            std::scoped_lock lock(_mtx);
            _data.clear();
        }

        bool empty()
        {
            std::scoped_lock lock(_mtx);
            return _data.empty();
        }

        T pop_front()
        {
            std::scoped_lock lock(_mtx);
            auto tmp = std::move(_data.front());
            _data.pop_front();
            return tmp;
        }

        T pop_back()
        {
            std::scoped_lock lock(_mtx);
            auto tmp = std::move(_data.back());
            _data.pop_back();
            return tmp;
        }

        void wait()
        {
            while (empty())
            {
                std::unique_lock<std::mutex> ul(_cv_mtx);
                _cv.wait(ul);
            }
        }

        ~tsqueue() { clear(); }

    private:
        std::mutex _mtx;
        std::deque<T> _data;
        std::condition_variable _cv;
        std::mutex _cv_mtx;
    };
}

#endif //CLIENT_APP_NETTHREADSAFEQUEUE_HPP
