#ifndef Def_NodeArray
#define Def_NodeArray
#include <vector>
#include "NodeReader.hpp"

class NodeArray
{
	private:
		std::vector<Node> _nodes;
	public:
		NodeArray(std::string fNode);
		Node& operator[] (int x) {return _nodes[x];}
		Node get(int i){return _nodes[i];}
		NodeArray(){};
		int size(){return _nodes.size();}
		int getNumberOfResources(){return 4;}
};

inline NodeArray::NodeArray(std::string fNode)
{
	NodeReader r(fNode);
	_nodes=r.read();
}
#endif
