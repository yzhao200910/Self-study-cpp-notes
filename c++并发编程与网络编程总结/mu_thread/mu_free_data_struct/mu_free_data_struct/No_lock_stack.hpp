#include <queue>
#include <set>
#include <cassert>
#include <iostream>
#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <list>
#include <shared_mutex>
#include <map>

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

namespace mutiThread_safe_list {
    template<typename T>
    class threadsafe_list
    {
        struct node
        {
            std::mutex m;
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
            node() :
                next()
            {}
            node(T const& value) :
                data(std::make_shared<T>(value))
            {}
        };

        node head;
    public:
        threadsafe_list()
        {}

        ~threadsafe_list()
        {
            remove_if([](node const&) {return true; });
        }

        threadsafe_list(threadsafe_list const& other) = delete;
        threadsafe_list& operator=(threadsafe_list const& other) = delete;

        void push_front(T const& value)
        {
            std::unique_ptr<node> new_node(new node(value));
            std::lock_guard<std::mutex> lk(head.m);
            new_node->next = std::move(head.next);
            head.next = std::move(new_node);
        }

        template<typename Function>
        void for_each(Function f)
        {
            node* current = &head;
            std::unique_lock<std::mutex> lk(head.m);
            while (node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                lk.unlock();
                f(*next->data);
                current = next;
                lk = std::move(next_lk);
            }
        }

        template<typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p)
        {
            node* current = &head;
            std::unique_lock<std::mutex> lk(head.m);
            while (node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                lk.unlock();
                if (p(*next->data))
                {
                    return next->data;
                }
                current = next;
                lk = std::move(next_lk);
            }
            return std::shared_ptr<T>();
        }

        template<typename Predicate>
        void remove_if(Predicate p)
        {
            node* current = &head;
            std::unique_lock<std::mutex> lk(head.m);
            while (node* const next = current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                if (p(*next->data))
                {
                    std::unique_ptr<node> old_next = std::move(current->next);
                    current->next = std::move(next->next);
                    next_lk.unlock();
                }
                else
                {
                    lk.unlock();
                    current = next;
                    lk = std::move(next_lk);
                }
            }
        }
    };
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
namespace test_serach_table {

}

namespace search2 {
    template<typename Key, typename Value, typename Hash = std::hash<Key>>
    class threadsafe_lookup_table
    {
    private:

        // 桶类型
        class bucket_type
        {
            friend class threadsafe_lookup_table;
        private:
            //存储元素的类型为pair，由key和value构成
            typedef std::pair<Key, Value> bucket_value;
            //由链表存储元素构
            typedef std::list<bucket_value> bucket_data;
            //链表的迭代器
            typedef typename bucket_data::iterator bucket_iterator;
            //链表数据
            bucket_data data;
            //改用共享锁
            mutable std::shared_mutex mutex;
            //查找操作，在list中找到匹配的key值，然后返回迭代器
            bucket_iterator find_entry_for(const Key& key)
            {
                return std::find_if(data.begin(), data.end(),
                    [&](bucket_value const& item)
                    {return item.first == key; });
            }
        public:
            //查找key值，找到返回对应的value，未找到则返回默认值
            Value value_for(Key const& key, Value const& default_value)
            {
                std::shared_lock<std::shared_mutex> lock(mutex);
                bucket_iterator const found_entry = find_entry_for(key);
                return (found_entry == data.end()) ?
                    default_value : found_entry->second;
            }
            //添加key和value，找到则更新，没找到则添加
            void add_or_update_mapping(Key const& key, Value const& value)
            {
                std::unique_lock<std::shared_mutex> lock(mutex);
                bucket_iterator const found_entry = find_entry_for(key);
                if (found_entry == data.end())
                {
                    data.push_back(bucket_value(key, value));
                }
                else
                {
                    found_entry->second = value;
                }
            }
            //删除对应的key
            void remove_mapping(Key const& key)
            {
                std::unique_lock<std::shared_mutex> lock(mutex);
                bucket_iterator const found_entry = find_entry_for(key);
                if (found_entry != data.end())
                {
                    data.erase(found_entry);
                }
            }
        };
        //用vector存储桶类型
        std::vector<std::unique_ptr<bucket_type>> buckets;
        //hash<Key> 哈希表 用来根据key生成哈希值
        Hash hasher;

