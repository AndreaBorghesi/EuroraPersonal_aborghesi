#ifndef Def_Model20
#define Def_Model20
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"
#include "IModel.hpp"
#include "IModelAdvanced.hpp"
#include <ilcp/cp.h>
#include <vector>
#include <stdio.h>


class Model20: public IModelAdvanced
{
private:
	int _minutesOfExecution;
	int _minTime;
	double _os;
	
	
	int numberOfJointNodes(Job j,Node n)
	{
		int r=j.getNumberOfNodes();
		if(j.getNumberOfCores()!=0)
			r=Util::min(r,n.getCoresNumber()/(j.getNumberOfCores()/j.getNumberOfNodes()));
		if(j.getNumberOfGPU()!=0)
			r=Util::min(r,n.getGPUsNumber()/(j.getNumberOfGPU()/j.getNumberOfNodes()));
		if(j.getNumberOfMIC()!=0)
			r=Util::min(r,n.getMICsNumber()/(j.getNumberOfMIC()/j.getNumberOfNodes()));
		if(j.getMemory()!=0)
			r=Util::min(r,n.getTotalMemory()/(j.getMemory()/j.getNumberOfNodes()));
		return r;
	}
	
public:
	Model20();
	Model20(QueueArray queue,NodeArray nodes,JobArray jobs){_queue=queue,_nodes=nodes,_jobs=jobs;_minTime=1;_os=-1;_wt=-1;_wtDelta=-1;_nl=-1;_nlDelta=-1;_mk=-1;_mkDelta=-1;}
	JobArray solve(int refTime);
	double getOptimalSolution(){return _os;}
};

