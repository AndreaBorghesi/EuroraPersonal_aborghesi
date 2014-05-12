#ifndef Def_SchedulerBase
#define Def_SchedulerBase
#include "IModel.hpp"
#include "Model_MKS.hpp"
#include "Model_WT.hpp"
#include "Model_NL.hpp"
#include "Model_ER.hpp"
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "NodeEnergyUpdater.hpp"
#include "JobArray.hpp"
#include "Checker.hpp"
#include "Stats.hpp"
#include <string>    
#include <stdlib.h>
#include <sstream>
#include <sys/stat.h>


template <class ModelType>class SchedulerBase
{
private:
	QueueArray _queues;
	NodeArray _nodes;
	JobArray _jobs;
	JobArray _original;
	IModel * _model;
	std::string _fJob;
	int _numIteration;
	string _directory;
	int _initialTime;
	
	JobArray getOriginal(JobArray res)
	{
		std::vector<Job> sel;
		for(int i=0;i<res.size();i++)
		{
			for(int j=0;j<_original.size();j++)
			{
				if(res[i].getJobId().compare(_original[j].getJobId()))
				{
					sel.push_back(_original[j]);
					break;
				}
			}
		}
		
		return JobArray(sel);
	}
	

		
public:
	int getInitialTime(){return _initialTime;}
	JobArray getOriginal()
	{
		return _original;
	}
	JobArray getResult()
	{
		return _jobs;
	}
	IModel * getModel(){return _model;}
	SchedulerBase(std::string fQueue,std::string fNode,std::string nfFile,std::string rfMFile,std::string rfCFile,std::string eMFile,std::string eCFile,std::string fJob,string directory){

		//std::cout<<"Inside SchedulerBase constructor"<<std::endl; 
		
		_queues=QueueArray(fQueue);
		_nodes=NodeArray(fNode);  
		
		//std::cout<<"Before NodeEnergyUpdater "<<std::endl; 
		
		NodeEnergyUpdater u(nfFile,rfMFile,rfCFile,eMFile,eCFile);
		
		//std::cout<<"Before NodeEnergyUpdater "<<std::endl; 
		//std::cout<<"After NodeEnergyUpdater update()"<<std::endl; 
		u.setup(_nodes);
		u.update(_nodes);
		//std::cout<<"After NodeEnergyUpdater update()"<<std::endl; 
		//test if energy info correctly read
		
		//std::cout<<"Before test"<<std::endl; 
		//std::cout<<_nodes.size()<<std::endl;
		
		for(int i=0;i<_nodes.size();i++)
			std::cout<<" ---- "<<_nodes[i].getNodeNumber()<<";"<<_nodes[i].getNominalFrequency()<<";"<<_nodes[i].getRealMEMFrequency()<<";"<<_nodes[i].getRealCPUFrequency()<<";"<<_nodes[i].getEnergyMEM()<<";"<<_nodes[i].getEnergyCPU()<<" ---- "<<std::endl;
			
		_jobs=JobArray(fJob);
		_model=new ModelType(_queues,_nodes,_jobs);
		_fJob=fJob;
		_original=JobArray(fJob);
		_numIteration=0;
		_directory=directory;
		// sottraggo tempi
		int minStart=INT_MAX;
		for(int i=0;i<_jobs.size();i++)
		{
			if(_jobs[i].getEnterQueueTime()<minStart)
			{
				std::cout<<i<<" "<<_jobs[i].toString(0)<<std::endl;
				minStart=_jobs[i].getEnterQueueTime();
			}
		}
		
		int speed=1;
		minStart-=1*speed;
		for(int i=0;i<_jobs.size();i++)
		{
			_jobs[i].setStartTime((_jobs[i].getStartTime()-minStart));
			_jobs[i].setEnterQueueTime((_jobs[i].getEnterQueueTime()-minStart)/speed);
			//	_jobs[i].setEnterQueueTime(1);
			//	_jobs[i].setStartTime(2);
			//	_jobs[i].setEstimatedDuration(_jobs[i].getRealDuration());
			_original[i].setStartTime((_original[i].getStartTime()-minStart));
			_original[i].setEnterQueueTime((_original[i].getEnterQueueTime()-minStart)/speed);
			//	_original[i].setEnterQueueTime(1);
			//	_original[i].setStartTime(2);
			//	_original[i].setEstimatedDuration(_original[i].getRealDuration());
		}
		_initialTime=minStart;
	}
	int findNextIstant(int fromTime)
	{
		return _jobs.findNextIstant(fromTime);
	}

	JobArray Solve(int refTime){
		std::vector<Job> sel;
		for(int i=0;i<_jobs.size();i++)
		{
			if(_jobs[i].getRealDuration()+_jobs[i].getStartTime()>refTime && _jobs[i].getEnterQueueTime()<=refTime)
				sel.push_back(_jobs[i]);
		}

		_model->setJobArray(sel);
		JobArray res=_model->solve(refTime);

		int err;
		if((err=Checker::CheckJobArray(res,_nodes))<=0)
		{
			for(int i=0; i<res.size();i++)
			{
				std::cout<<res[i].toString(refTime)<<std::endl;
			}
			std::cout<<"Solution Error: Not respected constraints in model result - "<<err<<std::endl;
			//exit(0);
			//getchar();
			/*stringstream file("");
			file<<_directory;
		
			file<<"Errore "<<-err<<" "<<typeid(_model).name()<<"_"<<_numIteration<<"_"<<refTime<<".log";
			res.write(file.str(),0);*/
			exit(0);
		}

		Stats::Update(getOriginal(res),res,_queues,refTime);
		for(int i=0;i<_jobs.size();i++)
		{
			for(int j=0;j<res.size();j++)
			{
				if(res[j].getJobId().compare(_jobs[i].getJobId())==0)
				{
					_jobs[i].allocate(res[j].getStartTime(),res[j].getUsedNodes());
					_jobs[i].setRealDuration(res[j].getRealDuration());
					_jobs[i].setLastMakespan(res.getMakespan());
					if(res[j].getStartTime()<=refTime)
						_jobs[i].setScheduled(true);
				}
			}
		}

		/*stringstream file("");
		file<<_directory;
		
		file<<typeid(_model).name()<<".log";
		res.write(file.str(),_initialTime);*/
		
		_numIteration++;

		return _jobs;
	}
	
	string getStats()
	{
		return Stats::printStats(_original,_jobs,_queues,_initialTime);
	}
	
	void saveJobArray(string file)
	{
		_jobs.write(file);
	}
	
};

 
#endif
