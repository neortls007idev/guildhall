#pragma once
#include <mutex>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Job
{
public:
	Job();
	virtual ~Job();

	virtual void Execute() = 0;
			bool IsLocked();
			void LockMutex();
			void UnlockMutex();
protected:
	int				m_jobID = 0;
	std::mutex		m_mutex;
};

//--------------------------------------------------------------------------------------------------------------------------------------------