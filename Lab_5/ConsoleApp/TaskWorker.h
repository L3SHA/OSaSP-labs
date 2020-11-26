#pragma once

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