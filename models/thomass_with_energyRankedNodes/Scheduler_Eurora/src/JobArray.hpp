#ifndef Def_JobArray
#define Def_JobArray
#include <vector>
#include <climits>
#include "JobReader.hpp"
#include "JobWriter.hpp"

class JobArray
{
private:
	std::vector<Job> _jobs;
	double _makespan;
	int min(int a,int b){return (a<b?a:b);}
public:
	void setMakespan(double mksp){_makespan=mksp;}
	double getMakespan(){return _makespan;}
	JobArray(std::string fJob);
	JobArray(std::vector<Job> j){_jobs=j;}
	Job& operator[] (int x) {return _jobs[x];}
	JobArray(){};
	void push_back(Job j){_jobs.push_back(j);}
	std::vector<int> getJobUtilizzation(int job){return _jobs[job].getUsedNodes();}
	int getJobStartTime(int job){return _jobs[job].getStartTime();}
	int size(){return _jobs.size();}
	void write(std::string fJob,int refTime);
	void write(std::string fJob){write(fJob,0);}
	int findNextIstant(int fromTime)
	{
		int minTime=INT_MAX;
		for(int i=0;i<_jobs.size();i++)
		{
			if(_jobs[i].getEnterQueueTime()>fromTime && _jobs[i].getEnterQueueTime()<minTime)
				minTime=_jobs[i].getEnterQueueTime();
			if(_jobs[i].getStartTime()+min(_jobs[i].getRealDuration(),_jobs[i].getEstimatedDuration())>fromTime && _jobs[i].getStartTime()+min(_jobs[i].getRealDuration(),_jobs[i].getEstimatedDuration())<minTime)
				minTime=_jobs[i].getStartTime()+min(_jobs[i].getRealDuration(),_jobs[i].getEstimatedDuration());
			//if(_jobs[i].getStartTime()+_jobs[i].getEstimatedDuration()>fromTime && _jobs[i].getStartTime()+_jobs[i].getEstimatedDuration()<minTime)
			//	minTime=_jobs[i].getStartTime()+_jobs[i].getEstimatedDuration();
		}
		if(minTime==INT_MAX)
			return -1;
		return minTime;
	}
};

inline JobArray::JobArray(std::string fJob)
{
	JobReader r(fJob);
	_jobs=r.read();
	
}

inline void JobArray::write(std::string fJob,int refTime)
{
	JobWriter r(fJob);
	r.write(_jobs,refTime);
}
#endif
