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
//		Node getByNumber(int num) {
//		 Node res;
//		 int i;
//		 for(i=0;i<_nodes.size();i++)
//		 	if(_nodes[i].getNodeNumber()==num)
//		 		res=_nodes[i];
//		 return res;
//		}
		Node& getByNumber(int num) {
		 int i;
		 for(i=0;i<_nodes.size();i++)
		 	if(_nodes[i].getNodeNumber()==num)
		 		return _nodes[i];
		}
		NodeArray(){};
		int size(){return _nodes.size();}
		int getNumberOfResources(){return 4;}
		
};

inline NodeArray::NodeArray(std::string fNode)
{
	NodeReader r(fNode);
	//NodeEnergyUpdater u(nfFile,rfMFile,rfCFile,eMFile,eCFile);
	_nodes=r.read();
	//u.update(_nodes);
}
#endif
