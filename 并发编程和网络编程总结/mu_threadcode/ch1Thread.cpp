#include <thread>
using namespace std;

void run() {

	cout << "thread 1 test" << endl;
}


int main() {

	thread t(run);
	t.join();

	cout << "main thread over test" << endl;

	return 0;

}
