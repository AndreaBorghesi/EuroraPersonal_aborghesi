#ifndef Def_JobReader
#define Def_JobReader
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "Job.hpp"
#include "Util.hpp"

class JobReader
{
private:
	std::string _file;
public:
	JobReader(std::string file);
	std::vector<Job> read();
};

inline JobReader::JobReader(std::string file)
{
	_file=file;
}

inline std::vector<Job> JobReader::read()
{
	std::ifstream input(_file.c_str());
	if (!input) 
	{
		std::cout << "ERROR: Could not open file \"" << _file << "\"" << std::endl;
		exit(1);
	}
	
	std::vector<Job> job;
	string line;
	int letti=0;
	while ( getline (input,line) )
	{
		string id,name,userName,coda;
		/*std::cout<<line<<std::endl;
		std::cout<<"qui0"<<std::endl;*/
		int enterQueueTime,start,nodes=0,cores=0,gpu=0,mic=0,mem=0,estimatedDuration,realDuration;
		std::vector<int> usedNodes(64,0);
		bool ready=true;
		std::vector<string> terzi=Util::split(line,"__");
		std::vector<string> data1=Util::split(terzi[0],";");

		
		//std::cout<<"qui01"<<std::endl;
		enterQueueTime = Util::strToTime(data1[4]);
		//for(int i=0;i<terzi.size();i++)
		//	std::cout<<terzi[i]<<std::endl;
		std::vector<string> data3=Util::split(terzi[2],";");

		//std::cout<<"qui012"<<std::endl;
		bool gpuMicPresenti=false;

		//std::cout<<"qui013"<<std::endl;
		start = Util::strToTime(data3[0]);

		//std::cout<<"qui014"<<std::endl;
		realDuration = Util::strToTime(data3[1])-start;
		//std::cout<<"qui02"<<std::endl;
		if(data3[2].compare("--")!=0)
			nodes=std::atoi(data3[2].c_str());
		else
			nodes=-1;
		if(data3[3].compare("--")!=0)
			cores=std::atoi(data3[3].c_str());
		else
		{
			ready=false;
		}
		//std::cout<<"qui1"<<std::endl;
		/*if(data3.size()==9)
		{
		if(data3[4].compare("--")!=0)
		gpu=std::atoi(data3[4].c_str());
		else
		{
		ready=false;
		}
		if(data3[5].compare("--")!=0)
		mic=std::atoi(data3[5].c_str());
		else 
		{
		ready=false;
		}
		if(data3[6].compare("--")!=0)
		{
		int mul=1;
		if(data3[6][data3[6].size()-2]=='g')
		mul=1024*1024;
		else if (data3[6][data3[6].size()-2]=='m')
		mul=1024;
		mem=std::atoi(data3[6].c_str())*mul;
		}
		else
		{
		ready=false;
		}
		estimatedDuration = Util::strTimeToTime(data3[7]);
		gpuMicPresenti=true;
		}*/
		//else
		//{
		if(data3[4].compare("--")!=0)
		{
			int mul=1;
			if(data3[4][data3[4].size()-2]=='g')
				mul=1024*1024;
			else if (data3[4][data3[4].size()-2]=='m')
				mul=1024;
			mem=std::atoi(data3[4].c_str())*mul;
		}
		else 
		{
			ready=false;
		}
		estimatedDuration = Util::strHHMMToTime(data3[5]);
		//std::cout<<data3[5]<<" "<<estimatedDuration<<std::endl;
		//getchar();
		//}
		//std::cout<<"qui2"<<std::endl;
		std::vector<string> data2=Util::split(terzi[1],"#");
		if(nodes==-1)
			nodes=data2.size();

		if(!ready || !gpuMicPresenti)
		{
			nodes=data2.size(),cores=0,gpu=0,mic=0,mem=0;
			for(int i=0;i<data2.size();i++)
			{
				if(data2[i].size()>0)
				{
					std::vector<string> sottoInfo=Util::split(data2[i],";");
					//int index=std::atoi(sottoInfo[0].c_str())-1;
					//usedNodes[index]=1;
			
					//if(cores==-1)
					cores+=std::atoi(sottoInfo[1].c_str());
					//if(!gpuMicPresenti || mic==-1 || gpu==-1)
					//{
					gpu+=std::atoi(sottoInfo[2].c_str());
					mic+=std::atoi(sottoInfo[3].c_str());
					//}
					//if(mem==-1)
					mem+=std::atoi(sottoInfo[4].c_str());
				}
			}
		}
		//std::cout<<"qui3"<<std::endl;
		if(estimatedDuration==0)
			estimatedDuration=((realDuration/60)+1)*60;
		if(realDuration==0)
			realDuration=1;
		id=data1[0];
		name=data1[1];
		userName=data1[2];
		coda=data1[3];
		if(nodes==0)
			nodes=1;
		if(cores==0)
			cores=4;
		if(mem==0)
			mem=200*1024;
		//std::cout<<"qui4"<<std::endl;
		if(cores/nodes>16)
		{
			std::cout<<"TROPPI CORE"<<std::endl;
			getchar();
		}
		if(gpu/nodes>2)
		{
			std::cout<<"TROPPI GPU"<<std::endl;
			getchar();
		}
		if(mic/nodes>2)
		{
			std::cout<<"TROPPI MIC"<<std::endl;
			getchar();
		}
		if(gpu/nodes>0 && mic>0)
		{
			std::cout<<"GPU con MIC mischiati"<<std::endl;
			getchar();
		}
		if(mem/nodes>33554432)
		{
			std::cout<<"TROPPI CORE"<<std::endl;
			getchar();
		}
		//std::cout<<id<<" "<<name<<" "<<userName<<" "<<coda<<" "<<enterQueueTime<<" "<<nodes<<" "<<cores<<" "<<gpu<<" "<<mic<<" "<<mem<<" "<<estimatedDuration<<" "<<realDuration<<" "<<start<<std::endl;
		job.push_back(Job(data1[0],data1[1],data1[2],data1[3],enterQueueTime,nodes,cores,gpu,mic,mem,estimatedDuration,realDuration,start,usedNodes));
		letti++;
	}
	std::cout<<"Letti "<<letti<<" Jobs"<<std::endl;
	input.close();
	return job;
}
#endif
