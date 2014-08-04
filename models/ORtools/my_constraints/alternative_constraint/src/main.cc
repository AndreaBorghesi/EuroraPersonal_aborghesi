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

#define HORIZON 100

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
	IntervalVar* const tasks0 = solver.MakeFixedDurationIntervalVar(0,HORIZON,20,true,"Task_0");
	IntervalVar* const tasks1 = solver.MakeFixedDurationIntervalVar(0,HORIZON,40,true,"Task_1");
	IntervalVar* const tasks2 = solver.MakeFixedDurationIntervalVar(20,HORIZON,50,true,"Task_2");
	std::vector<IntervalVar*> innerIntervals0;
	for(int i = 0; i<3; i++){
		const std::string name = StringPrintf("Inner_Interval_0_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(0,HORIZON,20,true,name);
		innerIntervals0.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals1;
	for(int i = 0; i<6; i++){
		const std::string name = StringPrintf("Inner_Interval_1_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(0,HORIZON,40,true,name);
		innerIntervals1.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals2;
	for(int i = 0; i<4; i++){
		const std::string name = StringPrintf("Inner_Interval_2_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(20,HORIZON,50,true,name);
		innerIntervals2.push_back(innerInterval);
	}
	int subUnitsNumber0 = 1;
	int subUnitsNumber1 = 4;
	int subUnitsNumber2 = 2;

	/* build alternative constraint */
        Constraint* cst0 = MakeAlternativeConstraint(&solver, tasks0, innerIntervals0, subUnitsNumber0);
        solver.AddConstraint(cst0);
	Constraint* cst1 = MakeAlternativeConstraint(&solver, tasks1, innerIntervals1, subUnitsNumber1);
	solver.AddConstraint(cst1);
	Constraint* cst2 = MakeAlternativeConstraint(&solver, tasks2, innerIntervals2, subUnitsNumber2);
        solver.AddConstraint(cst2);


        cout << "Constraint 0, initial status: " << cst0 << "\n" << endl;
        cout << "Constraint 1, initial status: " << cst1 << "\n" << endl;
        cout << "Constraint 2, initial status: " << cst2 << "\n" << endl;


        /* fixed intervals */
        tasks0->SetStartMin(10);
	tasks2->SetEndMax(85);

        innerIntervals0[0]->SetPerformed(false);
        innerIntervals0[1]->SetStartMin(5);
        innerIntervals0[2]->SetEndMax(60);

	innerIntervals1[0]->SetPerformed(true);
	innerIntervals1[0]->SetStartMin(20);
        innerIntervals1[1]->SetPerformed(false);
	innerIntervals1[1]->SetStartMin(30);
	innerIntervals1[2]->SetStartMax(40);
	innerIntervals1[3]->SetEndMax(90);

	innerIntervals2[0]->SetStartMin(25);
	innerIntervals2[0]->SetPerformed(false);
        innerIntervals2[3]->SetStartMin(25);

	// Creates array of end_times of jobs.
	std::vector<IntVar*> all_ends;
	all_ends.push_back(tasks0->EndExpr()->Var());
	all_ends.push_back(tasks1->EndExpr()->Var());
	all_ends.push_back(tasks2->EndExpr()->Var());

	// Create array for all the intervals
	std::vector<IntervalVar*> all_intervals;
        all_intervals.push_back(tasks0);
        all_intervals.push_back(tasks1);
        all_intervals.push_back(tasks2);
        all_intervals.push_back(innerIntervals0[0]);
        all_intervals.push_back(innerIntervals0[1]);
        all_intervals.push_back(innerIntervals0[2]);
        all_intervals.push_back(innerIntervals1[0]);
        all_intervals.push_back(innerIntervals1[1]);
        all_intervals.push_back(innerIntervals1[2]);
        all_intervals.push_back(innerIntervals1[3]);
        all_intervals.push_back(innerIntervals1[4]);
        all_intervals.push_back(innerIntervals1[5]);
	all_intervals.push_back(innerIntervals2[0]);
        all_intervals.push_back(innerIntervals2[1]);
        all_intervals.push_back(innerIntervals2[2]);
        all_intervals.push_back(innerIntervals2[3]);

	// Objective: minimize the makespan (maximum end times of all tasks)
	IntVar* const objective_var = solver.MakeMax(all_ends)->Var();
	OptimizeVar* const objective_monitor = solver.MakeMinimize(objective_var, 1);

	cout << "objective_var: " << objective_var << "\n" << endl;
	cout << "objective_monitor: " << objective_monitor << "\n" << endl;

	// ----- Search monitors and decision builder -----
	DecisionBuilder* const interval_phase = solver.MakePhase(all_intervals, Solver::INTERVAL_DEFAULT);

	DecisionBuilder* const obj_phase = solver.MakePhase(objective_var, Solver::CHOOSE_FIRST_UNBOUND, Solver::ASSIGN_MIN_VALUE);

	DecisionBuilder* const main_phase = solver.Compose(interval_phase, obj_phase);

	// Search log.
	const int kLogFrequency = 1000000;
	SearchMonitor* const search_log = solver.MakeSearchLog(kLogFrequency, objective_monitor);

	SearchLimit* limit = NULL;
	limit = solver.MakeTimeLimit(3000000);

	SolutionCollector* const collector = solver.MakeLastSolutionCollector();
	collector->Add(all_intervals);

	// Search
	//if (solver.Solve(main_phase, search_log, objective_monitor, limit,collector))
	//	cout << "Solution: " << collector->solution(0)->Value(all_intervals) << "\n" << endl;
	//
	
	solver.NewSearch(main_phase);
	//cst0->PostAndPropagate();
	//cst1->PostAndPropagate();
	//cst2->PostAndPropagate();
	cout << "Constraint 0, after NewSearch: " << cst0 << "\n" << endl;
        cout << "Constraint 1, after NewSearch: " << cst1 << "\n" << endl;
        cout << "Constraint 2, after NewSearch: " << cst2 << "\n" << endl;


	int solved = 0;

	while(solver.NextSolution()){

		solved = 1;

		cout << " ----- Task 0 ----- " << endl;
		cout << tasks0 << endl;
		cout << " ----- Task 1 ----- " << endl;
		cout << tasks1 << endl;
		cout << " ----- Task 2 ----- " << endl;
		cout << tasks2 << endl;
		cout << " ----- Util Node 0 0 ----- " << endl;
		cout << innerIntervals0[0] << endl;
		cout << " ----- Util Node 0 1 ----- " << endl;
		cout << innerIntervals0[1] << endl;
		cout << " ----- Util Node 0 2 ----- " << endl;
		cout << innerIntervals0[2] << endl;
		cout << " ----- Util Node 1 0 ----- " << endl;
		cout << innerIntervals1[0] << endl;
		cout << " ----- Util Node 1 1 ----- " << endl;
		cout << innerIntervals1[1] << endl;
		cout << " ----- Util Node 1 2 ----- " << endl;
		cout << innerIntervals1[2] << endl;
		cout << " ----- Util Node 1 3 ----- " << endl;
		cout << innerIntervals1[3] << endl;
		cout << " ----- Util Node 1 4 ----- " << endl;
		cout << innerIntervals1[4] << endl;
		cout << " ----- Util Node 1 5 ----- " << endl;
		cout << innerIntervals1[5] << endl;
		cout << " ----- Util Node 2 0 ----- " << endl;
		cout << innerIntervals2[0] << endl;
		cout << " ----- Util Node 2 1 ----- " << endl;
		cout << innerIntervals2[1] << endl;
		cout << " ----- Util Node 2 2 ----- " << endl;
		cout << innerIntervals2[2] << endl;
		cout << " ----- Util Node 2 3 ----- " << endl;
		cout << innerIntervals2[3] << endl;

		cout << "SOLUTION\n" << endl;
		cout << "objective_var: " << objective_var << "\n" << endl;
		cout << "objective_monitor: " << objective_monitor << "\n" << endl;
	}

	if(!solved)
		cout << "No Solution found " << endl;

	solver.EndSearch();


        return 0;        
    }
    

int main(int argc, char **argv)
{
	int result;

	cout << " -------------------- Test --------------\n" << endl;
	result = test_alternative();
   
	return 0;
}
