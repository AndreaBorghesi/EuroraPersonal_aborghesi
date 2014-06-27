j#ifndef Def_NodeEnergyUpdater
#define Def_NodeEnergyUpdater
#include <vector>
#include <iostream>
#include <fstream>
#include "Node.hpp"
#include "NodeArray.hpp"

class NodeEnergyUpdater
{
	private:
		std::string _nomFreqFile;
		std::string _rFreqMEMFile;
		std::string _rFreqCPUFile;
		std::string _energyMEMFile;
		std::string _energyCPUFile;
	public:
		NodeEnergyUpdater(std::string nfFile,std::string rfMFile,std::string rfCFile,std::string eMFile,std::string eCFile);
		void update(NodeArray &nodes);
		void setup (NodeArray &nodes);
};

inline NodeEnergyUpdater::NodeEnergyUpdater(std::string nfFile,std::string rfMFile,std::string rfCFile,std::string eMFile,std::string eCFile)
{
	//std::cout<<"Inside NodeEnergyUpdater constructor"<<std::endl; 
	_nomFreqFile = nfFile;
	_rFreqMEMFile = rfMFile;
	_rFreqCPUFile = rfCFile;
	_energyMEMFile = eMFile;
	_energyCPUFile = eCFile;
}

// since on EURORA we have some nodes not working or we miss some data (for example on nodes used only for debug), 
// but in our model we want to energetically rank all nodes we make some assumpionts for the missing values
inline void NodeEnergyUpdater::setup(NodeArray &nodes)
{
	for(int i=0;i<nodes.size();i++){
		if(i<32){
			nodes[i].setNFreq(2100.0);
			nodes[i].setRFreqMEM(2100.0);
			nodes[i].setRFreqCPU(2100.0);
			nodes[i].setEnergyMEM(61500.0);
			nodes[i].setEnergyCPU(52500.0);
			}
		else{
			nodes[i].setNFreq(3100);
			nodes[i].setRFreqMEM(3400);
			nodes[i].setRFreqCPU(3400);
			nodes[i].setEnergyMEM(103000.0);
			nodes[i].setEnergyCPU(62500.0);
		}
			
	}
}


// read nominal frequency, real frequency and energy values for each node from log files (MEM and CPU bound jobs)
inline void NodeEnergyUpdater::update(NodeArray &nodes)
{	
//	std::cout<<"Inside NodeEnergyUpdater update() -- BEGIN"<<std::endl; 
//	for(int i=0;i<nodes.size();i++)
//			std::cout<<" ---- "<<nodes[i].getNodeNumber()<<";"<<nodes[i].getNominalFrequency()<<";"<<nodes[i].getRealMEMFrequency()<<";"<<nodes[i].getRealCPUFrequency()<<";"<<nodes[i].getEnergyMEM()<<";"<<nodes[i].getEnergyCPU()<<" ---- "<<std::endl;
	// nominal frequency
	std::ifstream input(_nomFreqFile.c_str());
	//std::cout<<"Inside NodeEnergyUpdater update() -- Nominal Freq"<<std::endl; 
	if (!input) 
	{
		std::cout << "ERROR: Could not open file \"" << _nomFreqFile << "\"" << std::endl;
		exit(1);
	}
	while (!input.eof())
	{
		int numNode;
		double nfreq;
		input>>numNode;
		input>>nfreq;
		//std::cout<<"Inside NodeEnergyUpdater update() -- Before getByNumber() -- Nominal Freq"<<std::endl; 
		//std::cout<<numNode<<nfreq<<std::endl;  	
		nodes.getByNumber(numNode).setNFreq(nfreq);		
		//std::cout<<"Inside NodeEnergyUpdater update() -- After getByNumber() -- Nominal Freq"<<std::endl; 
	}
	input.close();
	
	// real frequency - MEM bound jobs
	//std::cout<<"Inside NodeEnergyUpdater update() -- Real Freq MEM"<<std::endl; 
	std::ifstream input2(_rFreqMEMFile.c_str());
	if (!input2) 
	{
		std::cout << "ERROR: Could not open file \"" << _rFreqMEMFile << "\"" << std::endl;
		exit(1);
	}
	while (!input2.eof())
	{
		int numNode;
		double rfMfreq;
		input2>>numNode;
		input2>>rfMfreq;
		nodes.getByNumber(numNode).setRFreqMEM(rfMfreq);		
	}
	input2.close();
	
	// real frequency - CPU bound jobs
	std::ifstream input3(_rFreqCPUFile.c_str());
	//std::cout<<"Inside NodeEnergyUpdater update() -- Real Freq CPU"<<std::endl; 
	if (!input3) 
	{
		std::cout << "ERROR: Could not open file \"" << _rFreqCPUFile << "\"" << std::endl;
		exit(1);
	}
	while (!input3.eof())
	{
		int numNode;
		double rfCfreq;
		input3>>numNode;
		input3>>rfCfreq;
//		std::cout << numNode << " " << rfCfreq <<std::endl;
//		Node n = nodes.getByNumber(numNode);
//		std::cout << n.getNodeNumber() << " " << n.getRealCPUFrequency() << std::endl;
		nodes.getByNumber(numNode).setRFreqCPU(rfCfreq);		
//		n.setRFreqCPU(rfCfreq);	
//		std::cout << n.getNodeNumber() << " " << n.getRealCPUFrequency() << std::endl;
	}
	input3.close();
	
	// energy - MEM bound jobs
	std::ifstream input4(_energyMEMFile.c_str());
//	std::cout<<"Inside NodeEnergyUpdater update() -- Energy MEM"<<std::endl; 
	if (!input4) 
	{
		std::cout << "ERROR: Could not open file \"" << _energyMEMFile << "\"" << std::endl;
		exit(1);
	}
	while (!input4.eof())
	{
		int numNode;
		double eMfreq;
		input4>>numNode;
		input4>>eMfreq;
		nodes.getByNumber(numNode).setEnergyMEM(eMfreq);		
	}
	input4.close();
	
	// energy - CPU bound jobs
//	std::cout<<"Inside NodeEnergyUpdater update() -- Energy CPU"<<std::endl; 
	std::ifstream input5(_energyCPUFile.c_str());
	if (!input5) 
	{
		std::cout << "ERROR: Could not open file \"" << _energyCPUFile << "\"" << std::endl;
		exit(1);
	}
	while (!input5.eof())
	{
		int numNode;
		double eCfreq;
		input5>>numNode;
		input5>>eCfreq;
		nodes.getByNumber(numNode).setEnergyCPU(eCfreq);		
	}
	input5.close();
	
//	std::cout<<"Inside NodeEnergyUpdater update() -- END"<<std::endl; 
//	for(int i=0;i<nodes.size();i++)
//			std::cout<<" ---- "<<nodes[i].getNodeNumber()<<";"<<nodes[i].getNominalFrequency()<<";"<<nodes[i].getRealMEMFrequency()<<";"<<nodes[i].getRealCPUFrequency()<<";"<<nodes[i].getEnergyMEM()<<";"<<nodes[i].getEnergyCPU()<<" ---- "<<std::endl;
}
#endif