        //根据key生成数字，并对桶的大小取余得到下标，根据下标返回对应的桶智能指针
        bucket_type& get_bucket(Key const& key) const
        {
            std::size_t const bucket_index = hasher(key) % buckets.size();
            return *buckets[bucket_index];
        }
    public:

        threadsafe_lookup_table(
            unsigned num_buckets = 19, Hash const& hasher_ = Hash()) :
            buckets(num_buckets), hasher(hasher_)
        {
            for (unsigned i = 0; i < num_buckets; ++i)
            {
                buckets[i].reset(new bucket_type);
            }
        }

        threadsafe_lookup_table(threadsafe_lookup_table const& other) = delete;
        threadsafe_lookup_table& operator=(
            threadsafe_lookup_table const& other) = delete;

        Value value_for(Key const& key,
            Value const& default_value = Value())
        {
            return get_bucket(key).value_for(key, default_value);
        }

        void add_or_update_mapping(Key const& key, Value const& value)
        {
            get_bucket(key).add_or_update_mapping(key, value);
        }

        void remove_mapping(Key const& key)
        {
            get_bucket(key).remove_mapping(key);
        }

        std::map<Key, Value> get_map()
        {
            std::vector<std::unique_lock<std::shared_mutex>> locks;
            for (unsigned i = 0; i < buckets.size(); ++i)
            {
                locks.push_back(
                    std::unique_lock<std::shared_mutex>(buckets[i]->mutex));
            }
            std::map<Key, Value> res;
            for (unsigned i = 0; i < buckets.size(); ++i)
            {
                //需用typename告诉编译器bucket_type::bucket_iterator是一个类型，以后再实例化
                //当然此处可简写成auto it = buckets[i]->data.begin();
                typename bucket_type::bucket_iterator it = buckets[i]->data.begin();
                for (; it != buckets[i]->data.end(); ++it)
                {
                    res.insert(*it);
                }
            }
            return res;
        }
    };
}



static const int MAX_HAZARD_POINTERS = 100;
struct HazardPointer {
    std::atomic<std::thread::id> id;
    std::atomic<void*> pointer;
    HazardPointer():pointer(nullptr){}
};

//风险指针数组
//extern HazardPointer hazard_pointers[MAX_HAZARD_POINTERS];//对于整个项目它是唯一的
 HazardPointer hazard_pointers[MAX_HAZARD_POINTERS];
//风险指针持有类
class hp_owner {
public:
    hp_owner(hp_owner const&) = delete;
    hp_owner operator=(hp_owner const&) = delete;
    hp_owner() :hp(nullptr) {
        bind_hazard_pointer();
    }
    std::atomic<void*>& get_pointer() {
        return hp->pointer;
    }

    ~hp_owner() {
        hp->pointer.store(nullptr);
        hp->id.store(std::thread::id());
    }

private:
    void bind_hazard_pointer() {
        for (unsigned i = 0; i < MAX_HAZARD_POINTERS; i++) {
           
            std::thread::id old_id;
            if (hazard_pointers[i].id.compare_exchange_strong(old_id, std::this_thread::get_id())) {
                hp = &hazard_pointers[i];
                break;
            }
        }
        if (!hp) throw std::runtime_error("No hazard pointers available");
    }
    HazardPointer* hp;
};

std::atomic<void*>& get_hazard_pointer_for_current_thread() {
    //每个线程都具有自己的风险指针 线程本地变量
    thread_local static hp_owner hazzard;
    return hazzard.get_pointer();
}

template<typename T>
class hazard_pointer_stack {

private:
    struct node {
        std::shared_ptr<T> data;
        node* next;
        node(T const& data_) :data(std::make_shared<T>(data_)),next(nullptr) {}
    };

    //待删节点
    struct data_to_reclaim {
        node* data;
        data_to_reclaim* next;
        data_to_reclaim(node* p) :data(p), next(nullptr) {}
        ~data_to_reclaim() {
            delete data;
        }
    };
private:
    hazard_pointer_stack(const hazard_pointer_stack&) = delete;
    hazard_pointer_stack& operator = (const hazard_pointer_stack&) = delete;
    std::atomic<node*> head;
    std::atomic<data_to_reclaim*>  nodes_to_reclaim;
public:
	hazard_pointer_stack() {}

