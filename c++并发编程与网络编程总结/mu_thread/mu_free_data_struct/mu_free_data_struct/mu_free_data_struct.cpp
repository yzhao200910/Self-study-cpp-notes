// mu_free_data_struct.cpp
// Better grasp the accuracy of the lock
//
#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <string>
#include <chrono>

// Thread-safe queue based on the forward-list
namespace queue {
    template<class T>
    class mu_lock_queue {
    public:
        mu_lock_queue() : m_head(new node), m_tail(m_head.get()) {
            std::cout << "construct the queue successful" << std::endl;
        };
        mu_lock_queue(const mu_lock_queue& other) = delete;
        mu_lock_queue(const mu_lock_queue&& other) = delete;
        mu_lock_queue& operator=(const mu_lock_queue& other) = delete;
        mu_lock_queue& operator=(const mu_lock_queue&& other) = delete;
        ~mu_lock_queue() {
            std::cout << "destroy the queue successful" << std::endl;
        }

        std::shared_ptr<T> pop() {
            std::unique_lock<std::mutex> mlock(wait_for_data());
            std::unique_ptr<node> temp = std::move(m_head);
            m_head = std::move(temp->m_next);
            return temp->m_data;
        }

        void push(T new_value) {
            std::unique_ptr<node> new_node = std::make_unique<node>();
           // std::shared_ptr<T> new_data = std::make_shared<T>(std::move(new_value));
            node* new_tail = new_node.get();
            {
                std::lock_guard<std::mutex> lock(m_tail_lock);
                m_tail->m_data = std::make_shared<T>(std::move(new_value));
                m_tail->m_next = std::move(new_node);
                m_tail = new_tail;
            }
           m_cond.notify_one();
        }

    private:
        std::unique_lock<std::mutex> wait_for_data() {
            std::unique_lock<std::mutex> ui_lock(m_head_lock);
            m_cond.wait(ui_lock, [&] {
                return m_head.get() != m_tail;
                });
            return ui_lock;
        }

        struct node {
            std::shared_ptr<T> m_data;
            std::unique_ptr<node> m_next;
        };

        std::mutex m_head_lock;
        std::mutex m_tail_lock;
        std::condition_variable m_cond;
        std::unique_ptr<node> m_head; // pop
        node* m_tail; //push
    };
}

namespace queue2 {
    template<typename T>
    class threadsafe_queue_ht
    {
    private:
        struct node
        {
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
        };
        std::mutex head_mutex;
        std::unique_ptr<node> head;
        std::mutex tail_mutex;
        node* tail;
        std::condition_variable data_cond;

        node* get_tail()
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }
        std::unique_ptr<node> pop_head()
        {
            std::unique_ptr<node> old_head = std::move(head);
            head = std::move(old_head->next);
            return old_head;
        }
        std::unique_lock<std::mutex> wait_for_data()
        {
            std::unique_lock<std::mutex> head_lock(head_mutex);
            data_cond.wait(head_lock, [&] {return head.get() != get_tail(); }); //5
            return std::move(head_lock);
        }
        std::unique_ptr<node> wait_pop_head()
        {
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            return pop_head();
        }
        std::unique_ptr<node> wait_pop_head(T& value)
        {
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            value = std::move(*head->data);
            return pop_head();
        }


        std::unique_ptr<node> try_pop_head()
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if (head.get() == get_tail())
            {
                return std::unique_ptr<node>();
            }
            return pop_head();
        }
        std::unique_ptr<node> try_pop_head(T& value)
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if (head.get() == get_tail())
            {
                return std::unique_ptr<node>();
            }
            value = std::move(*head->data);
            return pop_head();
        }
    public:

        threadsafe_queue_ht() :  // ⇽-- - 1
            head(new node), tail(head.get())
        {}

        threadsafe_queue_ht(const threadsafe_queue_ht& other) = delete;
        threadsafe_queue_ht& operator=(const threadsafe_queue_ht& other) = delete;

        std::shared_ptr<T> wait_and_pop() //  <------3
        {
            std::unique_ptr<node> const old_head = wait_pop_head();
            return old_head->data;
        }

        void wait_and_pop(T& value)  //  <------4
        {
            std::unique_ptr<node> const old_head = wait_pop_head(value);
        }


        std::shared_ptr<T> try_pop()
        {
            std::unique_ptr<node> old_head = try_pop_head();
            return old_head ? old_head->data : std::shared_ptr<T>();
        }
        bool try_pop(T& value)
        {
            std::unique_ptr<node> const old_head = try_pop_head(value);
            return old_head;
        }
        bool empty()
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            return (head.get() == get_tail());
        }

        void push(T new_value)  //<------2
        {
            std::shared_ptr<T> new_data(
                std::make_shared<T>(std::move(new_value)));
            std::unique_ptr<node> p(new node);
            node* const new_tail = p.get();
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;
            tail->next = std::move(p);
            tail = new_tail;
        }
    };
}
namespace search_table {







}
// Function for producer threads to push strings into the queue
namespace test_queue {
    void producer(queue::mu_lock_queue<std::string>& q, int id) {
        for (int i = 0; i < 10; ++i) {
            std::string str = "Producer " + std::to_string(id) + " - String " + std::to_string(i);
            q.push(str);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // Function for consumer threads to pop strings from the queue
    void consumer(queue::mu_lock_queue<std::string>& q, int id) {
        for (int i = 0; i < 10; ++i) {
            std::shared_ptr<std::string> str = q.pop();
            if (str) {
                std::cout << "Consumer " << id << " - Popped: " << *str << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void testfunc_queue() {
        queue::mu_lock_queue<std::string> q;

        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;

        // Create producer threads
        for (int i = 0; i < 3; ++i) {
            producers.emplace_back(producer, std::ref(q), i);
        }

        // Create consumer threads
        for (int i = 4; i < 6; ++i) {
            consumers.emplace_back(consumer, std::ref(q), i);
        }

        // Join producer threads
        for (auto& t : producers) {
            t.join();
        }

        // Join consumer threads
        for (auto& t : consumers) {
            t.join();
        }

        std::cout << "Test run over" << std::endl;
    }
}


int main() {

    test_queue::testfunc_queue();

    return 0;
}
