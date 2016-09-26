#pragma once

#include "CoreInterface.h"
#include "Thread.h"

#include <queue>

static unsigned int taskcount = 0;

namespace Amendux {

	struct Task
	{
		std::string name;
		std::map<std::wstring, std::wstring> params;

		Task(std::string n) {
			name = n;
			tid = taskcount++;
		}

		Task(std::wstring wn) {
			std::string n(wn.begin(), wn.end());
			name = n;
			tid = taskcount++;
		}

		unsigned int GetId() {
			return tid;
		}

	private:
		unsigned int tid;
	};

	class TaskQueue : CoreInterface
	{
		static TaskQueue *s_TaskQueue;
		std::queue<Task> mainqueue;
		
		DWORD Worker();
		void InitClass();

	public:
		TaskQueue();
		~TaskQueue();

		void Add(const std::string& mod);

		void Add(Task& task);

		size_t Size() {
			return mainqueue.size();
		}

		bool Empty() {
			return mainqueue.empty();
		}

		static void SpawnTaskWorker() {
			//if (!Config::Current()->CanConnect()) {
			//	return;
			//}

			Thread<TaskQueue> *thread = new Thread<TaskQueue>(TaskQueue::Current(), &TaskQueue::Worker);
			if (!thread->Start()) {
				Log::Error(L"TaskQueue", L"Cannot spawn worker process");
			}
		}

		static TaskQueue *Current() {
			if (!s_TaskQueue) {
				s_TaskQueue = new TaskQueue;
			}

			return s_TaskQueue;
		}

		static void Init() {
			if (!s_TaskQueue) {
				s_TaskQueue = new TaskQueue;
			}

			s_TaskQueue->InitClass();
		}

		static void Terminate() {
			if (s_TaskQueue) {
				delete s_TaskQueue;
				s_TaskQueue = nullptr;
			}
		}
	};

}
