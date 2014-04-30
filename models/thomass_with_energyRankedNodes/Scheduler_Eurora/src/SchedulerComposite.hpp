#ifndef Def_Scheduler2
#define Def_Scheduler2
#include "IModel.hpp"
#include "IModelAdvanced.hpp"
#include "Model0.hpp"
#include "Model1.hpp"
#include "Model2.hpp"
//#include "Model3.hpp"
//#include "Model4.hpp"
#include "Model5.hpp"
//#include "Model6.hpp"
#include "Model7.hpp"
#include "Model8.hpp"
#include "Model9.hpp"
#include "Model10.hpp"
//#include "Model11.hpp"
//#include "Model12.hpp"
#include "Model13.hpp"
#include "Model14.hpp"
#include "Model15.hpp"
#include "Model16.hpp"
#include "Model17.hpp"
#include "Model18.hpp"
#include "Model19.hpp"
#include "Model20.hpp"
#include "Model21.hpp"
#include "Model22.hpp"
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"
#include "Checker.hpp"
#include "Stats.hpp"
#include <string>    
#include <stdlib.h>
#include <sstream>
#include <sys/stat.h>


template <class ModelType>class Scheduler2
{
private:
	QueueArray _queues;
	NodeArray _nodes;
	JobArray _jobs;
	JobArray _original;
	IModelAdvanced * _model;
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
	Scheduler2(std::string fQueue,std::string fNode,std::string fJob,string directory){

		_queues=QueueArray(fQueue);
		_nodes=NodeArray(fNode);
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
				minStart=_jobs[i].getEnterQueueTime();
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
	
	JobArray Solve(int refTime,double wtDelta,  double numLateDelta,double makespanDelta){

		if(wtDelta>0)
		{
			Model21 mwt(_queues,_nodes,_jobs);
			Solve(refTime,&mwt);
			std::cout<<"risolta parte wei tard con sol:"<<mwt.getOptimalSolution()<<std::endl;
			_model->setWT(mwt.getOptimalSolution(),wtDelta);
		}
		else
			_model->setWT(-1,wtDelta);

		if(numLateDelta>0)
		{
			Model22 nl(_queues,_nodes,_jobs);
			Solve(refTime,&nl);
			std::cout<<"risolta parte num late con sol:"<<nl.getOptimalSolution()<<std::endl;
			_model->setNumLate(nl.getOptimalSolution(),numLateDelta);
		}
		else
			_model->setNumLate(-1,numLateDelta);
		
		if(makespanDelta>0)
		{
			Model20 mk(_queues,_nodes,_jobs);
			Solve(refTime,&mk);
			std::cout<<"risolta parte makesp con sol:"<<mk.getOptimalSolution()<<std::endl;
			_model->setMK(mk.getOptimalSolution(),makespanDelta);
		}
		else
			_model->setMK(-1,makespanDelta);
		
		return Solve(refTime);	
	}
	
	JobArray Solve(int refTime)
	{
		return Solve(refTime,_model);
	}

	JobArray Solve(int refTime, IModel * model){
		std::vector<Job> sel;
		for(int i=0;i<_jobs.size();i++)
		{
			if(_jobs[i].getRealDuration()+_jobs[i].getStartTime()>refTime && _jobs[i].getEnterQueueTime()<=refTime)
				sel.push_back(_jobs[i]);
		}

		model->setJobArray(sel);
		JobArray res=model->solve(refTime);

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
			stringstream file("");
			file<<_directory;
		
			file<<"Errore "<<-err<<" "<<typeid(model).name()<<"_"<<_numIteration<<"_"<<refTime<<".log";
			res.write(file.str(),0);
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