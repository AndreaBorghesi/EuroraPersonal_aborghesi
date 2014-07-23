#include <alternative_cst.h>

using namespace std;

namespace operations_research {


class AlternativeConstraint : public Constraint
{
private:
	Rev<int> nM_; // number of inner intervals which must be executed
	Rev<int> nC_; // number of inner intervals which cannot be executed
	Rev<int> minOuter_;  // minimum start time for outer interval so far
	Rev<int> maxOuter_;  // maximum end time for outer interval so far
	// the outer interval
	IntervalVar* const outerInterval_;
	// the list of inner intervals
	std::vector<IntervalVar*> innerIntervals_;
	// number of sub-units of jobs to be mapped
	int subUnitsNumber_;
	
public:
	/**
	 * @brief Constructor to build an alternative constraint
	 * @param s the solver
	 * @param outerInterval the outer interval
	 * @param innerInterval the list of inner intervals 
	 * @param subUnitsNumber the number of sub-units of jobs to be mapped 
	 */
	AlternativeConstraint(Solver* const s,
			IntervalVar* const outerInterval,
			std::vector<IntervalVar*> innerIntervals,
			int subUnitsNumber)
	: Constraint(s), outerInterval_(outerInterval), innerIntervals_(innerIntervals), subUnitsNumber_(subUnitsNumber), 
		nM_(0), nC_(0), minOuter_(outerInterval->StartMin()), maxOuter_(outerInterval->EndMax()) {}

	virtual ~AlternativeConstraint() {}

	/**
	 * @brief Post method; the constraints enforces consistency on ranges and performedness
	 */
	void Post(){
		// when the outer interval gets bounded
		outerInterval_->WhenPerformedBound(MakeConstraintDemon0(solver(), this,
					&AlternativeConstraint::PropagateOuterToInner_Perf, "o2i_Bound"));
		// when one of the innner intervals gets bounded
		for(int i = 0; i < innerIntervals_.size(); i++)
			innerIntervals_[i]->WhenPerformedBound(MakeConstraintDemon1(solver(), this,
						&AlternativeConstraint::PropagateInnerToOuter_Perf, "i2o_Bound",i));

		// when the range of the outer interval changes -- is this really needed?
		//outerInterval_->WhenRange(MakeConstraintDemon0(solver(), this,
		//			&AlternativeConstraint::PropagateOuterToInner_Range, "o2i_Range"));
		// when the range of one of the inner intervals changes
		for(int i = 0; i < innerIntervals_.size(); i++){
			innerIntervals_[i]->WhenStartRange(MakeConstraintDemon1(solver(), this,
						&AlternativeConstraint::PropagateInnerToOuter_Range, "i20_Range",i));
			innerIntervals_[i]->WhenEndRange(MakeConstraintDemon1(solver(), this,
						&AlternativeConstraint::PropagateInnerToOuter_Range, "i20_Range",i));
		}

	}


	void PropagateOuterToInner_Perf(){
		if(!outerInterval_->MayBePerformed()){  // if the outer interval is not performed no inner interval can be performed
			nM_.Value(0);
			for(int i = 0; i<innerIntervals_.size(); i++){
				innerIntervals_[i]->SetPerformed(false);
				innerIntervals_[i]->SetPerformed(false);
			}
		}
		else if(outerInterval_->MustBePerformed()){  // if the outer interval must be performed then exactly subUnitsNumber_ inner intervals must be performed
			std::vector<IntVar*> performedExpressions;
			for(int i = 0; i<innerIntervals_.size(); i++)
				performedExpressions.push_back(innerIntervals_[i]->PerformedExpr()->Var());
			//solver->SumEqual(performedExpressions,subUnitsNumber_);
			
		}
	}

        
	void PropagateInnerToOuter_Perf(int intervalIndex){
		// update the constraint inner counter depending of the "performedness" of the considered inner interval
		if(innerIntervals_[intervalIndex]->MustBePerformed())
			nM_.SetValue(nM_.Value()+1);
		if(!innerIntervals_[intervalIndex]->MayBePerformed())
			nC_.SetValue(nC_.Value()+1);

		if(nM_.Value()==subUnitsNumber_){ 
			// we reached the number of inner intervals to be performed: the remaining must be unperformed
			for

		}

		if(nM_.Value()>subUnitsNumber_)  // too many inner intervals required -> constraint fails
			fail

		if(nM_.Value()>0){  // at least one inner interval (and thus the outer one) must be performed
			if(innerIntervals_.size()-nC_.Value()<subUnitsNumber_)  // too few inner intervals may be executed -> constraint fails
				fails
	                if(innerIntervals_.size()-nC_.Value()==<subUnitsNumber_){ 
			// we reached the number of inner intervals not to be performed: the remaining must be performed

			}	

		}

		// if no inner interval must be perfomed then neither the outer one has to
		if(nM_.Value()==0){
			outerInterval_->SetPerformed(false);
			outerInterval_->SetPerformed(false);
		}
		else{ // if the outer interval needs to be performed
			outerInterval_->SetPerformed(true);
			outerInterval_->SetPerformed(true);
		}

		// if a new inner interval gets bounded should also check the new ranges for the outer interval
		PropagateInnerToOuter_Range(intervalIndex);
	}

