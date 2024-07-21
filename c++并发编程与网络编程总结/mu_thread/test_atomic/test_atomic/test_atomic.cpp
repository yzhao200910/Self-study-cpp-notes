#include <atomic>
#include <thread>
#include <iostream>
#include <assert.h>



//synchronizes-with 's problem and  Sequential constraint problem
//test the std::memory_order_relaxed
namespace test1 {
    std::atomic<int> a = 2;
    void set_value() {
        a.store(10, std::memory_order_relaxed);
    }
    void read_value() {

        // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cout << "a's value" << a << std::endl;
    }

    void run() {
        std::thread t1(set_value);

        std::thread t2(read_value);
        t1.join();
        t2.join();
    }

    std::atomic<bool> start;
    std::atomic<bool> end;
    std::atomic<int> z = 0;

    void Set(bool a) {
        //有可能编译器优化导致，他们不一定会按照原来的顺序执行end->start，有可能是start->end
        end.store(a, std::memory_order_relaxed);
        start.store(a, std::memory_order_relaxed);
   
    }
    void Get() {
        while (!start.load(std::memory_order_relaxed))
            std::cout << "we have catch the start' value change" << std::endl;
        
        while (!end.load(std::memory_order_relaxed)) {
            ++z;
            std::cout << "end value still false" << std::endl;
        }
        if (z) {
            std::cout << "end is false,and the order is " << "z's value" << z << std::endl;
        }
    }
    void run2() {
        std::thread t1(Set, true);
        std::thread t2(Get);
        t1.join();
        t2.join();
        std::cout << "test1 run2 over" << std::endl;
    }
    

}

//test std::memory_order_seq_cst
namespace test2 {

    std::atomic<bool> start;
    std::atomic<bool> end;
    std::atomic<int> z = 0;

    void Set(bool a) {
        start.store(a, std::memory_order_seq_cst);
        end.store(a, std::memory_order_seq_cst);
        
    }
    void Get() {
        if(start.load(std::memory_order_seq_cst))
            std::cout << "we have catch the start' value change" << std::endl;

        if(end.load(std::memory_order_seq_cst)) {
            ++z;
            std::cout << "end value still is true" << std::endl;
        }
        if (z) {
            std::cout << "end is false,and the order is " << "z's value : " << z << std::endl;
        }
    }
    void run() {
        std::thread t1(Set, true);
        std::thread t2(Get);
        t1.join();
        t2.join();
        std::cout << "test2 run over" << std::endl;
    }


}
//test  the Acquire-Release Memory Model
namespace test3 {
    std::atomic<bool> start;
    std::atomic<bool> end;
    std::atomic<int> z = 0;

    void run() {
        std::thread t1([&] {
            while(!start.load(std::memory_order_acquire))
                std::cout << "wait the start value" << std::endl;
           
            });

        std::thread t2([&] {
            start.store(true,std::memory_order_release);
            });
        t1.join();
        t2.join();
        std::cout << "test3 run over" << std::endl;
    }
   // use this model keep the relax_model run correct

    void run(int) {
        std::thread t1([&] {
            while (!end.load(std::memory_order_acquire))
                std::cout << "wait the start value" << std::endl;

            if (start)
                std::cout << "get the value change " << std::endl;

            }
        );
 
        //利用该模型是得relax可以正常进行线程同步,保证顺序是start->end
        std::thread t2([&] {
            start.store(true, std::memory_order_relaxed);
            end.store(true, std::memory_order_release);
            });
        std::thread t3([&] {
            end.store(true, std::memory_order_release);
            });
        t1.join();
        t2.join();
        t3.join();
        std::cout << "test3 run over" << std::endl;
    }

    void ConsumeDependency() {
        std::atomic<std::string*> ptr;
        int data;

        std::thread t1([&]() {
            std::string* p = new std::string("Hello World"); // (1)
            data = 42; // (2)
            ptr.store(p, std::memory_order_release); // (3)
            });

        std::thread t2([&]() {
            std::string* p2;
            while (!(p2 = ptr.load(std::memory_order_consume))); // (4)
            assert(*p2 == "Hello World"); // (5)
            assert(data == 42); // (6)
            });

        t1.join();
        t2.join();
    }

}


int main() {
    //test1::run();
    //test1::run2();
    //test2::run();
    //test3::run();
    //test3::run(1);
    test3::ConsumeDependency();
    return 0;
}