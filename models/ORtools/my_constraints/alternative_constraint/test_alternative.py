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
utilNodes[(0,1)] = solver.FixedDurationIntervalVar(0, maxHorizon, 20,True,'UtilNode_0_1')
utilNodes[(0,2)] = solver.FixedDurationIntervalVar(0, maxHorizon, 20,True,'UtilNode_0_2')
utilNodes[(0,3)] = solver.FixedDurationIntervalVar(0, maxHorizon, 20,True,'UtilNode_0_3')
utilNodes[(1,1)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_1')
utilNodes[(1,2)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_2')
utilNodes[(1,3)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_3')
utilNodes[(1,4)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_4')
utilNodes[(1,5)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_5')
utilNodes[(1,6)] = solver.FixedDurationIntervalVar(0, maxHorizon, 40,True,'UtilNode_1_6')
utilNodes[(2,1)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_1')
utilNodes[(2,2)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_2')
utilNodes[(2,3)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_3')
utilNodes[(2,4)] = solver.FixedDurationIntervalVar(20, maxHorizon, 50,True,'UtilNode_2_4')


#
# Add the constraints
# 
alternativeCons0 = pycp.AlternativeConstraint(solver, task[0], [utilNodes[(0,1)],utilNodes[(0,2)],utilNodes[(0,3)]], 1)
solver.Add(alternativeCons0)

alternativeCons1 = pycp.AlternativeConstraint(solver, task[1], [utilNodes[(1,1)],utilNodes[(1,2)],utilNodes[(1,3)],utilNodes[(1,4)],utilNodes[(1,5)],utilNodes[(1,6)]], 4)
solver.Add(alternativeCons1)

alternativeCons2 = pycp.AlternativeConstraint(solver, task[2], [utilNodes[(2,1)],utilNodes[(2,2)],utilNodes[(2,3)],utilNodes[(2,4)]], 0)
solver.Add(alternativeCons2)

#fix 
task[0].SetStartMin(10)
utilNodes[(0,1)].SetPerformed(False)
utilNodes[(0,2)].SetStartMin(5)
utilNodes[(1,0)].SetPerformed(True)
utilNodes[(1,1)].SetStartMin(20)
utilNodes[(1,2)].SetPerformed(False)
utilNodes[(1,2)].SetStartMin(30)
utilNodes[(1,3)].SetStartMax(40)
utilNodes[(1,4)].SetEndMax(90)
utilNodes[(2,0)].SetPerformed(False)
utilNodes[(2,1)].SetStartMin(50)


#
# Define search method
#
db = solver.Phase(task.values() + utilNodes.values() , solver.INTERVAL_DEFAULT)

#
# Search
#
print "Printing all solutions"
solver.NewSearch(db)
solved = False

while solver.NextSolution():
    solved = True
    print '---------  Task 0 ----------'
    print task[0]
    print '---------  Task 1 ----------'
    print task[1]
    print '---------  Task 2 ----------'
    print task[2]
    print '---------  UtilNode 0 1 ----------'
    print utilNodes[(0,1)]
    print '---------  UtilNode 0 2 ----------'
    print utilNodes[(0,1)]
    print '---------  UtilNode 0 3 ----------'
    print utilNodes[(0,1)]
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
    print '---------  UtilNode 1 6 ----------'
    print utilNodes[(1,6)]
    print '---------  UtilNode 2 1 ----------'
    print utilNodes[(2,1)]
    print '---------  UtilNode 2 2 ----------'
    print utilNodes[(2,22)]
    print '---------  UtilNode 2 3 ----------'
    print utilNodes[(2,3)]

if not solved:
    print "No solution"

solver.EndSearch()
