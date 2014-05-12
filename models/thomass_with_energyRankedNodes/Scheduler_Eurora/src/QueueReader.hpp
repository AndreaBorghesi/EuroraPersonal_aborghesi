#ifndef Def_QueueReader
#define Def_QueueReader
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "Queue.hpp"
#include <stdlib.h>

class QueueReader
{
	private:
		std::string _file;
	public:
		QueueReader(std::string file);
		std::vector<Queue> read();
};

inline QueueReader::QueueReader(std::string file)
{
	_file=file;
}

inline std::vector<Queue> QueueReader::read()
{
	std::ifstream inputCode(_file.c_str());
	if (!inputCode) 
	{
		std::cout << "ERROR: Could not open file \"" << _file << "\"" << std::endl;
		exit(1);
	}
	
	std::vector<Queue> code;
	while (!inputCode.eof())
	{
		int w;
		std::string id;
		inputCode>>id;
		inputCode>>w;
		code.push_back(Queue(id,w));
	}
	inputCode.close();
	return code;
}
#endif
