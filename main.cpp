#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

const int QUEUE_SIZE = 10;
const int NUM_ITEMS = 20;

std::queue<int> queue;
std::mutex mtx;
std::condition_variable cv_not_full;
std::condition_variable cv_not_empty;

void producer() {
	for (int i = 0; i < NUM_ITEMS; ++i) {
		std::unique_lock<std::mutex> lock(mtx);

		if (queue.size() == QUEUE_SIZE) {
			std::cout << "Producer is waiting, queue is full." << std::endl;
		}
		cv_not_full.wait(lock, [] { return queue.size() < QUEUE_SIZE; });

		queue.push(i);
		std::cout << "Produced item: " << i << ", Queue size: " << queue.size() << std::endl;

		cv_not_empty.notify_all();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void consumer() {
	for (int i = 0; i < NUM_ITEMS; ++i) {
		std::unique_lock<std::mutex> lock(mtx);

		if (queue.empty()) {
			std::cout << "Consumer is waiting, queue is empty." << std::endl;
		}
		cv_not_empty.wait(lock, [] { return !queue.empty(); });

		int item = queue.front();
		queue.pop();
		std::cout << "Consumed item: " << item << ", Queue size: " << queue.size() << std::endl;

		cv_not_full.notify_all();

		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
}

int main() {
	std::thread producer_thread(producer);
	std::thread consumer_thread(consumer);

	producer_thread.join();
	consumer_thread.join();

	return 0;
}