	void push(T const& data) {
		node* const new_node = new node(data);    	
		new_node->next = head.load(std::memory_order_acquire); 
		// if true head = new_node ,false new_node->next = head
		while (!head.compare_exchange_weak(new_node->next, new_node,std::memory_order_release,std::memory_order_relaxed));   
	}

	bool outstanding_hazard_pointers_for(void* p)
	{
		for (unsigned i = 0; i < MAX_HAZARD_POINTERS; ++i)
		{
			if (hazard_pointers[i].pointer.load() == p)
			{
				return true;
			}
		}
		return false;
	}
	//待删列表插入
	void add_to_reclaim_list(data_to_reclaim* reclaim_node) {
		reclaim_node->next = nodes_to_reclaim.load();
		while (!nodes_to_reclaim.compare_exchange_weak(reclaim_node->next, reclaim_node));
	}

	void reclaim_later(node* old_head) {
		add_to_reclaim_list(new data_to_reclaim(old_head));
	}

	void delete_nodes_with_no_hazards() {
		//原子地以非原子实参的值替换原子对象的值，并返回该原子对象的旧值
		data_to_reclaim* current = nodes_to_reclaim.exchange(nullptr);
		while (current) {
			data_to_reclaim* const next = current->next;
			if (!outstanding_hazard_pointers_for(current->data)) {
				delete current;
			}
			else {
				add_to_reclaim_list(current);
			}

			current = next;
		}
	}

	std::shared_ptr<T> pop() {
		//1 从风险列表中获取一个节点给当前线程
		std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
		node* old_head = head.load();
		do
		{
			node* temp;
			do
			{
				temp = old_head;
				hp.store(old_head);
				old_head = head.load();
			}
			while (old_head != temp);
		}
		while (old_head &&
			!head.compare_exchange_strong(old_head, old_head->next)); 
		hp.store(nullptr);
		std::shared_ptr<T> res;
		if (old_head)
		{
			res.swap(old_head->data);
			
			if (outstanding_hazard_pointers_for(old_head))
			{
				
				reclaim_later(old_head);
			}
			else
			{
				
				delete old_head;
			}
		
			delete_nodes_with_no_hazards();
		}
		return res;
	}
};


// ref_count
template<typename T>
class single_ref_stack {
public:
	single_ref_stack() {

	}

	~single_ref_stack() {
		//循环出栈
		while (pop());
	}

	void push(T const& data) {
		auto new_node = ref_node(data);
		new_node._node_ptr->_next = head.load();
		while (!head.compare_exchange_weak(new_node._node_ptr->_next, new_node));
	}

	std::shared_ptr<T> pop() {
		ref_node old_head = head.load();
		for (;;) {
			//1 只要执行pop就对引用计数+1并更新到head中
			ref_node new_head;
			do {
				new_head = old_head;
				new_head._ref_count += 1;
			} while (!head.compare_exchange_weak(old_head, new_head));

			old_head = new_head;

			auto* node_ptr = old_head._node_ptr;
			if (node_ptr == nullptr) {
				return  std::shared_ptr<T>();
			}

			//2 比较head和old_head想等则交换否则说明head已经被其他线程更新
			if (head.compare_exchange_strong(old_head, node_ptr->_next)) {

				//要返回的值
				std::shared_ptr<T> res;
				//交换智能指针
				res.swap(node_ptr->_data);

				//增加的数量
				int increase_count = old_head._ref_count - 2;

				if (node_ptr->_dec_count.fetch_add(increase_count) == -increase_count) {
					delete node_ptr;
				}

				return res;
			}
			else {
				if (node_ptr->_dec_count.fetch_sub(1) == 1) {
					delete node_ptr;
				}
			}
		}
	}

private:
	struct ref_node;
	struct node {
		//1 数据域智能指针
		std::shared_ptr<T>  _data;
		//2  下一个节点
		ref_node _next;
		node(T const& data_) : _data(std::make_shared<T>(data_)) {}

		//减少的数量
		std::atomic<int>  _dec_count;
	};

	struct ref_node {
		// 引用计数
		int _ref_count;

