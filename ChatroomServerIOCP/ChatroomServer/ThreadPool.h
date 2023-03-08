#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <winsock2.h>
#pragma once

using namespace std;

// Define the maximum number of threads in the thread pool.
const int MAX_THREADS = 10;

// A structure to hold the client socket and address information.
struct ClientInfo {
	SOCKET socket;
	SOCKADDR_IN address;
};

// Define a thread pool class.
class ThreadPool {
	public:
		ThreadPool() {
			stop = false;
			for (int i = 0; i < MAX_THREADS; i++) {
				workers[i] = new thread(&ThreadPool::WorkerThread, this);
			}
		}

		~ThreadPool() {
			stop = true;
			tasksCV.notify_all();
			for (int i = 0; i < MAX_THREADS; i++) {
				if (workers[i]->joinable()) {
					workers[i]->join();
				}
				delete workers[i];
			}
		}

		// Add a task to the thread pool.
		void AddTask(ClientInfo clientInfo) {
			// Lock the task queue.
			lock_guard<mutex> lock(tasksMutex);

			// Add the new task to the queue.
			tasks.push(clientInfo);

			// Notify a waiting worker thread that a new task is available.
			tasksCV.notify_one();
		}

	private:
		// The worker function for each thread in the pool.

		// A queue to hold tasks.
		queue<ClientInfo> tasks;

		// A mutex to protect the task queue.
		mutex tasksMutex;

		// A condition variable to signal when new tasks are added to the queue.
		condition_variable tasksCV;

		// An array of worker threads.
		thread* workers[MAX_THREADS];

		// A flag to indicate when the thread pool should stop processing tasks.
		bool stop;
		void WorkerThread() {
			while (true) {
				// Lock the task queue.
				unique_lock<mutex> lock(tasksMutex);

				// Wait for a task to become available.
				tasksCV.wait(lock, [this] { return !tasks.empty() || stop; });

				// If the thread pool is stopping, exit the worker thread.
				if (stop && tasks.empty()) {
					return;
				}

				// Get the next task from the queue.
				ClientInfo clientInfo = tasks.front();
				tasks.pop();

				// Unlock the task queue.
				lock.unlock();

				// Process the task.
				// Example code to send a response back to the client
				char buffer[1024] = { 0 };
				const char* response = "Hello, client!";
				send(clientInfo.socket, response, strlen(response), 0);

				// Close the client socket.
				closesocket(clientInfo.socket);
			}
		}
};