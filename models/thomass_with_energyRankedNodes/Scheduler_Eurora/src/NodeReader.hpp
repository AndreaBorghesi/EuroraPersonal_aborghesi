#ifndef Def_NodeReader
#define Def_NodeReader
#include <vector>
#include <iostream>
#include <fstream>
#include "Node.hpp"

class NodeReader
{
	private:
		std::string _file;
	public:
		NodeReader(std::string file);
		std::vector<Node> read();
};

inline NodeReader::NodeReader(std::string file)
{
	_file=file;
}

inline std::vector<Node> NodeReader::read()
{
	std::ifstream input(_file.c_str());
	if (!input) 
	{
		std::cout << "ERROR: Could not open file \"" << _file << "\"" << std::endl;
		exit(1);
	}j
	
	std::vector<Node> node;
	while (!input.eof())
	{
		int numNode;
		int numCores;
		int numGPUs;
		int numMICs;
		int totMem;
		int reserved;
		input>>numNode;
		input>>numCores;
		input>>numGPUs;
		input>>numMICs;
		input>>totMem;
		input>>reserved;
		node.push_back(Node(numNode,numCores,numGPUs,numMICs,totMem,(reserved==1?true:false)));
		
	}
	input.close();
	return node;
}
#endif
