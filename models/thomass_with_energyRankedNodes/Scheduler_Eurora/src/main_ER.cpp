#include <stdio.h>
#include <iostream>
#include "Stats.hpp"
#include <ctime>
#include "SchedulerBase.hpp"

using namespace std;
int main(int argc, const char * argv[]) 
{
	if (argc < 3) 
	{
		cout << "USAGE: " << argv[0] << " <problem file>" << endl;
		exit(1);
	}
	
	stringstream directory("");
	directory<<"log/"<<Util::split(Util::split(argv[3],"/")[1],".")[0]<<"/";
	mkdir(directory.str().c_str(),S_IRWXU|S_IRWXG|S_IRWXO);
	string fQueue(argv[1]);
	string fNodes(argv[2]);
	string fJobs(argv[3]);

	//bad, bad practice
	string nfFile = "data/NFreq_CPU_Bound.logprocessed";
	string rfMFile = "data/RFreq_Mem_Bound.logprocessed";
	string rfCFile = "data/RFreq_CPU_Bound.logprocessed";
	string eMFile = "data/Energy_Mem_Bound.logprocessed";
	string eCFile = "data/Energy_CPU_Bound.logprocessed";
	
	//cout<<"Before SchedulerBase creation "<<endl;

	SchedulerBase<Model_ER> s(fQueue,fNodes,nfFile,rfMFile,rfCFile,eMFile,eCFile,fJobs,directory.str());
	
	//cout<<"After SchedulerBase creation "<<endl;
	
	cout<<Util::timeToStr(s.getInitialTime())<<endl;
	
	stringstream dest;
	JobArray r;
	int precNow;
	time_t now=0;
	while((now=s.findNextIstant(now))!=-1)
	{

		cout<<"Istante: "<<now<<endl;
		dest.str("");
		r=s.Solve(now);
		/*for(int i=0;i<r.size();i++)
		{
				cout<<f<<" "<<r[i].toString(now)<<endl;
		}*/
		
		precNow=now;

		//break;
	}
	
	for(int i=0;i<r.size();i++)
	{
		//if(r[i].isScheduled()==false)	
		cout<<r[i].toString(precNow)<<endl;
		
	}
	
	//cout<<Util::timeToStr(s.getInitialTime())<<endl;
	//getchar();
	stringstream file("");
	file<<directory.str();
	
	file<<"Energy_Rank_Model"<<".log";
	r.write(file.str(),s.getInitialTime());
	
	/*string statString=s.getStats();
	stringstream file("");
	file<<directory.str();
	file<<"Stats_Makespan"<<typeid(*s.getModel()).name()<<".log";
	ofstream out(file.str().c_str());
	out << statString;
	out.close();
	cout<<statString<<endl;*/
	return 0;
}
