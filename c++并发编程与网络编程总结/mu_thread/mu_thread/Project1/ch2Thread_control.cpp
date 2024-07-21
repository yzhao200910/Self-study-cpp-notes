//#include <iostream>  
//#include <vector>  
//#include <thread>  
//#include <mutex>  
//#include <condition_variable>  
//#include <queue>  
//#include <string>  
//#include <stdexcept>
//#include <atomic>
//#include <future>
//
//class jThread {
//public:
//	explicit jThread() = default;
//
//	template<typename Callable, typename ... Args>
//	explicit jThread(Callable&& callback, Args&& ... args)
//		: m_t(std::forward<Callable>(callback), std::forward<Args>(args)...)
//	{}
//
//	jThread(std::thread t) noexcept : m_t(std::move(t)) {}
//
//	jThread(jThread&& other) noexcept : m_t(std::move(other.m_t)) {}
//
//	jThread& operator=(jThread&& other) {
//		if (joinable()) {
//			join();
//		}
//		m_t = std::move(other.m_t);
//		return *this;
//	}
//
//	jThread& operator=(jThread other) {
//		if (joinable()) {
//			join();
//		}
//		m_t = std::move(other.m_t);
//		return *this;
//	}
//
//	~jThread() {
//		if (joinable()) {
//			join();
//		}
//	}
//
//	void detach() {
//		if (joinable()) {
//			m_t.detach();
//		}
//	}
//
//	void swap_thread(jThread& other) {
//		if (joinable() && other.joinable()) {
//			m_t.swap(other.m_t);
//		}
//	}
//
//	bool joinable() const {
//		return m_t.joinable();
//	}
//
//	void join() {
//		if (joinable()) {
//			m_t.join();
//		}
//	}
//
//	std::thread::id get_id() const noexcept {
//		return m_t.get_id();
//	}
//
//private:
//	std::thread m_t;
//};
//
//void test_code() {
//	printf("dasdadas");
//
//}
