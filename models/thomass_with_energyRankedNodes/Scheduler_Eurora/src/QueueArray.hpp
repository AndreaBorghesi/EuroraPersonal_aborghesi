#ifndef Def_QueueArray
#define Def_QueueArray
#include <vector>
#include <string>
#include "QueueReader.hpp"

class QueueArray
{
	private:
		std::vector<Queue> _queue;
	public:
		int getMaxMinutesToWait(std::string id);
		QueueArray(std::string fQueue);
		QueueArray(){};
		int size(){return _queue.size();}
};

inline QueueArray::QueueArray(std::string fQueue)
{
	QueueReader r(fQueue);
	_queue=r.read();
}

inline int QueueArray::getMaxMinutesToWait(std::string id)
{
	for(int i=0;i<_queue.size();i++)
		if(_queue[i].getId().compare(id)==0)
			return _queue[i].getMaxMinutesToWait();
	for(int i=0;i<_queue.size();i++)
		if(_queue[i].getId().compare("reservation")==0)
			return _queue[i].getMaxMinutesToWait();
	return 0;
}
#endif
