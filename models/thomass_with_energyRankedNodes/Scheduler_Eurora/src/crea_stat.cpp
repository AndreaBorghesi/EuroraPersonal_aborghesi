#include "SchedulerBase.hpp"
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
	time_t ora;
	time(&ora);
	/*cout<<ora<<" "<<Util::timeToStr(ora)<<" "<<Util::strToTime(Util::timeToStr(ora))<<" "<<Util::timeToStr(Util::strToTime(Util::timeToStr(ora)))<<endl;
	string data="2014-03-27 14:00:00";
	int d=Util::strToTime(data);
	cout<<data<<" "<<d<<" "<<Util::timeToStr(d)<<" "<<Util::strToTime(Util::timeToStr(d))<<" "<<Util::timeToStr(Util::strToTime(Util::timeToStr(d)))<<" "<<Util::strToTime(Util::timeToStr(Util::strToTime(Util::timeToStr(d))))<<endl;
	int a=1395925200;
	
	cout<<a<<" "<<Util::timeToStr(a)<<endl;
	
	string b="2014-03-27 14:00:00";
	cout<<b<<" "<<Util::strToTime(b)<<endl;*/
	stringstream directory("");
	directory<<"log/";
	mkdir(directory.str().c_str(),S_IRWXU|S_IRWXG|S_IRWXO);
	string fQueue(argv[1]);
	string fNodes(argv[2]);
	string fJobs(argv[3]);
	QueueArray queue(fQueue);
	NodeArray node(fNodes);
	JobArray jobs(fJobs);
	
	int t=0;
	while((t=jobs.findNextIstant(t))>=0)
	{
		Stats::Util(jobs,t);
	}
	string statString=Stats::printStats(jobs,queue);
	cout<<statString<<endl;
	stringstream file("");
	file<<Util::split(argv[3],"/")[0]<<"/"<<Util::split(argv[3],"/")[1]<<"/Stats_"<<Util::split(Util::split(argv[3],"/")[2],".")[0]<<"_"<<ora<<".log";
	cout<<file.str()<<endl;
	ofstream out(file.str().c_str());
	out << statString;
	out.close();
	
	return 0;
}
