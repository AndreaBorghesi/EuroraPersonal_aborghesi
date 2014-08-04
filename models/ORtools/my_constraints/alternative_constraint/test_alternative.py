#!/usr/bin/env python

# Copyright 2014 Andrea Borghesi <andre.borghesi3@unibo.it>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pywrapcp_acst as pycp

#
# data
#
maxHorizon = 100

def print_solution():
    print '---------  Task 0 ----------'
    print task[0]
    print '---------  Task 1 ----------'
    print task[1]
    print '---------  Task 2 ----------'
    print task[2]
    print '---------  UtilNode 0 0 ----------'
    print utilNodes[(0,0)]
    print '---------  UtilNode 0 1 ----------'
    print utilNodes[(0,1)]
    print '---------  UtilNode 0 2 ----------'
    print utilNodes[(0,2)]
    print '---------  UtilNode 1 0 ----------'
    print utilNodes[(1,0)]
    print '---------  UtilNode 1 1 ----------'
    print utilNodes[(1,1)]
    print '---------  UtilNode 1 2 ----------'
    print utilNodes[(1,2)]
    print '---------  UtilNode 1 3 ----------'
    print utilNodes[(1,3)]
    print '---------  UtilNode 1 4 ----------'
    print utilNodes[(1,4)]
    print '---------  UtilNode 1 5 ----------'
    print utilNodes[(1,5)]
    print '---------  UtilNode 2 0 ----------'
    print utilNodes[(2,0)]
    print '---------  UtilNode 2 1 ----------'
    print utilNodes[(2,1)]
    print '---------  UtilNode 2 2 ----------'
    print utilNodes[(2,2)]
    print '---------  UtilNode 2 3 ----------'
    print utilNodes[(2,3)]
    print '--------- Best Objective Value ----------'
    print objective.Best()


#
# Create the solver.
#
solver = pycp.Solver('Alternative Constraint Test')

#
# declare variables
#
task = {}
task[0] = solver.FixedDurationIntervalVar(0, maxHorizon, 20,True,'Task_0')
task[1] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'Task_1')
task[2] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'Task_2')

utilNodes = {}
utilNodes[(0,0)] = solver.FixedDurationIntervalVar(0, maxHorizon, 20,True,'UtilNode_0_0')
utilNodes[(0,1)] = solver.FixedDurationIntervalVar(0, maxHorizon, 20,True,'UtilNode_0_1')
utilNodes[(0,2)] = solver.FixedDurationIntervalVar(0, maxHorizon, 20,True,'UtilNode_0_2')
utilNodes[(1,0)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_0')
utilNodes[(1,1)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_1')
utilNodes[(1,2)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_2')
utilNodes[(1,3)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_3')
utilNodes[(1,4)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_4')
utilNodes[(1,5)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_5')
utilNodes[(2,0)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_0')
utilNodes[(2,1)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_1')
utilNodes[(2,2)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_2')
utilNodes[(2,3)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_3')


#
# Add the constraints
# 
alternativeCons0 = pycp.AlternativeConstraint(solver, task[0], [utilNodes[(0,0)],utilNodes[(0,1)],utilNodes[(0,2)]], 1)
solver.Add(alternativeCons0)

alternativeCons1 = pycp.AlternativeConstraint(solver, task[1], [utilNodes[(1,0)],utilNodes[(1,1)],utilNodes[(1,2)],utilNodes[(1,3)],utilNodes[(1,4)],utilNodes[(1,5)]], 4)
solver.Add(alternativeCons1)

alternativeCons2 = pycp.AlternativeConstraint(solver, task[2], [utilNodes[(2,0)],utilNodes[(2,1)],utilNodes[(2,2)],utilNodes[(2,3)]], 2)
solver.Add(alternativeCons2)

#fix 
task[0].SetStartMin(10)
utilNodes[(0,0)].SetPerformed(False)
utilNodes[(0,1)].SetStartMin(5)
utilNodes[(0,2)].SetEndMax(60)
utilNodes[(1,0)].SetPerformed(True)
utilNodes[(1,0)].SetStartMin(20)
utilNodes[(1,1)].SetPerformed(False)
utilNodes[(1,1)].SetStartMin(30)
utilNodes[(1,2)].SetStartMax(40)
utilNodes[(1,3)].SetEndMax(90)
utilNodes[(2,0)].SetPerformed(False)
utilNodes[(2,0)].SetStartMin(25)
utilNodes[(2,3)].SetStartMin(25)
task[2].SetEndMax(85)

# objective: minimize makespan
obj_var = solver.Max([task[i].EndExpr() for i,job in task.items()])

#obj_var = task[0].EndExpr()
print obj_var
objective = solver.Minimize(obj_var, 1)

#
# Define search method
#
db = solver.Phase(task.values() + utilNodes.values() , solver.INTERVAL_DEFAULT)
#search_log = solver.SearchLog(100000, objective)
#restart_monitor = solver.LubyRestart(5)
#global_limit = solver.TimeLimit(10000)

#
# Search
#
#solver.NewSearch(db, [objective, search_log, global_limit, restart_monitor])
solver.NewSearch(db, [objective])
solved = False

#print "------------- CONSTRAINTS BEFORE NEXT SOLUTION -------------"
#print alternativeCons0
#print alternativeCons1
#print alternativeCons2

while solver.NextSolution():
    solved = True
    print_solution()

if not solved:
    print "No solution"

#print "------------- CONSTRAINTS AFTER SEARCH -------------"
#print alternativeCons0
#print alternativeCons1
#print alternativeCons2

solver.EndSearch()

