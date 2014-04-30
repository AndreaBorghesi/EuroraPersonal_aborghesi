#include "Scheduler.hpp"
#include "Scheduler2.hpp"
#include <stdio.h>
#include <iostream>
#include "Stats.hpp"
#include <ctime>

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
	Scheduler2<Model20> s(fQueue,fNodes,fJobs,directory.str());
	
	JobArray original=s.getOriginal();
	
	
	stringstream dest;
	JobArray r;
	int precNow;
	time_t now=0;
	while((now=s.findNextIstant(now))!=-1)
	{
		bool continua=false;
		cout<<"Istante: "<<now<<endl;
		dest.str("");
		r=s.Solve(now,2,2,-1);
		/*for(int i=0;i<r.size();i++)
		{
				cout<<f<<" "<<r[i].toString(now)<<endl;
		}*/
		
		precNow=now;
		for(int i=0;i<original.size();i++)
		{
			//cout<<original[i].getEnterQueueTime()<<endl;
			if(original[i].getEnterQueueTime()>now)
				continua=true;
		}
		if(!continua)
			break;
	}
	
	for(int i=0;i<r.size();i++)
	{
		//if(r[i].isScheduled()==false)	
		cout<<r[i].toString(precNow)<<endl;
		
	}
	stringstream file("");
	file<<directory.str();
	
	file<<"Makespan_2WT_2NL_Model23"<<".log";
	r.write(file.str(),s.getInitialTime());
	/*
	string statString=s.getStats();
	stringstream file("");
	file<<directory.str();
	file<<"Stats_num_late_2WT"<<typeid(*s.getModel()).name()<<".log";
	ofstream out(file.str().c_str());
	out << statString;
	out.close();
	cout<<statString<<endl;*/
	return 0;
}
