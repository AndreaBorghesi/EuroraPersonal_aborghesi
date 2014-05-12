#ifndef Def_Stats
#define Def_Stats
#include <vector>
#include "JobArray.hpp"
#include "QueueArray.hpp"
#include "Util.hpp"
#include <stdio.h>
#include <climits>
#include <math.h>
using namespace std;

class Stats
{
private:
	static int _preInLate;
	static int _postInLate;
	static int _numPreFirstEnd;
	static int _numPostFirstEnd;
	static int _updateCounter;
	static vector<int> _times;
	static vector<int> _coreVolume;
	static vector<int> _gpuVolume;
	static vector<int> _micVolume;
	static vector<int> _memVolume;
	static vector<int> _nodeVolume;
	static vector<int> _numJobs;
public:
	static void Util(JobArray post,int time)
	{
		
		int cores=0,gpu=0,mic=0,mem=0,nodes=0,jobs=0;
		for(int i=0;i<post.size();i++)
		{
			if(post[i].getStartTime()<=time && post[i].getStartTime()+post[i].getDuration(time)>time)
			{
				cores+=post[i].getNumberOfCores();
				gpu+=post[i].getNumberOfGPU();
				mic+=post[i].getNumberOfMIC();
				mem+=post[i].getMemory();
				nodes+=post[i].getNumberOfNodes();
				jobs+=1;
			}
		}
		_numJobs.push_back(jobs);
		_coreVolume.push_back(cores);
		_gpuVolume.push_back(gpu);
		_micVolume.push_back(mic);
		_memVolume.push_back(mem);
		_nodeVolume.push_back(nodes);
		_times.push_back(time);
		
		_updateCounter++;
	}
	
	static string printUtil()
	{
		stringstream res("");
		for(int i=0;i<_updateCounter;i++)
		{
			//if(_nodeVolume[i]>10)
				res<<_times[i]<<" "<<_nodeVolume[i]<<" "<<_coreVolume[i]<<endl;
		}
		return res.str();
	}
	
	static void Update(JobArray pre,JobArray post,QueueArray queue,int time)
	{
		int maxPre=0;
		int maxPost=0;
		
		if(pre.size() != post.size())
		{
			cout<<pre.size()<<" "<<post.size()<<endl;
			//getchar();
		}
		for(int i=0;i<post.size();i++)
		{
			if(post[i].getStartTime()+post[i].getDuration(time)<=time && post[i].getStartTime()+post[i].getRealDuration()>maxPost)
				maxPost=post[i].getStartTime()+post[i].getRealDuration();
			if(post[i].getStartTime()-post[i].getEnterQueueTime() > queue.getMaxMinutesToWait(post[i].getQueue()))
				_postInLate++;	
		}
		for(int i=0;i<pre.size();i++)
		{
			if(pre[i].getStartTime()+pre[i].getRealDuration()>maxPre)
				maxPre=pre[i].getStartTime()+pre[i].getRealDuration();
			if(pre[i].getStartTime()-pre[i].getEnterQueueTime() > queue.getMaxMinutesToWait(pre[i].getQueue()))
				_preInLate++;
		}
		if(maxPre>maxPost)
			_numPostFirstEnd++;
		else if(maxPre<maxPost)
			_numPreFirstEnd++;
		int cores=0,gpu=0,mic=0,mem=0,nodes=0;
		for(int i=0;i<post.size();i++)
		{
			if(post[i].getStartTime()<=time && post[i].getStartTime()+post[i].getDuration(time)>time)
			{
				cores+=post[i].getNumberOfCores();
				gpu+=post[i].getNumberOfGPU();
				mic+=post[i].getNumberOfMIC();
				mem+=post[i].getMemory();
				nodes+=post[i].getNumberOfNodes();
			}
		}
		_coreVolume.push_back(cores);
		_gpuVolume.push_back(gpu);
		_micVolume.push_back(mic);
		_memVolume.push_back(mem);
		_nodeVolume.push_back(nodes);
		_times.push_back(time);
		
		_updateCounter++;
	}
	
