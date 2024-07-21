#include <iostream>
#include <mutex>
#include <memory>
#include <atomic>
#include <string>
#include <atomic>
#include <stack>
namespace lock_queue {
    template<class T>
    class Circular_queue {
    public:
        Circular_queue(size_t max_size)
            : m_max_size(max_size), m_data(nullptr), m_head(0), m_tail(0) {
            if (m_max_size > 0) {
                m_data = std::allocator_traits<std::allocator<T>>::allocate(m_allocator, m_max_size);
            }
            std::cout << "sucessful run in the construct" << std::endl;
        }

        Circular_queue(const Circular_queue& other) = delete;
        Circular_queue& operator=(const Circular_queue& other) = delete;

        ~Circular_queue() {
            std::lock_guard<std::mutex> lockguard(m_mutex);
            while (m_head != m_tail) {
                std::allocator_traits<std::allocator<T>>::destroy(m_allocator, m_data + m_head);
                m_head = (m_head + 1) % m_max_size;
            }
            if (m_data) {
                std::allocator_traits<std::allocator<T>>::deallocate(m_allocator, m_data, m_max_size);
            }
            std::cout << "clear sucessfully" << std::endl;
        }

        template<typename ...Args>
        bool emplace(Args&& ... args) {
            std::lock_guard<std::mutex> lockguard(m_mutex);
            if ((m_tail + 1) % m_max_size == m_head) {
                std::cout << "emplace error because of the full" << std::endl;
                return false;
            }
            std::allocator_traits<std::allocator<T>>::construct(m_allocator, m_data + m_tail, std::forward<Args>(args)...);
            m_tail = (m_tail + 1) % m_max_size;
            return true;
        }

        bool push(const T& val) {
            std::cout << "called push const T& version" << std::endl;
            return emplace(val);
        }

        bool push(T&& val) {
            std::cout << "called push T&& version" << std::endl;
            return emplace(std::move(val));
        }

        bool pop(T& val) {
            std::lock_guard<std::mutex> lockguard(m_mutex);
            if (m_head == m_tail) {
                std::cout << "circular queue empty! " << std::endl;
                return false;
            }
            val = std::move(m_data[m_head]);
            std::allocator_traits<std::allocator<T>>::destroy(m_allocator, m_data + m_head);
            m_head = (m_head + 1) % m_max_size;
            return true;
        }

    private:
        std::allocator<T> m_allocator;
        T* m_data;
        size_t m_max_size;
        size_t m_head;
        size_t m_tail;
        std::mutex m_mutex;
    };

    void test_queue() {
        Circular_queue<int> queue(5);


        int value;
        queue.pop(value);
        std::cout << "Popped value: " << value << std::endl;

        queue.push(3);
        queue.push(4);
        queue.push(5);

        while (queue.pop(value)) {
            std::cout << "Popped value: " << value << std::endl;
        }
        Circular_queue<int> queue2(2);
        int x = 20;
        queue2.push(x);
        int value2;
        queue2.pop(value2);
        std::cout << "Popped value: " << value2 << std::endl;
    }

    void queue_test_under_mu_thread() {
        Circular_queue<std::string >queue(5);
        std::string s1 = "sdasdas";
        std::string s2 = "123213213";
        std::thread t1([&] {queue.push(s1); });
        std::thread t2([&] {queue.push(s2); });
    
        t1.join();
        t2.join();
      
        std::string  value;
        while (queue.pop(value)) {
            std::cout << "Popped value: " << value << std::endl;
        }
    }
}

void test_exchange() {
    std::atomic<int> atomic_val(10);
    int expected = 10;
    int desired = 20;

    if (atomic_val.compare_exchange_weak(expected, desired)) {
        std::cout << "Exchange successful: " << atomic_val.load() << std::endl;
    }
    else {
        std::cout << "Exchange failed, expected was updated to: " << expected << std::endl;
    }
}

//利用原子类型实现无锁队列
namespace lock_free_queue{
    template<typename T, size_t Cap>
    class CircularQueSeq :private std::allocator<T> {
    public:
        CircularQueSeq() :_max_size(Cap + 1), _data(std::allocator<T>::allocate(_max_size)), _atomic_using(false), _head(0), _tail(0) {}
        CircularQueSeq(const CircularQueSeq&) = delete;
        CircularQueSeq& operator = (const CircularQueSeq&) volatile = delete;
        CircularQueSeq& operator = (const CircularQueSeq&) = delete;

        ~CircularQueSeq() {
            //循环销毁
            bool use_expected = false;
            bool use_desired = true;
            do
            {
                use_expected = false;
                use_desired = true;
            } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));
            //调用内部元素的析构函数
            while (_head != _tail) {
                std::allocator<T>::destroy(_data + _head);
                _head = （_head + 1） % _max_size;
            }
            //调用回收操作
            std::allocator<T>::deallocate(_data, _max_size);

