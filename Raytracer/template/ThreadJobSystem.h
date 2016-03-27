#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <queue>
#include <list>

class ThreadJobSystem;


class Thread
{
public:
	Thread(int _id, ThreadJobSystem* _newJobCallBack);
	Thread(const Thread&) = delete;
	Thread(Thread&& t)
	{
		m_job = t.m_job;
		id = t.id;
		m_controller = t.m_controller;
		m_thread = t.m_thread;
		m_programActive = t.m_programActive;

		t.m_job = nullptr;
		t.id = 0;
		t.m_controller = nullptr;
		t.m_thread = nullptr;
		t.m_programActive = 0;
	}

	Thread& operator=(const Thread&) = delete;
	Thread& operator=(Thread&& t)
	{
		m_job = t.m_job;
		id = t.id;
		m_controller = t.m_controller;
		m_thread = t.m_thread;
		m_programActive = t.m_programActive;

		t.m_job = nullptr;
		t.id = 0;
		t.m_controller = nullptr;
		t.m_thread = nullptr;
		t.m_programActive = 0;
		return *this;
	}

	void mainLoop();
	std::function<void()> m_job;
	std::mutex m_jobMutex;
	int id;
	ThreadJobSystem* m_controller;
	std::thread* m_thread;
	bool m_programActive;
};


class ThreadJobSystem
{
public:
	ThreadJobSystem(int _numOfThreads);
	ThreadJobSystem()
	{
	}
	~ThreadJobSystem();
	bool CanSync();
	void QueueJob(std::function<void()> _jobToQueue);
	void WaitForJobsToFinish();
	std::function<void()> TryDeQueueJob();
	std::mutex m_managerMutex;
private:

	std::queue<std::function<void()>> m_jobQueue;
	//std::vector<Thread> m_threads;
	std::vector<Thread*> m_threads;
};

