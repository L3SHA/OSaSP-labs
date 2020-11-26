#include <thread>
#include "TaskQueue.h"
class TaskWorker
{
private:
	TaskQueue _queue;
	int _maxThreadsCount;
public:
	TaskWorker(TaskQueue queue);

	void startExecution(int maxThreadsCount);
};

TaskWorker::TaskWorker(TaskQueue queue)
{
	_queue = queue;
}


void threadFunction(TaskQueue queue, int count)
{
	std::vector<std::thread> arr;
	while (count) {
		TTask task = queue.popTask();
		if (task != NULL) {
			std::thread thr(task);
			arr.push_back(move(thr));
			count--;
		}
	}

	for (int i = 0; i < arr.size(); i++) {
		arr[i].join();
	}
}

void TaskWorker::startExecution(int maxThreadsCount) {
	std::thread thr(threadFunction, _queue, maxThreadsCount);
	thr.join();
}