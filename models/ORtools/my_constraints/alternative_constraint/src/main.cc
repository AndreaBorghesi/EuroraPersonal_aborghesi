// Copyright 2010-2011 Google
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "base/commandlineflags.h"
#include "base/integral_types.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "constraint_solver/constraint_solver.h"

#include "alternative_cst.h"

using namespace operations_research;
using namespace std;

#define HORIZON 20
#define DURATION 10

//==============================================================================
// Main program
//==============================================================================


    
    //
    // Alternative Constraint
    //
    
    int test_alternative()
    {

        /* build the solver */
        Solver solver("Test alternative constraint");

        /* build variables  */
	IntervalVar* const outerInterval = solver.MakeFixedDurationIntervalVar(0,HORIZON,DURATION,true,"Outer_Interval");

	std::vector<IntervalVar*> innerIntervals;
	for(int i = 0; i<=4; i++){
		const std::string name = StringPrintf("Inner_Interval_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(0,HORIZON,DURATION,true,name);
		innerIntervals.push_back(innerInterval);
	}

	int subUnitsNumber = 2;


        /* fixed inner intervals */
        innerIntervals[0]->SetStartMin(0);
        innerIntervals[0]->SetEndMax(HORIZON);
	innerIntervals[0]->SetPerformed(false);

	innerIntervals[1]->SetStartMin(4);
        innerIntervals[1]->SetEndMax(18);
	innerIntervals[1]->SetPerformed(true);

	innerIntervals[2]->SetStartMin(7);
        innerIntervals[2]->SetEndMax(19);
	innerIntervals[2]->SetPerformed(true);

	innerIntervals[3]->SetStartMin(8);
        innerIntervals[3]->SetEndMax(18);
	innerIntervals[3]->SetPerformed(false);

	innerIntervals[4]->SetStartMin(1);
        innerIntervals[4]->SetEndMax(11);


	/* fixed outer interval */
        //outerInterval->SetStartMin(1);
	//outerInterval->SetEndMax(15);
	//outerInterval->SetPerformed(true);
	//
        //innerIntervals[0]->SetStartMin(0);
        //innerIntervals[0]->SetEndMax(HORIZON);

	//innerIntervals[1]->SetStartMin(4);
        //innerIntervals[1]->SetEndMax(18);

	//innerIntervals[2]->SetStartMin(7);
        //innerIntervals[2]->SetEndMax(19);

	//innerIntervals[3]->SetStartMin(8);
        //innerIntervals[3]->SetEndMax(18);

	//innerIntervals[4]->SetStartMin(1);
        //innerIntervals[4]->SetEndMax(11);




        /* build alternative constraint */
        Constraint* cst = MakeAlternativeConstraint(&solver, outerInterval, innerIntervals, subUnitsNumber);

        solver.AddConstraint(cst);
        cout << "Initial constraint status: " << cst << "\n" << endl;
        
        /* propagate the alternative constraint */
        cst->PostAndPropagate();
        cout << "Constraint after propagation: " << cst << "\n" << endl;	
        
        /* search */
        //cout << "Outer interval: " << outerInterval << endl;	
        //cout << "Outer interval min start: " << outerInterval->StartMin() << endl;	
        //cout << "Outer interval max end: " << outerInterval->EndMax() << endl;	
        //cout << "Outer interval must be performed: " << outerInterval->MustBePerformed() << endl;	

        //for(int i = 0; i<=4; i++){
	//	cout << "Inner interval " << i << " : " << innerIntervals[i] << endl;	
	//	cout << "Inner interval " << i << " must be performed: " << innerIntervals[i]->MustBePerformed() << endl;
	//	if(innerIntervals[i]->MayBePerformed()){
	//		cout << "Inner interval " << i << " min start: " << innerIntervals[i]->StartMin() << endl;	
	//		cout << "Inner interval " << i << " max end: " << innerIntervals[i]->EndMax() << endl;	
	//	}
	//}
	

	innerIntervals[1]->SetStartMin(5);
        cout << "Constraint after range change: " << cst << endl;	
        
        
        return 0;        
    }
    

int main(int argc, char **argv)
{
	int result;

	result = test_alternative();	
	    
	return 0;
}
