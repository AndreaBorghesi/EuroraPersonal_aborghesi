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
//
// Search strategy for EURORA or-tools model

#include "base/commandlineflags.h"
#include "base/integral_types.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "constraint_solver/constraint_solver.h"

#include "alternative_cst.h"

using namespace operations_research;
using namespace std;

#define HORIZON 200

//==============================================================================
// Main program
//==============================================================================
    
int test_search()
    {

        /* build the solver */
        Solver solver("Test search strategy");

        /* build variables  */
	// outer intervals (tasks)
	IntervalVar* const tasks0 = solver.MakeFixedDurationIntervalVar(0,HORIZON,20,true,"Task_0");
	IntervalVar* const tasks1 = solver.MakeFixedDurationIntervalVar(0,HORIZON,40,true,"Task_1");
	IntervalVar* const tasks2 = solver.MakeFixedDurationIntervalVar(20,HORIZON,50,true,"Task_2");
	IntervalVar* const tasks3 = solver.MakeFixedDurationIntervalVar(100,HORIZON,60,true,"Task_3");
	IntervalVar* const tasks4 = solver.MakeFixedDurationIntervalVar(150,HORIZON,40,true,"Task_4");
	IntervalVar* const tasks5 = solver.MakeFixedDurationIntervalVar(60,HORIZON,110,true,"Task_5");
	IntervalVar* const tasks6 = solver.MakeFixedDurationIntervalVar(40,HORIZON,80,true,"Task_6");
	IntervalVar* const tasks7 = solver.MakeFixedDurationIntervalVar(30,HORIZON,70,true,"Task_7");
	IntervalVar* const tasks8 = solver.MakeFixedDurationIntervalVar(0,HORIZON,50,true,"Task_8");
	IntervalVar* const tasks9 = solver.MakeFixedDurationIntervalVar(100,HORIZON,10,true,"Task_9");
	// Create array for all the intervals
	std::vector<IntervalVar*> all_intervals;
        all_intervals.push_back(tasks0);
        all_intervals.push_back(tasks1);
        all_intervals.push_back(tasks2);
	all_intervals.push_back(tasks3);
        all_intervals.push_back(tasks4);
        all_intervals.push_back(tasks5);
	all_intervals.push_back(tasks6);
        all_intervals.push_back(tasks7);
        all_intervals.push_back(tasks8);
	all_intervals.push_back(tasks9);
	// sub intervals (utilNodes)
	std::vector<IntervalVar*> innerIntervals0;
	for(int i = 0; i<3; i++){
		const std::string name = StringPrintf("Inner_Interval_0_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(0,HORIZON,20,true,name);
		innerIntervals0.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals1;
	for(int i = 0; i<6; i++){
		const std::string name = StringPrintf("Inner_Interval_1_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(0,HORIZON,40,true,name);
		innerIntervals1.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals2;
	for(int i = 0; i<4; i++){
		const std::string name = StringPrintf("Inner_Interval_2_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(20,HORIZON,50,true,name);
		innerIntervals2.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals3;
	for(int i = 0; i<2; i++){
		const std::string name = StringPrintf("Inner_Interval_3_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(100,HORIZON,60,true,name);
		innerIntervals3.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals4;
	for(int i = 0; i<3; i++){
		const std::string name = StringPrintf("Inner_Interval_4_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(150,HORIZON,40,true,name);
		innerIntervals4.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals5;
	for(int i = 0; i<1; i++){
		const std::string name = StringPrintf("Inner_Interval_5_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(60,HORIZON,110,true,name);
		innerIntervals5.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals6;
	for(int i = 0; i<6; i++){
		const std::string name = StringPrintf("Inner_Interval_6_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(40,HORIZON,80,true,name);
		innerIntervals6.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals7;
	for(int i = 0; i<8; i++){
		const std::string name = StringPrintf("Inner_Interval_7_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(30,HORIZON,70,true,name);
		innerIntervals7.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals8;
	for(int i = 0; i<10; i++){
		const std::string name = StringPrintf("Inner_Interval_8_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(0,HORIZON,50,true,name);
		innerIntervals8.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	std::vector<IntervalVar*> innerIntervals9;
	for(int i = 0; i<5; i++){
		const std::string name = StringPrintf("Inner_Interval_9_%d", i);
		IntervalVar* const innerInterval = solver.MakeFixedDurationIntervalVar(100,HORIZON,10,true,name);
		innerIntervals9.push_back(innerInterval);
		all_intervals.push_back(innerInterval);
	}
	int subUnitsNumber0 = 1;
	int subUnitsNumber1 = 4;
	int subUnitsNumber2 = 2;
	int subUnitsNumber3 = 1;
	int subUnitsNumber4 = 2;
	int subUnitsNumber5 = 1;
	int subUnitsNumber6 = 5;
	int subUnitsNumber7 = 6;
	int subUnitsNumber8 = 8;
	int subUnitsNumber9 = 3;


	/* build alternative constraint */
        Constraint* cst0 = MakeAlternativeConstraint(&solver, tasks0, innerIntervals0, subUnitsNumber0);
        solver.AddConstraint(cst0);
	Constraint* cst1 = MakeAlternativeConstraint(&solver, tasks1, innerIntervals1, subUnitsNumber1);
	solver.AddConstraint(cst1);
	Constraint* cst2 = MakeAlternativeConstraint(&solver, tasks2, innerIntervals2, subUnitsNumber2);
        solver.AddConstraint(cst2);
	Constraint* cst3 = MakeAlternativeConstraint(&solver, tasks3, innerIntervals3, subUnitsNumber3);
        solver.AddConstraint(cst3);
	Constraint* cst4 = MakeAlternativeConstraint(&solver, tasks4, innerIntervals4, subUnitsNumber4);
	solver.AddConstraint(cst4);
	Constraint* cst5 = MakeAlternativeConstraint(&solver, tasks5, innerIntervals5, subUnitsNumber5);
        solver.AddConstraint(cst5);
	Constraint* cst6 = MakeAlternativeConstraint(&solver, tasks6, innerIntervals6, subUnitsNumber6);
        solver.AddConstraint(cst6);
	Constraint* cst7 = MakeAlternativeConstraint(&solver, tasks7, innerIntervals7, subUnitsNumber7);
	solver.AddConstraint(cst7);
	Constraint* cst8 = MakeAlternativeConstraint(&solver, tasks8, innerIntervals8, subUnitsNumber8);
        solver.AddConstraint(cst8);
	Constraint* cst9 = MakeAlternativeConstraint(&solver, tasks9, innerIntervals9, subUnitsNumber9);
        solver.AddConstraint(cst9);


        //cout << "Constraint 0, initial status: " << cst0 << "\n" << endl;
        //cout << "Constraint 1, initial status: " << cst1 << "\n" << endl;
        //cout << "Constraint 2, initial status: " << cst2 << "\n" << endl;


        /* fix intervals */
        tasks0->SetStartMin(10);
	tasks2->SetEndMax(85);
	tasks1->SetPerformed(true);
	tasks3->SetPerformed(true);
	tasks4->SetPerformed(true);
	tasks5->SetPerformed(true);
	tasks6->SetPerformed(true);
	tasks7->SetPerformed(true);
	tasks8->SetPerformed(true);
	tasks9->SetPerformed(true);

        innerIntervals0[0]->SetPerformed(false);
        innerIntervals0[1]->SetStartMin(5);
        innerIntervals0[2]->SetEndMax(60);

	innerIntervals1[0]->SetPerformed(true);
        innerIntervals1[1]->SetPerformed(false);
	innerIntervals1[0]->SetStartMin(20);
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
	all_ends.push_back(tasks3->EndExpr()->Var());
	all_ends.push_back(tasks4->EndExpr()->Var());
	all_ends.push_back(tasks5->EndExpr()->Var());
	all_ends.push_back(tasks6->EndExpr()->Var());
	all_ends.push_back(tasks7->EndExpr()->Var());
	all_ends.push_back(tasks8->EndExpr()->Var());
	all_ends.push_back(tasks9->EndExpr()->Var());



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
	
	//cout << "Constraint 0, after NewSearch: " << cst0 << "\n" << endl;
        //cout << "Constraint 1, after NewSearch: " << cst1 << "\n" << endl;
        //cout << "Constraint 2, after NewSearch: " << cst2 << "\n" << endl;


	int solved = 0;

	while(solver.NextSolution()){

		solved = 1;

		cout << " ----- Task 0 ----- " << endl;
		cout << tasks0 << endl;
		cout << " ----- Task 1 ----- " << endl;
		cout << tasks1 << endl;
		cout << " ----- Task 2 ----- " << endl;
		cout << tasks2 << endl;
		cout << " ----- Task 3 ----- " << endl;
		cout << tasks3 << endl;
		cout << " ----- Task 4 ----- " << endl;
		cout << tasks4 << endl;
		cout << " ----- Task 5 ----- " << endl;
		cout << tasks5 << endl;
		cout << " ----- Task 6 ----- " << endl;
		cout << tasks6 << endl;
		cout << " ----- Task 7 ----- " << endl;
		cout << tasks7 << endl;
		cout << " ----- Task 8 ----- " << endl;
		cout << tasks8 << endl;
		cout << " ----- Task 9 ----- " << endl;
		cout << tasks9 << endl;
		for(int i = 0; i<3; i++){
			cout << " ----- Util Node 0 " << i << " ----- " << endl;
			cout << innerIntervals0[i] << endl;
		}
		for(int i = 0; i<6; i++){
			cout << " ----- Util Node 1 " << i << " ----- " << endl;
			cout << innerIntervals1[i] << endl;
		}
		for(int i = 0; i<4; i++){
			cout << " ----- Util Node 2 " << i << " ----- " << endl;
			cout << innerIntervals2[i] << endl;
		}
		for(int i = 0; i<2; i++){
			cout << " ----- Util Node 3 " << i << " ----- " << endl;
			cout << innerIntervals3[i] << endl;
		}
		for(int i = 0; i<3; i++){
			cout << " ----- Util Node 4 " << i << " ----- " << endl;
			cout << innerIntervals4[i] << endl;
		}
		for(int i = 0; i<1; i++){
			cout << " ----- Util Node 5 " << i << " ----- " << endl;
			cout << innerIntervals5[i] << endl;
		}
		for(int i = 0; i<6; i++){
			cout << " ----- Util Node 6 " << i << " ----- " << endl;
			cout << innerIntervals6[i] << endl;
		}
		for(int i = 0; i<8; i++){
			cout << " ----- Util Node 7 " << i << " ----- " << endl;
			cout << innerIntervals7[i] << endl;
		}
		for(int i = 0; i<10; i++){
			cout << " ----- Util Node 8 " << i << " ----- " << endl;
			cout << innerIntervals8[i] << endl;
		}
		for(int i = 0; i<5; i++){
			cout << " ----- Util Node 9 " << i << " ----- " << endl;
			cout << innerIntervals9[i] << endl;
		}

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
	result = test_search();
   
	return 0;
}