	void PropagateOuterToInner_Range(){
		for(int i = 0; i<innerIntervals_.size(); i++){
			innerIntervals_[i]->SetStartMin(outerInterval_->StartMin());
			innerIntervals_[i]->SetEndMax(outerInterval_->EndMax());
		}
	}

	void PropagateInnerToOuter_Range(int intervalIndex){
		// the outer interval domain is the union of the (performed) inner intervals domains
		int maxEnd, minStart;
		if(innerIntervals_[intervalIndex]->MustBePerformed()){  // if the interval changed must still be performed
			// we compute the new outer range only if the change in the innner interval really affect it 
			if((innerIntervals_[intervalIndex]->StartMin() > minOuter_.Value()) || 
				(innerIntervals_[intervalIndex]->EndMax() < maxOuter_.Value())){
				// search for the new range bounds
				minStart=innerIntervals_[intervalIndex]->StartMin();
				maxEnd=innerIntervals_[intervalIndex]->EndMax();
				for(int i = 0; i<innerIntervals_.size(); i++){
					if(innerIntervals_[i]->MustBePerformed()){
						if(innerIntervals_[i]->StartMin() < minStart)
							minStart = innerIntervals_[i]->StartMin();
						if(innerIntervals_[i]->EndMax() > maxEnd)
							maxEnd = innerIntervals_[i]->EndMax();
					}
				}
				// when needed, we update the outer range - also the internal state vars
				outerInterval_->SetStartMin(minStart);
				minOuter_.SetValue(solver(),minStart);
				outerInterval_->SetEndMax(maxEnd);
				maxOuter_.SetValue(solver(),maxEnd);
			}
		}
	}
	

	/**
	 * @brief Propagation method
	 */
	void InitialPropagate() {
		// set initial internal status of the constraint
		int toBePerformed = 0;
		int notToBePerformed = 0;
		for(int i = 0; i<innerIntervals_.size(); i++){
			if(innerIntervals_[i]->MustBePerformed())
				toBePerformed++;
			if(innerIntervals_[i]->MayBePerformed())
				notToBePerformed++;
		}
                nM_.SetValue(solver(),toBePerformed);
		nC_.SetValue(solver(),notToBePerformed);

		for(int i = 0; i<innerIntervals_.size(); i++){
			PropagateInnerToOuter_Perf(i);
			PropagateInnerToOuter_Range(i);
		}

		PropagateOuterToInner_Perf();
		PropagateOuterToInner_Range();
	}
	
	
	/**
	 * @brief Print debug string
	 * @return the debug string
	 */
	string DebugString() const {
		string out;
		out.append("ALTERNATIVE >> outerInterval: ");
		out.append(outerInterval_->DebugString());
		out.append(" - innerIntervals: ");
		for( int i=0; i<innerIntervals_.size(); i++){
			out.append(innerIntervals_[i]->DebugString());
			out.append(" ; ");
		}
		out.append(" - subUnitsNumber: ");
		StringAppendF(&out, "%d", subUnitsNumber_);
		out.append(" <<");
		return out;
	}
	
	// the accept method needs not to be virtual
	void Accept(ModelVisitor* const visitor) const {
		// empty for now
	}
}; // end of AlternativeConstraint


//
// Building function for an alternative product constraint
//
Constraint* MakeAlternativeConstraint(Solver* s,
	IntervalVar* const outerInterval,
	std::vector<IntervalVar*> innerIntervals,
	int subUnitsNumber) {
    return s->RevAlloc(new AlternativeConstraint(s, outerInterval, innerIntervals, subUnitsNumber));
}
	
} // end of operations_research namespace
