#ifndef Def_IModel
#define Def_IModel
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"

class IModel
{
	protected:
		QueueArray _queue;
		NodeArray _nodes;
		JobArray _jobs;
	public:
		virtual JobArray solve(int refTime)=0;
		void setJobArray(JobArray j){_jobs=j;}
	
};
#endif
