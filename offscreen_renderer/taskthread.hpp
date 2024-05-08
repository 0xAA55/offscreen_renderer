#pragma once

#include<set>
#include<unordered_map>
#include<memory>
#include<thread>

namespace RenderTaskSolver
{
	using TaskThread = std::unique_ptr<std::jthread>;
	using ThreadIdType = std::jthread::id;

	class TaskThreadManager
	{
	protected:
		std::unordered_map<ThreadIdType, TaskThread> Threads;

	public:

		ThreadIdType AddThread(TaskThread thrd);
		void Join();
		void WaitFor(ThreadIdType thrd_id);
	};
}
