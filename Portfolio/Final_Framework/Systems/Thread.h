#pragma once
//#include "ISubsystem.h"

class Task final
{
public:
	typedef std::function<void()> Process;

public:
	Task(Process&& process)
	{
		this->process = std::forward<Process>(process);
	}
	~Task() = default;

	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

	void Execute()
	{
		process();
	}

private:
	Process process;
};

class Thread //final : public ISubsystem
{
public:
	static Thread* Get();

	static void Create();
	static void Delete();
	
	void Invoke();

	template <typename Process>
	void AddTask(Process&& process);

private:
	Thread();
	~Thread();

	static Thread* instance;

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	//const bool Initialize() override;
	const bool Init();



private:
	std::vector<std::thread> threads;
	std::queue<std::shared_ptr<Task>> tasks;
	std::mutex taskMutex;
	std::condition_variable conditionVar;
	UINT threadCount;
	bool bStopping;
};

template<typename Process>
inline void Thread::AddTask(Process && process)
{
	if (threads.empty())
	{
		//LOG_WARNING("Thread::AddTask: No available threads");
		std::cout << "Thread::AddTask: No available threads" << std::endl;
		process();
		return;
	}

	std::unique_lock<std::mutex> lock(taskMutex);

	tasks.push(std::make_shared<Task>(std::bind(std::forward<Process>(process))));

	lock.unlock();

	conditionVar.notify_one();
}
