#include "stdafx.h"
#include "Log.h"
#include "Config.h"
#include "ModuleLoader.h"
#include "TaskQueue.h"

using namespace Amendux;

TaskQueue *TaskQueue::s_TaskQueue = nullptr;

TaskQueue::TaskQueue()
{
	Log::Info(L"TaskQueue", L"Initialize module task queue class");
}


TaskQueue::~TaskQueue()
{
	Log::Info(L"TaskQueue", L"Terminate module task queue class");
}


void TaskQueue::InitClass()
{
	if (!Config::Current()->CanRunWorker()) {
		return;
	}
}


void TaskQueue::Add(const std::string& mod)
{
	Task task(mod);

	Add(task);
}


void TaskQueue::Add(Task& task)
{
	mainqueue.push(task);

	Log::Info(L"TaskQueue", L"Enqueued new task " + std::to_wstring(task.GetId()));
}


DWORD TaskQueue::Worker() {
	int nextInterval = 0;
	while (true) {
		/* Randomize the interval */
		Sleep((nextInterval) + 100);
		if (Empty()) {
			continue;
		}

		Task task = mainqueue.front();

		Log::Info(L"TaskQueue", L"Pop task " + std::to_wstring(task.GetId()) + L" from queue");

		if (!ModuleLoader::Current()->RunModule(task.name, task.params)) {
			Log::Warn(L"TaskQueue", L"Task is not an internal module");
		}

		mainqueue.pop();

		nextInterval = (rand() % 180) * 1000;
	}

	return 0;
}
