#ifndef Def_Checker
#define Def_Checker
#include <vector>
#include "JobArray.hpp"

class Checker
{
private:
public:
	static int CheckJobArray(JobArray jobs,NodeArray nodes)
	{
		int checkTime=0;
		int result=1;
		for(int i=0;i<jobs.size();i++)
		{
			int count=0;
			if(jobs[i].getRealDuration()<0)
				result=-3;
			if(jobs[i].getEstimatedDuration()<=0)
				result=-4;
			if(jobs[i].getStartTime()<jobs[i].getEnterQueueTime())
			{
				result=-5;
			}
			for(int j=0;j<jobs[i].getUsedNodes().size();j++)
			{
				count+=jobs[i].getUsedNodes()[j];
			}
			if(count!=jobs[i].getNumberOfNodes() && jobs[i].isScheduled())
			{
				result=-1;
			}
		}
		while((checkTime=jobs.findNextIstant(checkTime))!=-1)
		{
			vector< vector<int> > risorse(nodes.size());
			for(int i=0;i<nodes.size();i++)
			{
				
				risorse[i].push_back(nodes[i].getCoresNumber());
				risorse[i].push_back(nodes[i].getGPUsNumber());
				risorse[i].push_back(nodes[i].getMICsNumber());
				risorse[i].push_back(nodes[i].getTotalMemory());
			}
			for(int i=0;i<jobs.size();i++)
			{
				if(jobs[i].getStartTime()<=checkTime && jobs[i].getStartTime()+jobs[i].getDuration(checkTime)>checkTime)
				{
					std::vector<int> usedNodes=jobs[i].getUsedNodes();
					for(int j=0;j<usedNodes.size();j++)
					{
						risorse[j][0]-=usedNodes[j]*jobs[i].getNumberOfCores()/jobs[i].getNumberOfNodes();
						risorse[j][1]-=usedNodes[j]*jobs[i].getNumberOfGPU()/jobs[i].getNumberOfNodes();
						risorse[j][2]-=usedNodes[j]*jobs[i].getNumberOfMIC()/jobs[i].getNumberOfNodes();
						risorse[j][3]-=usedNodes[j]*jobs[i].getMemory()/jobs[i].getNumberOfNodes();
					}
				}
			}
			for(int i=0;i<nodes.size();i++)
			{
				for(int j=0;j<nodes.getNumberOfResources();j++)
				{
					if(risorse[i][j]<0)
					{
						result=-2;
					}
				}
			}
		}
		return result;
	}
	
};


#endif
