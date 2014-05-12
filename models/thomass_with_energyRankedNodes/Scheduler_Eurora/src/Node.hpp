#ifndef Def_Node
#define Def_Node
class Node 
{
	private:
		int _numNode;
		int _numCores;
		int _numGPUs;
		int _numMICs;
		int _totMem;
		bool _reserved;
		double _nominalFrequency;
		double _rFreqMEMbound;
		double _rFreqCPUbound;
		double _energyMEMbound;
		double _energyCPUbound;
	public:
		int getNodeNumber(){return _numNode;}
		int getCoresNumber(){return _numCores;}
		int getGPUsNumber(){return _numGPUs;}
		int getMICsNumber(){return _numMICs;}
		int getTotalMemory(){return _totMem;}
		int isReserved(){return _reserved==true?1:0;}
		double getNominalFrequency(){return _nominalFrequency;}
		double getRealMEMFrequency(){return _rFreqMEMbound;}
		double getRealCPUFrequency(){return _rFreqCPUbound;}
		double getEnergyMEM(){return _energyMEMbound;}
		double getEnergyCPU(){return _energyCPUbound;}
		
		void setNodeNumber(int n){ _numNode=n;}
		void setCoresNumber(int n){ _numCores=n;}
		void setGPUsNumber(int n){ _numGPUs=n;}
		void setMICsNumber(int n){ _numMICs=n;}
		void setTotalMemory(int n){ _totMem=n;}
		void setReserved(int r){ _reserved=(r==1?true:false);}
		void setReserved(bool r){ _reserved=r;} 
		void setNFreq(double f){ _nominalFrequency=f;}
		void setRFreqMEM(double f){ _rFreqMEMbound=f;}
		void setRFreqCPU(double f){ _rFreqCPUbound=f;}
		void setEnergyMEM(double f){ _energyMEMbound=f;}
		void setEnergyCPU(double f){ _energyCPUbound=f;}

		Node(int nodes,int cores,int gpus,int mics,int mem,bool reserved){setNodeNumber(nodes),setCoresNumber(cores),setGPUsNumber(gpus),setMICsNumber(mics),setTotalMemory(mem),setReserved(reserved);}
};
#endif
