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
		// // cout << "Inside Post() \n" << endl;
		// when the outer interval gets bounded
		// cout << " ---------- Inside Post --------------- \n" << endl;
		// cout << " ---------- Vars before propagation --------------- \n" << endl;
		// print_vars_state();

		outerInterval_->WhenPerformedBound(MakeConstraintDemon0(solver(), this,
					&AlternativeConstraint::PropagateOuterToInner_Perf, "o2i_Bound"));
		// when one of the innner intervals gets bounded
		for(int i = 0; i < innerIntervals_.size(); i++)
			innerIntervals_[i]->WhenPerformedBound(MakeConstraintDemon1(solver(), this,
						&AlternativeConstraint::PropagateInnerToOuter_Perf, "i2o_Bound",i));
                // when the range of the outer interval changes
		outerInterval_->WhenStartRange(MakeConstraintDemon0(solver(), this,
					&AlternativeConstraint::PropagateOuterToInner_Range, "o2i_StartRange"));
		outerInterval_->WhenEndRange(MakeConstraintDemon0(solver(), this,
					&AlternativeConstraint::PropagateOuterToInner_Range, "o2i_EndRange"));
		// when the range of one of the inner intervals changes
		for(int i = 0; i < innerIntervals_.size(); i++){
			innerIntervals_[i]->WhenStartRange(MakeConstraintDemon1(solver(), this,
						&AlternativeConstraint::PropagateInnerToOuter_Range, "i20_StartRange",i));
			innerIntervals_[i]->WhenEndRange(MakeConstraintDemon1(solver(), this,
						&AlternativeConstraint::PropagateInnerToOuter_Range, "i20_EndRange",i));
		}

	}


	void PropagateOuterToInner_Perf(){
		// cout << "Inside PropagateOuterToInner_Perf \n" << endl;
		if(!outerInterval_->MayBePerformed()){  // if the outer interval is not performed no inner interval can be performed
			// cout << "\t!outerInterval_->MayBePerformed()\n" << endl;
			// // cout << "Inside PropagateOuterToInner_Perf - outer must not be performed \n" << endl;
			nM_.SetValue(solver(),0);
			nC_.SetValue(solver(),innerIntervals_.size());
			for(int i = 0; i<innerIntervals_.size(); i++){
				innerIntervals_[i]->SetPerformed(false);
				innerIntervals_[i]->SetPerformed(false);
			}
		}
		else if(outerInterval_->MustBePerformed()){  // if the outer interval must be performed then exactly subUnitsNumber_ inner intervals must be performed

			// cout << "\touterInterval_->MustBePerformed()\n" << endl;
			if(nM_.Value()==subUnitsNumber_){ 
				// cout << "\t\tnM_.Value()==subUnitsNumber_\n" << endl;
				// we reached the number of inner intervals to be performed: the remaining must be unperformed
				for(int i = 0; i<innerIntervals_.size(); i++){
					// cout << "\t\t\tinnerIntervals_ " << i << "\n" << endl;
					if(innerIntervals_[i]->MayBePerformed() && !innerIntervals_[i]->MustBePerformed()){
						// cout << "\t\t\t\tinnerIntervals_[i]->MayBePerformed()" << i << "\n" << endl;
						innerIntervals_[i]->SetPerformed(false);
					}
				}
				nC_.SetValue(solver(),innerIntervals_.size()-subUnitsNumber_);

			}
			if(innerIntervals_.size()-nC_.Value()==subUnitsNumber_){ 
				// cout << "\t\tinnerIntervals_.size()-nC_.Value()==subUnitsNumber_\n" << endl;
				// we reached the number of inner intervals not to be performed: the remaining must be performed
				for(int i = 0; i<innerIntervals_.size(); i++)
					if(innerIntervals_[i]->MayBePerformed())
						innerIntervals_[i]->SetPerformed(true);

				nM_.SetValue(solver(),subUnitsNumber_);
			}
			// if the overall situation is not fixed yet, we force to perform the needed number of inner intervals
			int n_intervalsToSet = nM_.Value(); // the number of inner intervals set so far --> we need to set exactly subUnitsNumber
			int i = 0;

			// // cout << "Inside PropagateOuterToInner_Perf - n_intervalsToSet " << n_intervalsToSet << "\n" << endl;
                        while(n_intervalsToSet < subUnitsNumber_ && i < innerIntervals_.size()){
				// we set as performed the first undecided innner intervals we encounter
				// we follow the order of the interval variables declaration
				if(!innerIntervals_[i]->IsPerformedBound()){
					innerIntervals_[i]->SetPerformed(true);
					n_intervalsToSet++;
				}
				i++;
			}
			// // cout << "Inside PropagateOuterToInner_Perf - n_intervalsToSet " << n_intervalsToSet << "\n" << endl;
			// the remaining undecided innner intervals are forced not to be performed
			while(i<innerIntervals_.size()){
				if(!innerIntervals_[i]->IsPerformedBound())
					innerIntervals_[i]->SetPerformed(false);
				i++;
			}
		}
		
		// cout << "(Again) Inside PropagateOuterToInner_Perf \n" << endl;
		// cout << " ---------- Vars after propagation --------------- \n" << endl;
		// print_vars_state();
	}

        
	void PropagateInnerToOuter_Perf(int intervalIndex){

		// cout << "Inside PropagateInnerToOuter_Perf; Caller: " << intervalIndex << "\n" << endl;
		//// cout << "outerInterval_ " << outerInterval_ <<" \n" << endl;

		//// cout << "nM_ " << nM_.Value() <<" \n" << endl;
		//// cout << "nC_ " << nC_.Value() <<" \n" << endl;


		//// cout << "intervalIndex " << intervalIndex <<" \n" << endl;
                if(nM_.Value()!=subUnitsNumber_){
                // update the constraint inner counter depending of the "performedness" of the considered inner interval
		if(innerIntervals_[intervalIndex]->MustBePerformed()){
			// cout << "\tinnerIntervals_[intervalIndex]->MustBePerformed() nM_++\n " << endl;
			nM_.SetValue(solver(),nM_.Value()+1);
		}
		if(!innerIntervals_[intervalIndex]->MayBePerformed()){
			// cout << "\t!innerIntervals_[intervalIndex]->MayBePerformed() nC_++\n " << endl;
			nC_.SetValue(solver(),nC_.Value()+1);
		}
		// if the outer interval may not be performed then there's no point in checking the inner intervals
		if(outerInterval_->MayBePerformed()){
			
			// cout << "\touterInterval_->MayBePerformed()\n " << endl;
			if(nM_.Value()==subUnitsNumber_){ 
				// cout << "\t\tnM_.Value()==subUnitsNumber_\n " << endl;
				// we reached the number of inner intervals to be performed: the remaining must be unperformed
				for(int i = 0; i<innerIntervals_.size(); i++){
						if(i!=intervalIndex && !innerIntervals_[i]->MustBePerformed()){
							innerIntervals_[i]->SetPerformed(false);
							nC_.SetValue(solver(),nC_.Value()+1);
						}
				}
			}

			if(nM_.Value()>subUnitsNumber_){  // too many inner intervals required -> constraint fails
				// cout << "\t\tnM_: " << nM_.Value() << " SubunitsNumber: " << subUnitsNumber_ << "\n " << endl;
				// cout << "\t\tI'm failing here 1\n " << endl;
				solver()->Fail();
			}


			if(nM_.Value()>0 && nM_.Value()<subUnitsNumber_ ){  // at least one inner interval (and thus the outer one) must be performed

				// cout << "\t\tnM_.Value()>0 && nM_.Value()<subUnitsNumber_ \n " << endl;
				if(innerIntervals_.size()-nC_.Value()<subUnitsNumber_){  // too few inner intervals may be executed -> constraint fails
					// cout << "\t\t\tinnerIntervals_.size(): " << innerIntervals_.size() << " nC_.value(): " << nC_.Value() << endl;
				        // cout << "\t\t\tI'm failing here 2\n " << endl;
					solver()->Fail();
				}
				if(innerIntervals_.size()-nC_.Value()==subUnitsNumber_){ 
					// cout << "\t\t\tinnerIntervals_.size()-nC_.Value()==subUnitsNumber_\n " << endl;
					if(innerIntervals_.size()-nC_.Value()<subUnitsNumber_){  // too few inner intervals may be executed -> constraint fails
					// we reached the number of inner intervals not to be performed: the remaining must be performed
						for(int i = 0; i<innerIntervals_.size(); i++){
							if(i!=intervalIndex && innerIntervals_[i]->MayBePerformed()){
								innerIntervals_[i]->SetPerformed(true);
								nM_.SetValue(solver(),nM_.Value()+1);
							}
						}
					}	
				}

			}

			// if no inner interval must be perfomed then neither the outer one has to
			if(nM_.Value()==0 && nC_.Value()==innerIntervals_.size()){
				// cout << "\t\tnM_.Value()==0 && nC_.Value()==innerIntervals_.size()\n " << endl;
				outerInterval_->SetPerformed(false);
				outerInterval_->SetPerformed(false);
			}
			else if(nM_.Value()>0){ // if the outer interval needs to be performed
				// cout << "\t\tnM_.Value()>0\n " << endl;
				outerInterval_->SetPerformed(true);
				outerInterval_->SetPerformed(true);
			}

                        //// cout << "After propagation \n" << endl;
			//// cout << "outerInterval_ " << outerInterval_ <<" \n" << endl;

			//// cout << "nM_ " << nM_.Value() <<" \n" << endl;
			//// cout << "nC_ " << nC_.Value() <<" \n" << endl;

			// if a new inner interval gets bounded should also check the new ranges for the outer interval <<<<<< REALLY ??
			//PropagateInnerToOuter_Range(intervalIndex);
		}
		}

		// cout << "(Again) Inside PropagateInnerToOuter_Perf; Caller: " << intervalIndex << "\n" << endl;
		// cout << " ---------- Vars after propagation --------------- \n" << endl;
		// print_vars_state();
	}

	void PropagateOuterToInner_Range(){
		if(outerInterval_->StartMin() > minOuter_.Value())
			minOuter_.SetValue(solver(),outerInterval_->StartMin());
		if(outerInterval_->EndMax() < maxOuter_.Value())
			maxOuter_.SetValue(solver(),outerInterval_->EndMax());
		if(outerInterval_->MayBePerformed()){
			for(int i = 0; i<innerIntervals_.size(); i++){
				innerIntervals_[i]->SetStartMin(outerInterval_->StartMin());
				innerIntervals_[i]->SetEndMax(outerInterval_->EndMax());
			}
		}

		// cout << "Inside PropagateOuterToInner_Range \n" << endl;
		// cout << " ---------- Vars after propagation --------------- \n" << endl;
		// print_vars_state();
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

		// cout << "Inside PropagateInnerToOuter_Range; Caller: " << intervalIndex << "\n" << endl;
		// cout << " ---------- Vars after propagation --------------- \n" << endl;
		// print_vars_state();
		
	}
	

	/**
	 * @brief Propagation method
	 */
	void InitialPropagate() {
		// cout << "::: InitialPropagate begins :::\n" << endl;

		// set initial internal status of the constraint
		int toBePerformed = 0;
		int notToBePerformed = 0;
		for(int i = 0; i<innerIntervals_.size(); i++){
			if(innerIntervals_[i]->MustBePerformed())
				toBePerformed++;
			if(!innerIntervals_[i]->MayBePerformed())
				notToBePerformed++;
		}
                //nM_.SetValue(solver(),toBePerformed);
		//nC_.SetValue(solver(),notToBePerformed);

		nM_.SetValue(solver(),0);
		nC_.SetValue(solver(),0);

		for(int i = 0; i<innerIntervals_.size(); i++){
			PropagateInnerToOuter_Perf(i);
			PropagateInnerToOuter_Range(i);
		}

		PropagateOuterToInner_Perf();
		PropagateOuterToInner_Range();

		// cout << "::: InitialPropagate ends :::\n" << endl;
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

	//print status of internal variables
	void print_vars_state() const {
		cout << " >>>> outerInterval: " << outerInterval_ << " \n" << endl;
		for(int i=0;i<innerIntervals_.size();i++)
			 cout << " >>>> innerIntervals_: " << innerIntervals_[i] << " \n" << endl;
		cout << " >>>> subUnitsNumber_: " << subUnitsNumber_ << " \n" << endl;
		cout << " >>>> nM_: " << nM_.Value() << " \n" << endl;
		cout << " >>>> nC_: " << nC_.Value() << " \n" << endl;
		cout << " >>>> minOuter_: " << minOuter_.Value() << " \n" << endl;
		cout << " >>>> maxOuter_: " << maxOuter_.Value() << " \n" << endl;
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
