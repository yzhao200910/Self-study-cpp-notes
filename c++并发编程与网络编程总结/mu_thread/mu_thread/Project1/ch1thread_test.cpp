#include <iostream>  
#include <vector>  
#include <thread>  
#include <mutex>  
#include <condition_variable>  
#include <queue>  
#include <string>  
#include <stdexcept>
#include <atomic>
#include <future>
#include "windows.h"
#define  THREADPOOL_MAX_NUM 16
using namespace std;

//用于线程的安全回收
struct func {
    int& _i;
    func(int& i) : _i(i) {}
    void operator()() {
        for (int i = 0; i < 3; i++) {
            _i = i;
            std::cout << "_i is " << _i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

class thread_guard {
private:
    std::thread& _t;
public:
    explicit thread_guard(std::thread& t) :_t(t) {}
    ~thread_guard() {
        //join只能调用一次
        cout << "wait the thread" << endl;
        if (_t.joinable()) {
            _t.join();
            cout << "wait the thread sucessful" << endl;
        }
    }
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};
	 


//detach,主子线程分离谨慎使用
//当子线程使用主线程的一些资源时,(以引用,指针等),如果使用thread::detach,会在子线程执行的某个时刻,发生未定义行为
void oops() {
    int some_local_state = 0;
    func myfunc(some_local_state);
    std::thread functhread(myfunc);
    //隐患，访问局部变量，局部变量可能会随着}结束而回收或随着主线程退出而回收
    functhread.detach();
}
//对于thread由于某些原因一直挂起,回收问题
//常见场景,线程池,生产者消费者某型,当任务队列为空时,线程回收问题
//常见方式,使用条件变量唤醒
namespace Thread_pool {
	class threadpool
	{
		using Task = function<void()>;	//定义类型
		vector<thread> _pool;           //线程池
		queue<Task> _tasks;             //任务队列
		mutex _lock;                    //同步
		condition_variable _task_cv;    //条件阻塞
		atomic<bool> _run{ true };      //线程池是否执行,原子变量
		atomic<int>  _idlThrNum{ 0 };   //空闲线程数量,原子变量

	public:
		inline threadpool(unsigned short size = 4) { addThread(size); }

		inline ~threadpool() {
			_run = false;
			_task_cv.notify_all();
			for (thread& thread : _pool) {
				//thread.detach(); // 让线程“自生自灭”,不建议这种,前面提到
				if (thread.joinable())
					thread.join(); // 等待任务结束， 前提：线程一定会执行完
			}
		}

	public:
		// 提交一个任务
		// 调用.get()获取返回值会等待任务执行完,获取返回值
		// 有两种方法可以实现调用类成员，
		// 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
		// 一种是用   mem_fn： .commit(std::mem_fn(&Dog::sayHello), this)
		template<class F, class... Args>
		auto commit(F&& f, Args&&... args) -> future<decltype(f(args...))>//自定义识别,传参为左值还是右值
		{
			if (!_run)    // stoped ??
				throw runtime_error("commit on ThreadPool is stopped.");

			using RetType = decltype(f(args...));  // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
			// 把函数入口及参数,打包(绑定)
			auto task = make_shared<packaged_task<RetType()>>(bind(forward<F>(f), forward<Args>(args)...));//完美转发,让传进的参数是什么类型,就以什么类型,传入
			future<RetType> future = task->get_future();
			{    // 添加任务到队列
				lock_guard<mutex> lock{ _lock };
				_tasks.emplace([task]() { (*task)(); });
			}

#ifdef THREADPOOL_AUTO_GROW
			if (_idlThrNum < 1 && _pool.size() < THREADPOOL_MAX_NUM)
				addThread(1);
#endif // !THREADPOOL_AUTO_GROW
			_task_cv.notify_one(); // 唤醒一个线程执行
			return future;
		}
		//空闲线程数量
		int idlCount() { return _idlThrNum; }
		//线程数量
		int thrCount() { return _pool.size(); }

#ifndef THREADPOOL_AUTO_GROW
	private:
#endif // !THREADPOOL_AUTO_GROW

		//添加指定数量的线程
		void addThread(unsigned short size) {
			for (; _pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
			{   //增加线程数量,但不超过 预定义数量 THREADPOOL_MAX_NUM
				_pool.emplace_back([this] { //工作线程函数
					while (_run) {
						Task task; // 获取一个待执行的 task
						{
							// unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
							unique_lock<mutex> lock{ _lock };
							_task_cv.wait(lock, [this] {return !_run || !_tasks.empty(); }); // wait 直到有 task
							if (!_run && _tasks.empty())
								return;

							task = move(_tasks.front()); // 按先进先出从队列取一个 task
							_tasks.pop();
						}
						_idlThrNum--;
						task();//执行任务
						_idlThrNum++;
					}
					});
				_idlThrNum++;
			}
		}
	};

}


void fun1(int slp) {
	printf("  hello, fun1 !  %d\n", std::this_thread::get_id());
	if (slp > 0) {
		printf(" ======= fun1 sleep %d  =========  %d\n", slp, std::this_thread::get_id());
		std::this_thread::sleep_for(std::chrono::milliseconds(slp));
		//Sleep(slp );
	}
}

struct gfun {
	int operator()(int n) {
		std::cout << "hello, gfun ! " << n << "   " << std::this_thread::get_id();
		return 42;
	}
};

namespace test_class {
	class A {
	public:
		static int Afun(int n = 0) {
			std::cout << "hello, Afun !  " << " param: " << n
				<< std::this_thread::get_id() << std::endl;
			return n;
		}

		static std::string Bfun(int n, std::string str, char c) {
			std::cout << n << "  hello, Bfun !  param: " << str.c_str()
				<< "  " << (int)c << "  " << std::this_thread::get_id() << std::endl;
			return str;
		}
	};

}

namespace test_func {
	int myfun1(int num) {
		std::cout << std::this_thread::get_id() << "  " << __FUNCTION__ << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(num));
		return (num * num);
	}

	int myfun2(int num1, int num2) {
		std::cout << std::this_thread::get_id() << "  " << __FUNCTION__ << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(num1));
		return (num1 + num2);
	}

	void auto_guard() {
		int some_local_state = 0;
		func my_func(some_local_state);
		std::thread  t(my_func);
		thread_guard g(t);
		//本线程做一些事情
		std::cout << "auto guard finished " << std::endl;
	}

	void run() {

		cout << "thread 1 test" << endl;
	}

	//对于栈空间的回收，是由低地址向高地址回收的
	void test_code() {
		thread t(run);
		thread_guard g(t);
		cout << "test_code fuc over" << endl;
	}

	//一些问题,可能造成程序的崩溃,比如一些隐式转换,char* 转 string这种
	//说来说去,还是围绕一个问题,就是deatch的使用,如果使用了主线程资源,慎用
	//测试出现了两个问题
	//1: 当stdio_printf函数参数以引用的方式,会出现问题

	void stdio_printf(string s) {

		cout << "the test func stdio_printf ,want to test the wrong in the danger_thread" << endl;
		cout << "string s :" << s << endl;
	}
	// 运行时程序可能直接直接崩溃
	void danger_threadWrong(int parm) {
		char buffer[1024];//数组名为首地址,自然是char* 类型	
	sprintf_s(buffer,"%i",parm);
		//char* 转 string
		thread t1(stdio_printf, string(buffer));
		t1.detach();
		
	}

}

class joining_thread {
	std::thread  _t;
public:
	joining_thread() noexcept = default;
	template<typename Callable, typename ...  Args>
	explicit  joining_thread(Callable&& func, Args&& ...args) :
		_t(std::forward<Callable>(func), std::forward<Args>(args)...) {}
	explicit joining_thread(std::thread  t) noexcept : _t(std::move(t)) {}
	joining_thread(joining_thread&& other) noexcept : _t(std::move(other._t)) {}
	joining_thread& operator=(joining_thread&& other) noexcept
	{
		//如果当前线程可汇合，则汇合等待线程完成再赋值
		if (joinable()) {
			join();
		}
		_t = std::move(other._t);
		return *this;
	}
	joining_thread& operator=(joining_thread other) noexcept
	{
		//如果当前线程可汇合，则汇合等待线程完成再赋值
		if (joinable()) {
			join();
		}
		_t = std::move(other._t);
		return *this;
	}
	~joining_thread() noexcept {
		if (joinable()) {
			join();
		}
	}
	void swap(joining_thread& other) noexcept {
		_t.swap(other._t);
	}
	std::thread::id   get_id() const noexcept {
		return _t.get_id();
	}
	bool joinable() const noexcept {
		return _t.joinable();
	}
	void join() {
		_t.join();
	}
	void detach() {
		_t.detach();
	}
	std::thread& as_thread() noexcept {
		return _t;
	}
	const std::thread& as_thread() const noexcept {
		return _t;
	}
};

void use_jointhread() {
	//1 根据线程构造函数构造joiningthread
	joining_thread j1([](int maxindex) {
		for (int i = 0; i < maxindex; i++) {
			std::cout << "in thread id " << std::this_thread::get_id()
				<< " cur index is " << i << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		}, 10);
	//2 根据thread构造joiningthread
	joining_thread j2(std::thread([](int maxindex) {
		for (int i = 0; i < maxindex; i++) {
			std::cout << "in thread id " << std::this_thread::get_id()
				<< " cur index is " << i << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		}, 10));
	//3 根据thread构造j3
	joining_thread j3(std::thread([](int maxindex) {
		for (int i = 0; i < maxindex; i++) {
			std::cout << "in thread id " << std::this_thread::get_id()
				<< " cur index is " << i << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		}, 10));
	//4 把j3赋值给j1，joining_thread内部会等待j1汇合结束后
	//再将j3赋值给j1
	//j1 = std::move(j3);

}


class jThread {
public:
	explicit jThread() = default;

	template<typename Callable, typename ... Args>
	explicit jThread(Callable&& callback, Args&& ... args)
		: m_t(std::forward<Callable>(callback), std::forward<Args>(args)...)
	{}

	jThread(std::thread t) noexcept : m_t(std::move(t)) {}

	jThread(jThread&& other) noexcept : m_t(std::move(other.m_t)) {}

	jThread& operator=(jThread&& other) {
		if (joinable()) {
			join();
		}
		m_t = std::move(other.m_t);
		return *this;
	}

	jThread& operator=(jThread other) {
		if (joinable()) {
			join();
		}
		m_t = std::move(other.m_t);
		return *this;
	}

	~jThread() {
		if (joinable()) {
			join();
		}
	}

	void detach() {
		if (joinable()) {
			m_t.detach();
		}
	}

	void swap_thread(jThread& other) {
		if (joinable() && other.joinable()) {
			m_t.swap(other.m_t);
		}
	}

	bool joinable() const {
		return m_t.joinable();
	}

	void join() {
		if (joinable()) {
			m_t.join();
		}
	}

	std::thread::id get_id() const noexcept {
		return m_t.get_id();
	}

private:
	std::thread m_t;
};

void test_code(){
	printf("dasdadas");

}


int  main()
{
	/*std::threadpool mypool{ 5 };
	std::future<int> f_fun1 = mypool.commit(myfun1, 5);
	std::future<int> f_fun2 = mypool.commit(myfun2, 1, 2);
	std::future<int> f_gfun = mypool.commit(gfun{}, 0);
	std::future<int> f_afun = mypool.commit(A::Afun, 9999);
	std::future<std::string> f_bfun = mypool.commit(A::Bfun, 9998, "mult args", 123);
	std::future<std::string> f_lambda = mypool.commit([]()->std::string { std::cout << "hello, fh !  "
		<< std::this_thread::get_id() << std::endl; return "hello,fh ret !"; });

	std::cout << "idlCount: " << mypool.idlCount() << std::endl;
	std::cout << "thrCount: " << mypool.thrCount() << std::endl;
	std::cout << f_fun1.get() << std::endl;
	std::cout << f_fun2.get() << std::endl;
	std::cout << f_gfun.get() << std::endl;
	std::cout << f_afun.get() << std::endl;
	std::cout << f_bfun.get().c_str() << std::endl;
	std::cout << f_lambda.get().c_str() << std::endl;*/
	//test_func::test_code();
	//test_func::danger_threadWrong(2);
	//jThread j1(test_code);

	 SYSTEM_INFO sysInfo;
	 GetSystemInfo(&sysInfo);
	 printf("now system cpu num is %d\n", sysInfo.dwNumberOfProcessors);
	cout << std::thread::hardware_concurrency << endl;
	return 0;
}


