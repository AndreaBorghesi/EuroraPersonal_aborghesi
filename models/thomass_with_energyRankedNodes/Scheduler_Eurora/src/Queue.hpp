#ifndef Def_Queue
#define Def_Queue
#include <string>
class Queue
{
	private:
		std::string _id;
		int _waiting;
	public:
		int getMaxMinutesToWait(){return _waiting;}
		std::string getId(){return _id;}
		void setMaxMinutesToWait(int w){_waiting=w;}
		void setId(std::string id){_id=id;}
		Queue(std::string id,int w){setId(id),setMaxMinutesToWait(w);}
};
#endif
