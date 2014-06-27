#ifndef Def_Job
#define Def_Job
#include <ctime>
#include <vector>
#include <string>
#include "Util.hpp"

using namespace std;
class Job
{
	private:
		 string _id;
		 string _name;
		 string _userName;
		 string _coda;
		 int _enterQueueTime;
		 
		 int _nodes;
		 int _cores;
		 int _gpus;
		 int _mics;
		 int _mem;
		 
		 int _realDuration;
		 int _estimatedDuration;

		 int _start;
		 double _lastMakespan;
		 bool _makespanPresent;x

		 bool _schedulato;
		 std::vector<int> _usedNodes;
		 int min(int a,int b){return (a<b?a:b);}
		 
	 public:
		 bool isScheduled(){return _schedulato;}
		 void setScheduled(bool s){_schedulato=s;}
		 double getLastMakespan(){return _lastMakespan;}
		 bool isMakespanPresent(){return _makespanPresent;}
		 void setLastMakespan(double lmks){_lastMakespan=lmks;_makespanPresent=true;}
		 string getJobId(){return _id;}
		 string getJobName(){return _name;}
		 string getUserName(){return _userName;}
		 int getEnterQueueTime(){return _enterQueueTime;}
		 string getQueue(){return _coda;}
		 int getNumberOfNodes(){return _nodes;}
		 int getNumberOfCores(){return _cores;}
		 int getNumberOfGPU(){return _gpus;}
		 int getNumberOfMIC(){return _mics;}
		 int getMemory(){return _mem;}
		 int getEstimatedDuration(){return _estimatedDuration;}
		 int getRealDuration(){return _realDuration;}
		 int getStartTime(){return _start;}
		 std::vector<int> getUsedNodes(){return _usedNodes; }
		 int getDuration(int atTime)
		 {
			 if(getStartTime()+min(getEstimatedDuration(),getRealDuration())<= atTime)
			 {
				 return min(getEstimatedDuration(),getRealDuration());
			 }
			 return getEstimatedDuration();
		 }
		 
		 /*bool isAlreadyStarted(int atTime){
			 if(getStartTime()<atTime)
				 return true;
			 return false;
		 }
		 bool isAlreadyFinished(int atTime)
		 {
			 if(getStartTime()+min(getRealDuration(),getEstimatedDuration())<=atTime)
				 return true;
			 return false;
		 }*/
		 
		 
		 
		 void setJobId(string id){ _id=id;}
		 void setJobName(string name){ _name=name;}
		 void setUserName(string userName){_userName=userName;}
		 void setQueue(string queue){ _coda=queue;}
		 void setEnterQueueTime(int eqt){_enterQueueTime=eqt;}
		 
		 void setNumberOfNodes(int n){ _nodes=n;}
		 void setNumberOfCores(int n){ _cores=n;}
		 void setNumberOfGPU(int n){ _gpus=n;}
		 void setNumberOfMIC(int n){ _mics=n;}
		 void setMemory(int m){ _mem=m;}
		 void setEstimatedDuration(int d){_estimatedDuration=d;}
		 void setRealDuration(int d){
			 _realDuration=d;}
			 void setStartTime(int start){_start=start;}
		 
		 void allocate(int s, std::vector<int> nodes){_start=s, _usedNodes=nodes;}
		 inline Job(string id,string name,string userName,string queue,int eqt, int nodes,int cores,int gpu,int mic,int mem,int estDur,int realDur,int start,std::vector<int> used){
			 
			 setJobId(id);
			 setJobName(name);
			 setUserName(userName);
			 setQueue(queue);
			 setEnterQueueTime(eqt);
			 setNumberOfNodes(nodes);
			 setNumberOfCores(cores);
			 setNumberOfGPU(gpu);
			 setNumberOfMIC(mic);
			 setMemory(mem);
			 setEstimatedDuration(estDur);
			 setRealDuration(realDur);
			 allocate(start,used);
			 _makespanPresent=false;
			 _lastMakespan=-1;
			 _schedulato=false;
		 }
		 
		 string toString(int now)
		 {
			 stringstream s("");
			 if(isScheduled()==false)
				 s<<"N ";
 			s<<getJobId()<<" "<< getEnterQueueTime()<<" ["<<getStartTime()<<" : "<<Util::timeToStr(getStartTime())<<"->"<<getDuration(now)<<"->"<<Util::timeToStr(getDuration(now)+getStartTime())<<"] [";
			for(int j=0;j<getUsedNodes().size();j++)
			{
				s<<getUsedNodes()[j];
			}
			s<<"] ";
			
			/*for(int j=0;j<getUsedNodes().size();j++)
			{
				if(getUsedNodes()[j]==1)
					s<<j<<" ";
				else if(getUsedNodes()[j]>1)
					s<<j<<"*"<<getUsedNodes()[j]<<" ";
			}*/
			
			return s.str();
		 }
};
#endif
