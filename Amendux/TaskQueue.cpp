#include "stdafx.h"
#include "Log.h"
#include "Config.h"
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
	if (!Config::Current()->CanRunModules()) {
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
	while (true) {
		/* Randomize the interval for obvious reasons */
		Sleep(2 * 60 * 1000);

		if (Empty()) {
			continue;
		}

		Task task = mainqueue.front();

		Log::Info(L"TaskQueue", L"Execute task " + std::to_wstring(task.GetId()));

		// look for mod and run
		//if () {
		//
		//}

		mainqueue.pop();
	}

	return 0;
}