		node* _node_ptr;
		ref_node(T const& data_) :_node_ptr(new node(data_)), _ref_count(1) {}

		ref_node() :_node_ptr(nullptr), _ref_count(0) {}
	};

	//头部节点
	std::atomic<ref_node> head;
};



template<typename T>
class LockFreeStack {
public:
    struct Node {
        std::shared_ptr<T> data;
        Node* next;
        std::atomic<int> ref_count;

        Node(T const& data) : data(std::make_shared<T>(data)), next(nullptr), ref_count(1) {}
    };

    LockFreeStack() : head(nullptr) {}

    void push(T const& data) {
        Node* new_node = new Node(data);
        new_node->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_weak(new_node->next, new_node, std::memory_order_release, std::memory_order_relaxed));
    }

    std::shared_ptr<T> pop() {
        Node* old_head = head.load(std::memory_order_acquire);
        while (true) {
            if (old_head == nullptr) {
                return std::shared_ptr<T>(); // Stack is empty
            }

            Node* new_head = old_head;
            new_head->ref_count.fetch_add(1, std::memory_order_relaxed); // Increase reference count to protect node_ptr

            if (head.compare_exchange_weak(old_head, new_head, std::memory_order_acquire, std::memory_order_relaxed)) {
                break; // Successfully increased reference count
            }

            old_head->ref_count.fetch_sub(1, std::memory_order_relaxed);
            old_head = head.load(std::memory_order_acquire);
        }

        Node* node_ptr = old_head;
        if (head.compare_exchange_strong(old_head, node_ptr->next, std::memory_order_release, std::memory_order_relaxed)) {
            std::shared_ptr<T> res;
            res.swap(node_ptr->data);

            int increase_count = old_head->ref_count.load(std::memory_order_relaxed) - 2;
            if (node_ptr->ref_count.fetch_add(increase_count, std::memory_order_release) == -increase_count) {
                delete node_ptr;
            }

            return res;
        }
        else {
            if (node_ptr->ref_count.fetch_sub(1, std::memory_order_release) == 1) {
                delete node_ptr;
            }
        }

    }

private:
    std::atomic<Node*> head;
};

// 测试代码
void test() {
    LockFreeStack<int> stack;
    std::cout << "start" << std::endl;
    // 多线程写入
    std::thread writer1([&stack] {
        for (int i = 0; i < 100; ++i) {
            stack.push(i);
        }
        });

    std::thread writer2([&stack] {
        for (int i = 100; i < 200; ++i) {
            stack.push(i);
        }
        });

    // 多线程读取
    std::thread reader1([&stack] {
        for (int i = 0; i < 100; ++i) {
            auto data = stack.pop();
            if (data) {
                std::cout << "Reader1 popped: " << *data << std::endl;
            }
        }
        });

    std::thread reader2([&stack] {
        for (int i = 0; i < 100; ++i) {
            auto data = stack.pop();
            if (data) {
                std::cout << "Reader2 popped: " << *data << std::endl;
            }
        }
        });

    writer1.join();
    writer2.join();
    reader1.join();
    reader2.join();
}



namespace testFunc {
    void test_func() {
        hazard_pointer_stack<int> hazard_stack;
        std::set<int>  rmv_set;
        std::mutex set_mtx;

        std::thread t1([&]() {
            for (int i = 0; i < 20000; i++) {
                hazard_stack.push(i);
                std::cout << "push data " << i << " success!" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            });

        std::thread t2([&]() {
            for (int i = 0; i < 10000;) {
                auto head = hazard_stack.pop();
                if (!head) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                std::lock_guard<std::mutex> lock(set_mtx);
                rmv_set.insert(*head);
                std::cout << "pop data " << *head << " success!" << std::endl;
                i++;
            }
            });

        std::thread t3([&]() {
            for (int i = 0; i < 10000;) {
                auto head = hazard_stack.pop();
                if (!head) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                std::lock_guard<std::mutex> lock(set_mtx);
                rmv_set.insert(*head);
                std::cout << "pop data " << *head << " success!" << std::endl;
                i++;
            }
            });

        t1.join();
        t2.join();
        t3.join();

        assert(rmv_set.size() == 20000);


    }


}