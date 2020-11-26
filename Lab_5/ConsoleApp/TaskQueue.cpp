#include <mutex>
#include <queue>
#include <functional>

typedef std::function<void()> TTask;

class TaskQueue
{
public:
	TaskQueue();
	int addTask(TTask task);
	TTask popTask();
private:
	std::queue<TTask>* tasksQueue = new std::queue<TTask>;
};

std::mutex g_lock;

TaskQueue::TaskQueue()
{
}


int TaskQueue::addTask(TTask task)
{
	g_lock.lock();
	tasksQueue->push(task);
	g_lock.unlock();

	return 1;
}

TTask TaskQueue::popTask()
{
	TTask result;

	g_lock.lock();
	if (tasksQueue->empty()) {
		result = NULL;
	}
	else {
		result = tasksQueue->front();
		tasksQueue->pop();
	}
	g_lock.unlock();

	return result;
}