	static string printStats(JobArray original,JobArray result,QueueArray queue,int initialTime )
	{
		stringstream res("");
		int tMeanQueueOriginal=0;
		int tMeanQueueResult=0;
		int maxEndOrig=0,maxEndRes=0;
		_postInLate=0;
		_preInLate=0;
		
		for(int i =0;i<original.size();i++)
		{
			if(original[i].getStartTime()-original[i].getEnterQueueTime() > queue.getMaxMinutesToWait(original[i].getQueue()))
				_preInLate++;
			if(original[i].getStartTime()+original[i].getRealDuration()>maxEndOrig)
			{
				maxEndOrig=original[i].getStartTime()+original[i].getRealDuration();
			}
			tMeanQueueOriginal+=original[i].getStartTime()-original[i].getEnterQueueTime();
		}
		for(int i =0;i<result.size();i++)
		{
			if(result[i].isScheduled())
			{
			if(result[i].getStartTime()-result[i].getEnterQueueTime() > queue.getMaxMinutesToWait(result[i].getQueue()))
				_postInLate++;
			if(result[i].getStartTime()+result[i].getRealDuration()>maxEndRes)
			{
				maxEndRes=result[i].getStartTime()+result[i].getRealDuration();
			}
			if(result[i].getStartTime()-result[i].getEnterQueueTime()<0)
				std::cout<<i<<" start "<<result[i].getStartTime()<<" eqt "<<result[i].getEnterQueueTime()<<" s-eqt "<<result[i].getStartTime()-result[i].getEnterQueueTime()<<std::endl;
			tMeanQueueResult+=result[i].getStartTime()-result[i].getEnterQueueTime();
		}
		}
		_numPreFirstEnd=0;
		_numPostFirstEnd=0;
		for(int i=0;i<original.size();i++)
		{
			for(int j=0;j<result.size();j++)
			{
				if(original[i].getJobId().compare(result[j].getJobId())==0)
				{
					if(original[i].getStartTime()<result[j].getStartTime())
						_numPreFirstEnd++;
					else if(original[i].getStartTime()<result[j].getStartTime())
						_numPostFirstEnd++;
				}
			}
		}
		
		res<<"------------------------------------------------------------"<<endl;
		res<<"Total time in queue (scheduler) "<<tMeanQueueResult<<endl;
		res<<"Total time in queue (PBS) "<<tMeanQueueOriginal<<endl<<endl;
		

		res<<"Mean time in queue (scheduler) "<<(double)tMeanQueueResult/result.size()<<endl;
		res<<"Mean time in queue (PBS) "<<(double)tMeanQueueOriginal/original.size()<<endl<<endl;
		
		res<<"Numbers of Job exceding max queue time (scheduler) "<<_postInLate<<endl;
		res<<"Numbers of Job exceding max queue time (PBS) "<<_preInLate<<endl<<endl;
		
		res<<"Number of anticipated jobs (scheduler) "<<_numPostFirstEnd<<endl;
		res<<"Number of anticipated jobs (PBS) "<<_numPreFirstEnd<<endl<<endl;
		
		res<<"Maximum job end time (scheduler) "<<Util::timeToStr(maxEndRes+initialTime)<<endl;
		res<<"Maximum job end time (PBS) "<<Util::timeToStr(maxEndOrig+initialTime)<<endl<<endl;
		
		res<<"Time istants"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_times[i]<<";";
		}
		res<<endl<<"Used Nodes"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_nodeVolume[i]<<";";
		}
		res<<endl<<"Used Cores"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_coreVolume[i]<<";";
		}
		res<<endl<<"Used GPU"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_gpuVolume[i]<<";";
		}
		res<<endl<<"Used MIC"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_micVolume[i]<<";";
		}
		res<<endl<<"Used Memory"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_memVolume[i]<<";";
		}
		res<<endl;
		res<<"------------------------------------------------------------"<<endl;
		return res.str();
	}
	

	static string printStats(JobArray jobs,QueueArray queue)
	{
		stringstream res("");
		int minTime=INT_MAX,maxTime=0,jobsInLate=0;
		double tQueue=0,tardiness=0,weightTar=0,stdTard=0;
		for(int i=0;i<jobs.size();i++)
		{
			if(jobs[i].getEnterQueueTime()<minTime)
				minTime=jobs[i].getEnterQueueTime();
			if(jobs[i].getStartTime()+jobs[i].getRealDuration()>maxTime)
				maxTime=jobs[i].getStartTime()+jobs[i].getRealDuration();
				
			tQueue+=jobs[i].getStartTime()-jobs[i].getEnterQueueTime();
			if(jobs[i].getStartTime()-jobs[i].getEnterQueueTime()> queue.getMaxMinutesToWait(jobs[i].getQueue()))
			{
				jobsInLate++;
				tardiness+=jobs[i].getStartTime()-jobs[i].getEnterQueueTime()- queue.getMaxMinutesToWait(jobs[i].getQueue());
				weightTar+=(double)((double)jobs[i].getStartTime()-jobs[i].getEnterQueueTime()- queue.getMaxMinutesToWait(jobs[i].getQueue()))/(double)queue.getMaxMinutesToWait(jobs[i].getQueue());
			}
		}
		double meanTard=tardiness/jobsInLate,sum=0;
		for(int i=0;i<jobs.size();i++)
		{
			if(jobs[i].getStartTime()-jobs[i].getEnterQueueTime()> queue.getMaxMinutesToWait(jobs[i].getQueue()))
			{
				sum+=pow(jobs[i].getStartTime()-jobs[i].getEnterQueueTime()- queue.getMaxMinutesToWait(jobs[i].getQueue())-meanTard,2);
			}
		}
		
		stdTard=pow(sum/jobsInLate,0.5);
			
		res<<"Makespam (sec):"<<maxTime-minTime<<endl;
		res<<"Numbers of Job in late :"<<jobsInLate<<endl;
		
		res<<"Total time in queue (sec):"<<tQueue<<endl;
		res<<"Mean time in queue :"<<tQueue/jobs.size()<<endl;
		
		res<<"Tardiness (sec):"<<tardiness<<endl;
		res<<"Weighted tardiness :"<<weightTar<<endl;
		res<<"Tardiness standard deviation :"<<stdTard<<endl;
		
		
		res<<"Time istants"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_times[i]<<";";
		}
		res<<endl<<"Used Nodes"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_nodeVolume[i]<<";";
		}
		res<<endl<<"Used Cores"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_coreVolume[i]<<";";
		}
		res<<endl<<"Used GPU"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_gpuVolume[i]<<";";
		}
		res<<endl<<"Used MIC"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_micVolume[i]<<";";
		}
		res<<endl<<"Used Memory"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_memVolume[i]<<";";
		}
		res<<endl;
		
		res<<endl<<"Number running jobs"<<endl;
		for(int i=0;i<_updateCounter;i++)
		{
			res<<_numJobs[i]<<";";
		}
		res<<endl;
		res<<"------------------------------------------------------------"<<endl;
		return res.str();
	}
	
};

int Stats::_preInLate=0;
int Stats::_postInLate=0;
int Stats::_numPreFirstEnd=0;
int Stats::_numPostFirstEnd=0;
int Stats::_updateCounter=0;
vector<int> Stats::_times;
vector<int> Stats::_coreVolume;
vector<int> Stats::_gpuVolume;
vector<int> Stats::_micVolume;
vector<int> Stats::_memVolume;
vector<int> Stats::_nodeVolume;
vector<int> Stats::_numJobs;

#endif