            do
            {
                use_expected = true;
                use_desired = false;
            } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));
        }

        //先实现一个可变参数列表版本的插入函数最为基准函数
        template <typename ...Args>
        bool emplace(Args && ... args) {

            bool use_expected = false;
            bool use_desired = true;
            do
            {
                use_expected = false;
                use_desired = true;
            } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));

            //判断队列是否满了
            if ((_tail + 1) % _max_size == _head) {
                std::cout << "circular que full ! " << std::endl;
                do
                {
                    use_expected = true;
                    use_desired = false;
                } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));
                return false;
            }
            //在尾部位置构造一个T类型的对象，构造参数为args...
            std::allocator<T>::construct(_data + _tail, std::forward<Args>(args)...);
            //更新尾部元素位置
            _tail = (_tail + 1) % _max_size;

            do
            {
                use_expected = true;
                use_desired = false;
            } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));

            return true;
        }

        //push 实现两个版本，一个接受左值引用，一个接受右值引用

        //接受左值引用版本
        bool push(const T& val) {
            std::cout << "called push const T& version" << std::endl;
            return emplace(val);
        }

        //接受右值引用版本，当然也可以接受左值引用，T&&为万能引用
        // 但是因为我们实现了const T&
        bool push(T&& val) {
            std::cout << "called push T&& version" << std::endl;
            return emplace(std::move(val));
        }

        //出队函数
        bool pop(T& val) {

            bool use_expected = false;
            bool use_desired = true;
            do
            {
                use_desired = true;
                use_expected = false;
            } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));
            //判断头部和尾部指针是否重合，如果重合则队列为空
            if (_head == _tail) {
                std::cout << "circular que empty ! " << std::endl;
                do
                {
                    use_expected = true;
                    use_desired = false;
                } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));
                return false;
            }
            //取出头部指针指向的数据
            val = std::move(_data[_head]);
            //更新头部指针
            _head = (_head + 1) % _max_size;

            do
            {
                use_expected = true;
                use_desired = false;
            } while (!_atomic_using.compare_exchange_strong(use_expected, use_desired));
            return true;
        }
    private:
        size_t _max_size;
        T* _data;
        std::atomic<bool> _atomic_using;
        size_t _head = 0;
        size_t _tail = 0;
    };
    class MyClass{

    public:
        MyClass() = default;
        MyClass(int i) :m_i(i) {

        }
        ~MyClass() {
            std::cout << "delete myclass" << std::endl;
       }
    private:
        int m_i;
    };
    void test_code() {
        CircularQueSeq<MyClass, 3> cq_seq;
        for (int i = 0; i < 4; i++)
        {
            MyClass mc1(i);
            auto res = cq_seq.push(mc1);
            if (!res)
            {
                break;
            }
        }

        for (int i = 0; i < 4; i++)
        {
            MyClass mc1;
            auto res = cq_seq.pop(mc1);
            if (!res)
            {
                break;
            }

    
        }

        for (int i = 0; i < 4; i++)
        {
            MyClass mc1(i);
            auto res = cq_seq.push(mc1);
            if (!res)
            {
                break;
            }
        }

        for (int i = 0; i < 4; i++)
        {
            MyClass mc1;
            auto res = cq_seq.pop(mc1);
            if (!res)
            {
                break;
            }

            std::cout << "pop success, " << std::endl;
        }
    }
}

namespace freelock_stack {
    struct empty_stack : std::exception
    {
        const char* what() const throw();
    };
    template<typename T>
    class threadsafe_stack
    {
    private:
        std::stack<T> data;
        mutable std::mutex m;
    public:
        threadsafe_stack() {}

        threadsafe_stack(const threadsafe_stack& other)
        {
            std::lock_guard<std::mutex> lock(other.m);
            data = other.data;
        }

        threadsafe_stack& operator=(const threadsafe_stack&) = delete;

        void push(T new_value)
        {
            std::lock_guard<std::mutex> lock(m);
            data.push(std::move(new_value));    // ⇽-- - 1
        }

        std::shared_ptr<T> pop()
        {
            std::lock_guard<std::mutex> lock(m);
            if (data.empty()) throw empty_stack();  //  ⇽-- - 2
            std::shared_ptr<T> const res(
                std::make_shared<T>(std::move(data.top())));   // ⇽-- - 3
            data.pop();   // ⇽-- - 4
            return res;
        }

        void pop(T& value)
        {
            std::lock_guard<std::mutex> lock(m);
            if (data.empty()) throw empty_stack();
            value = std::move(data.top());   // ⇽-- - 5
            data.pop();   // ⇽-- - 6
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock(m);
            return data.empty();
        }
    };

}

int main() {
   

   //lock_queue::queue_test_under_mu_thread();
   //lock_queue::test_queue();
    //test_exchange();
    return 0;
}

