// BlockingQueue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

//--------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class BlockingQueue : protected std::queue<T>
{
public:
//	typedef std::queue<T> baseQueue;
	using valueType = typename T;
protected:
	using baseQueue = typename std::queue<T>;
		
public:
	BlockingQueue() : baseQueue() {}
	~BlockingQueue() {}
	
	void push( const valueType& value );
	valueType pop();
private:
	std::mutex m_lock;
	std::condition_variable m_condition;
};

//----------------------------------------------------------------------------------------------------------

template<typename T>
void BlockingQueue<T>::push( const valueType& value )
{
	std::lock_guard<std::mutex> guard( m_lock );
	baseQueue::push( value );
	m_condition.notify_all();
}

//----------------------------------------------------------------------------------------------------------

template<typename T>
typename BlockingQueue<T>::valueType BlockingQueue<T>::pop( )
{
	valueType value = valueType();

	std::unique_lock<std::mutex> uniqueLock( m_lock );
	if ( baseQueue::empty() )
	{
		m_condition.wait(uniqueLock );
	}
	
	value = baseQueue::front();
	baseQueue::pop();

	return value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
	BlockingQueue<int> queue;
	BlockingQueue<float> queueF;

	queue.push( 5 );
	queue.push( 7 );
	queue.push( 37 );

	int x1 = queue.pop();
	int x2 = queue.pop();
	int x3 = queue.pop();

	std::cout << x1 << std::endl;
	std::cout << x2 << std::endl;
	std::cout << x3 << std::endl;
}

