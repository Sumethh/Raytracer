#include "ThreadJobSystem.h"

Thread::Thread(int _id, ThreadJobSystem* _controller) :
id(_id), m_controller(_controller), m_programActive(true),
m_job(nullptr)
{
	//id = _id;
	//m_controller = _controller;
	//m_programActive = true;
	//m_job = nullptr;
	printf("ID %d\n", id);
	m_thread = new std::thread(&Thread::mainLoop, this);

}

void Thread::mainLoop()
{
	assert(m_job == nullptr);
	while (m_programActive)
	{
		for (;;)
		{
			//printf("HI %d\t%d\n", id, std::this_thread::get_id());
			if (m_job != nullptr)
				break;
			std::this_thread::yield();
			//printf("YIELDED\n");
		}
		//printf("GONNA RUN %d\t%d\n", id, std::this_thread::get_id());
		std::lock_guard<std::mutex> lock(m_jobMutex);
		m_job();
		//printf("DONE\n");
		m_job = m_controller->TryDeQueueJob();
	}
}

ThreadJobSystem::ThreadJobSystem(int _numOfThreads)
{
	m_threads.reserve(_numOfThreads);
	for (int i = 0; i < _numOfThreads; i++)
		m_threads.push_back(new Thread(i, this));
	printf("%d\n", m_threads.size());

}

ThreadJobSystem::~ThreadJobSystem()
{
	for (int i = 0; i < m_threads.size(); i++)
	{
		m_threads[i]->m_programActive = false;
	}
}

void ThreadJobSystem::QueueJob(std::function<void()> _jobToQueue)
{

	std::lock_guard<std::mutex> lock(m_managerMutex);
	for (int i = 0; i < m_threads.size(); i++)
	{
		if (m_threads[i]->m_job == nullptr)
		{
			std::lock_guard<std::mutex> lock(m_threads[i]->m_jobMutex);
			m_threads[i]->m_job = _jobToQueue;
			return;
		}
	}
	m_jobQueue.push(_jobToQueue);
	//printf("%d\n", m_jobQueue.size());

}

std::function<void()> ThreadJobSystem::TryDeQueueJob()
{
	std::lock_guard<std::mutex> lock(m_managerMutex);
	//m_managerMutex.lock();
	std::function<void()> returningFunction = nullptr;
	if (!m_jobQueue.empty())
	{
		//printf("%d\n", m_jobQueue.size());
		returningFunction = m_jobQueue.front();
		m_jobQueue.pop();
		//printf("HI\n");
	}
	//m_managerMutex.unlock();
	return returningFunction;
}

bool ThreadJobSystem::CanSync()
{
	if (m_jobQueue.empty())
	{
		for (int i = 0; i < m_threads.size(); i++)
		{
			if (m_threads[i]->m_job != nullptr)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void ThreadJobSystem::WaitForJobsToFinish()
{

	while (!CanSync())
	{
		//printf("WAITING\n");
	}
	////printf("%d\n", m_jobQueue.size());
	//for (;;)
	//{
	//	std::lock_guard<std::mutex> lock(m_managerMutex);
	//	//printf("%d\n", m_jobQueue.size());
	//	if (!m_jobQueue.empty())
	//		continue;

	//	for (int i = 0; i < m_threads.size(); i++)
	//		if (m_threads[i]->m_job != nullptr)
	//			continue;

	//	return;
	//}
}