#include "Framework.h"
#include "Thread.h"

Thread* Thread::instance = NULL;

Thread* Thread::Get()
{
	return instance;
}

void Thread::Create()
{
	assert(instance == NULL);

	instance = Debug_new Thread();
}

void Thread::Delete()
{
	SAFE_DELETE(instance);
}

Thread::Thread()
	:bStopping(false)
{
	threadCount = std::thread::hardware_concurrency() - 1;

	Init();
}

Thread::~Thread()
{
	std::unique_lock<std::mutex> lock(taskMutex);

	bStopping = true;

	lock.unlock();

	conditionVar.notify_all();

	for (auto& thread : threads)
		thread.join();

	threads.clear();
	threads.shrink_to_fit();
}

const bool Thread::Init()
{
	for (UINT i = 0; i < threadCount; i++)
		threads.emplace_back(std::thread(&Thread::Invoke, this));

	return true;
}

void Thread::Invoke()
{
	std::shared_ptr<Task> task;

	while (true)
	{
		std::unique_lock<std::mutex> lock(taskMutex);

		conditionVar.wait(lock, [this]() { return !tasks.empty() || bStopping; });

		if (bStopping && tasks.empty())
			return;

		task = tasks.front();

		tasks.pop();

		lock.unlock();

		task->Execute();
	}
}