inline JobArray Model20::solve(int refTime)
{
	bool solved = false;
	bool retry=false;
	_minutesOfExecution=_minTime;
	IloInt timeLimit = 60*_minutesOfExecution;
	
	try	
	{
		do{
			IloEnv env; 
			
			IloModel model(env);
			IloIntervalVarArray task(env);
			for (int i = 0; i < _jobs.size(); ++i) 
			{
				task.add(IloIntervalVar(env, _jobs[i].getDuration(refTime)));
				if(_jobs[i].isScheduled() && _jobs[i].getStartTime()<refTime)
				{
					task[i].setStartMin(_jobs[i].getStartTime());
					task[i].setStartMax(_jobs[i].getStartTime());
				}
				else
				{
					task[i].setStartMin(refTime);
				}
			
				if(_mk>0 && _mkDelta>0)
				{
					task[i].setStartMax(_mk*_mkDelta-_jobs[i].getDuration(refTime));
				}
			} 
			
			if(_nl>0 && _nlDelta>0)
			{
				IloNumExprArray nlSum(env);
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					nlSum.add(IloStartOf(task[i])-_jobs[i].getEnterQueueTime()>_queue.getMaxMinutesToWait(_jobs[i].getQueue()));
				}
				model.add(IloSum(nlSum) <= _nl*_nlDelta);
			}
			
			if(_wt>0 && _wtDelta>0)
			{
				IloNumExprArray wtSum(env);
				for (int i = 0; i < _jobs.size(); ++i) 
				{
					IloNum wait=_queue.getMaxMinutesToWait(_jobs[i].getQueue());
					wtSum.add(IloMax((IloStartOf(task[i])-_jobs[i].getEnterQueueTime()-wait)/wait,0));
				}
				model.add(IloSum(wtSum) <= _wt*_wtDelta);
			}
		
			std::vector< std::vector<IloIntervalVarArray> > UtilNodes(_jobs.size());
			for (int i = 0; i < _jobs.size(); i++) 
			{
				UtilNodes[i]=std::vector<IloIntervalVarArray>(_nodes.size());
				std::vector<int> usedNodes=_jobs[i].getUsedNodes();
				for(int j=0;j<_nodes.size();j++)
				{
					UtilNodes[i][j]=IloIntervalVarArray(env);
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j]);k++)
					{
						UtilNodes[i][j].add(IloIntervalVar(env, _jobs[i].getDuration(refTime)));
						if(_jobs[i].isScheduled() && _jobs[i].getStartTime()<refTime) 
						{

							if(usedNodes[j]>=1)
							{
								UtilNodes[i][j][k].setStartMin(_jobs[i].getStartTime());
								UtilNodes[i][j][k].setStartMax(_jobs[i].getStartTime());
								usedNodes[j]--;
							}
							else
							{
								UtilNodes[i][j][k].setAbsent();
							}
						}
						else
						{
							UtilNodes[i][j][k].setOptional();
							UtilNodes[i][j][k].setStartMin(refTime);
						}
					}	
				}		
			}
			
			//alternative
			for(int i=0;i<_jobs.size();i++)
			{
				IloIntervalVarArray c(env);
				for(int j=0;j<_nodes.size();j++)
				{
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j]);k++)
					{
						c.add(UtilNodes[i][j][k]);
					}
				}
				IloAlternative a1(env,task[i],c,_jobs[i].getNumberOfNodes());
				model.add(a1);
			}
		
		
			//cumulfunction cores
			IloCumulFunctionExprArray usgCores(env); 
			for (int j = 0; j < _nodes.size(); j++) {
				usgCores.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) {
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j]);k++)
					{
						usgCores[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfCores()/_jobs[i].getNumberOfNodes());
					}
				}
				model.add(usgCores[j] <= _nodes[j].getCoresNumber());
			}
		
			//cumulfunction gpu
			IloCumulFunctionExprArray usgGPU(env); 
			for (int j = 0; j < _nodes.size(); j++) {
				usgGPU.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) {
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j]);k++)
					{
						usgGPU[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfGPU()/_jobs[i].getNumberOfNodes());
					}
				}
				model.add(usgGPU[j] <= _nodes[j].getGPUsNumber());
			}
		
			//cumulfunction mic
			IloCumulFunctionExprArray usgMIC(env); 
			for (int j = 0; j < _nodes.size(); ++j) {
				usgMIC.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) {
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j]);k++)
					{
						usgMIC[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getNumberOfMIC()/_jobs[i].getNumberOfNodes());
					}
				}
				model.add(usgMIC[j] <= _nodes[j].getMICsNumber());
			}
		
			//cumulfunction mem
			IloCumulFunctionExprArray usgMem(env); 
			for (int j = 0; j < _nodes.size(); ++j) {
				usgMem.add(IloCumulFunctionExpr(env));
				for (int i = 0; i < _jobs.size(); ++i) {
					for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j]);k++)
					{
						usgMem[j] += IloPulse(UtilNodes[i][j][k], _jobs[i].getMemory()/_jobs[i].getNumberOfNodes());
					}
				}
				model.add(usgMem[j] <= _nodes[j].getTotalMemory());
			}

			// define the problem objective
			IloIntExprArray ends(env);
			for (int i = 0; i < _jobs.size(); ++i) {
				ends.add(IloEndOf(task[i]));
			}
			model.add(IloMinimize(env, IloMax(ends)));
	
			// ================================
			// = Bulid and configure a solver =
			// ================================
		
			// build a solver
			IloCP cp(model);

			// USE A SINGLE PROCESSOR (DO NOT TOUCH THIS)
			cp.setParameter(IloCP::Workers, 4);
	
			// DEFINING A TIME LIMIT
			// other limit types are described the solver documentation
			timeLimit = 60*_minutesOfExecution;
			cp.setParameter(IloCP::TimeLimit, timeLimit);
			
			// LOG EVERY N BRANCHES:
			// cp.setParameter(IloCP::LogPeriod, 100000);
	
			// DO NOT LOG AT ALL
			cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);

			// SET SEARCH TYPE TO DSF
			// keep commented to use te default search
			// other search modes are available: check the documentation
			cp.setParameter(IloCP::SearchType, IloCP::Auto);//IloCP::DepthFirst IloCP:Restart IloCP::MultiPoint IloCP::Auto 

			// SET FILTERING STRENGTH FOR "ALL" CONSTRAINTS
			// other strength values are available from the documentation
			cp.setParameter(IloCP::DefaultInferenceLevel, IloCP::Basic); //IloCP::Low, IloCP::Basic, IloCP::Medium, and IloCP::Extended
	
			// SET FILTERING STRENGTH FOR THE CUMULATIVE CONSTRAINTS ONLY
			// other strength values are available from the documentation
			cp.setParameter(IloCP::CumulFunctionInferenceLevel, IloCP::Extended);
	
			// =====================
			// = Solve the problem =
			// =====================
	
			// start to search
			cp.startNewSearch();
			

			// loops until there are solution
			while (cp.next()) 
			{
				
				// "remember" that at least a solution was found
				solved = true;
				retry=false;
				_minutesOfExecution=_minTime;
				std::cout << "***** SOLUTION FOUND ***** time: " << cp.getInfo(IloCP::SolveTime) <<" makespan: " << cp.getObjValue() << std::endl;
			
				/* DE-COMMENT THIS TO SEE HOW THE SOLUTION LOOKS LIKE
				for (int i = 0; i < _jobs.size(); ++i) 
				{
				std::cout << cp.domain(task[i])<<std::endl;
				}*/
				_os=cp.getObjValue();
			
			}
			if(solved)
			{
				for(int i=0;i<_jobs.size();i++)
				{
					std::vector<int> nodes(_nodes.size(),0);
					for(int j=0;j<_nodes.size();j++)
					{
						for(int k=0;k<numberOfJointNodes(_jobs[i],_nodes[j]);k++)
						{
							if(cp.isPresent(UtilNodes[i][j][k]))
								nodes[j]++;
					
						}
					}
					_jobs[i].allocate(cp.getStart(task[i]),nodes);
				}
			}
			else
			{
				_minutesOfExecution*=5;
			}

			// terminate search
			cp.endSearch();
			if (!solved && cp.getInfo(IloCP::SolveTime) >= timeLimit) 
			{
				retry=true;
				std::cout << "min" << _minutesOfExecution << std::endl;
				if(_minutesOfExecution>=125)
					exit(0);
				//getchar();
			}
			else if(!solved)
			{
				std::cout << "THE PROBLEM IS INFEASIBLE" << std::endl;
				break;
			}
	
			// DE-COMMENT THIS IF YOU ARE NOT USING LOGGING
			//cp.printInformation();
	
			env.end();
			
		}while(retry);
		
	}
	catch (IloException& ex) 
	{
		std::string m(ex.getMessage());
		if(m.find("Empty array in expression")==string::npos)
		{
			
			std::cout << "Error: " << m <<". Exception type " <<typeid(ex).name()<< std::endl;
		
			//getchar();
			exit(0);
		}
	}
	return _jobs;
}
#endif
