#ifndef Def_IModelAdvanced
#define Def_IModelAdvanced
#include "QueueArray.hpp"
#include "NodeArray.hpp"
#include "JobArray.hpp"
#include "IModel.hpp"

class IModelAdvanced : public IModel
{
	protected:
	double _os;
	double _wt;
	double _wtDelta;
	double _nl;
	double _nlDelta;
	double _mk;
	double _mkDelta;
	public:
		void setWT(double wt,double wtDelta){_wt=wt;_wtDelta=wtDelta;}
		void setNumLate(double nl,double nlDelta){_nl=nl;_nlDelta=nlDelta;}
		void setMK(double mk,double mkDelta){_mk=mk;_mkDelta=mkDelta;}
};
#endif
