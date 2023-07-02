#include "taskthread.hpp"


namespace RenderTaskSolver
{
	ThreadIdType TaskThreadManager::AddThread(TaskThread thr)
	{
		ThreadIdType thr_id = thr->get_id();
		Threads.insert({ thr_id, std::move(thr) });
		return thr_id;
	}

	void TaskThreadManager::Join()
	{
		Threads.clear();
	}

	void TaskThreadManager::WaitFor(ThreadIdType thrd_id)
	{
		Threads.at(thrd_id).reset();
	}
}
