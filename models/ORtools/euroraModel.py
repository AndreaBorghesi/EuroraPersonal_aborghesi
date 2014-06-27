from google.apputils import app
import gflags
from ortools.constraint_solver import pywrapcp
import EuroraNode
import EuroraJob
import EuroraQueue
import sys
import datetime

FLAGS = gflags.FLAGS

gflags.DEFINE_integer('time_limit', 60*1000, 'global time limit')

gflags.DEFINE_string('queuesFile', 'data/code', 'path to data file')
gflags.DEFINE_string('nodesFile', 'data/nodi', 'path to data file')
gflags.DEFINE_string('nomFreqFile', 'data/NFreq_CPU_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('rFreqMEMFile', 'data/RFreq_Mem_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('rFreqCPUFile', 'data/RFreq_CPU_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('energyMEMFile', 'data/Energy_Mem_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('energyCPUFile', 'data/Energy_CPU_Bound.logprocessed', 'path to data file')


''' Compute the minumum number of nodes needed by a job, considering its requests '''
def numberOfJointNodes(job,node):
    r = job.nNodes
    if(job.nCores!=0):
        r = min(r,node.nCores/job.nCores/job.nNodes)
    if(job.nGpus!=0):
        r = min(r,node.nGpus/job.nGpus/job.nNodes)
    if(job.nMics!=0):
        r = min(r,node.nMics/job.nMics/job.nNodes)
    if(job.memReq!=0):
        r = min(r,node.mem/job.memReq/job.nNodes)


#''' Alternative (CPOptimizer like) constraint '''
#def alternative(outerInterval, intervalList, nIntervals):
#    for interval in intervalList:
#        interval.SetStartMin(outerInterval.StartMin())


def main(unused_argv):


    refTime = datetime.datetime(2014,03,31,14,00,00)  # reference time --> should be changing in rolling horizon

    # -----  Read system info  -----

    nr = EuroraNode.NodeReader()
    nr.readNodes(FLAGS.nodesFile)
    nr.updateNodes(FLAGS.nomFreqFile,FLAGS.rFreqMEMFile, FLAGS.rFreqCPUFile, FLAGS.energyMEMFile, FLAGS.energyCPUFile)
    #nr.printTest()

    qr = EuroraQueue.QueueReader()
    qr.readQueues(FLAGS.queuesFile)
    #qr.printTest()

    jobsFile = sys.argv[1]
    jr = EuroraJob.JobReader()
    jr.readJobs(jobsFile)
    #jr.printTest()
    # getDuration() test
#    atTime = datetime.datetime(2014,03,31,14,00,00)
#    for j,job in jr.jobs.items():
#        print job.humanPrintJob()
#        print job.getDuration_asTime(atTime)
#        print job.getStart_asInt(atTime)
#        print job.getDuration_asInt(atTime)


    # -----  Compute node efficiencies  -----
    
    energyEfficiencies = {}
    for n,node in nr.nodes.items():
        # efficiency as average of MEM and CPU bounds applications
        eff = (node.energyMEMbound + node.energyCPUbound)/2
        energyEfficiencies[n] = eff
    # normalize energy efficiencies
    maxEff = max(energyEfficiencies.values())
    for k,v in energyEfficiencies.items():
        v = v/maxEff


    # -----  Model  -----


    solver = pywrapcp.Solver('Summer School Contest')


    #  Task variable, one for each job
    maxHorizon = sum([job.getDuration_asInt(refTime) for j,job in jr.job.items()])
    task = {}
    for i, job in jr.jobs.items():
        # FixedDurationIntervalVar --> just to begin
        task[i] = solver.FixedDurationIntervalVar(refTime, maxHorizon, job.getDuration_asInt(refTime),False,'Task_%i' %i)

        if(job.isScheduled() && jobs.startTime_asDate < refTime):  # if the job is already scheduled we know its start time
            task[i].SetStartMin(job.getStart_asInt(refTime))
            task[i].SetStartMax(job.getStart_asInt(refTime))
        else:
            task[i].SetStartMin(refTime)


    # job on nodes variable, one for each job, node and sub-jobs (part of job executing in one node)
    utilNodes = {}
    for i,job in jr.jobs.items():
        usedNodes = job.usedNodes
        for j,node in nr.nodes.items():
            for k in range(numberOfJointNodes(job,node)):
                utilNodes[(i,j,k)] = solver.FixedDurationIntervalVar(refTime,maxHorizon,
                        job.getDuration_asInt(refTime),True,'UtilNode_%i_%i_%i' %i %j %k)
                if(job.isScheduled() && jobs.startTime_asDate < refTime):  # if the job is already scheduled 
                    if(usedNodes[j]>=1):
                        utilNodes[(i,j,k)].SetStartMin(job.getStart_asInt(refTime))
                        utilNodes[(i,j,k)].SetStartMax(job.getStart_asInt(refTime))
                        usedNodes[j]-=1       # one of the used nodes has been considered
                    else:
                        utilNodes[(i,j,k)].SetPerformed(False)
                else:
                    utilNodes[(i,j,k)].SetStartMin(refTime)


    # alternative constraint ---> NO, this a series of disjunctive constraints, not exaclty an alternative
    #for i, job in jr.jobs.items():
    #    alternativeJobs = []
    #    for j,node in nr.nodes.items():
    #        for k in range(numberOfJointNodes(job,node)):
    #            alternativeJobs.append(utilNodes[(i,j,k)])
    #    disj = solver.DisjunctiveConstraint(alternativeJobs, 'alternative %i' % i)
    #    solver.Add(disj)


    # cumulative contraints cores
    for j,node in nr.nodes.items():
        jobIntervals = []
        coresRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                coresRequested.append(job.cpuReq/job.nodeReq)
        cumul = solver.MakeCumulative(jobIntervals,coresRequested,node.nCores,'cumulativeCore_onNode_%i' % j)
        solver.Add(cumul)


    # cumulative contraints gpu
    for j,node in nr.nodes.items():
        jobIntervals = []
        gpusRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                gpusRequested.append(job.gpuReq/job.nodeReq)
        cumul = solver.MakeCumulative(jobIntervals,gpusRequested,node.nGpus,'cumulativeGPU_onNode_%i' % j)
        solver.Add(cumul)


    # cumulative contraints mic 
    for j,node in nr.nodes.items():
        jobIntervals = []
        micsRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                micsRequested.append(job.micReq/job.nodeReq)
        cumul = solver.MakeCumulative(jobIntervals,micsRequested,node.nMics,'cumulativeMIC_onNode_%i' % j)
        solver.Add(cumul)


    # cumulative contraints mem 
    for j,node in nr.nodes.items():
        jobIntervals = []
        memRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                memRequested.append(job.memReq/job.nodeReq)
        cumul = solver.MakeCumulative(jobIntervals,memRequested,node.mem,'cumulativeMem_onNode_%i' % j)
        solver.Add(cumul)

    

    

#    solver = pywrapcp.Solver('Summer School Contest')
#
#    item_prev_delay = [solver.IntVar(0, n_slots-n_items, 'item_prev_delay%d' % i)  for i in range(n_items)]
#    solver.Add(solver.Sum(item_prev_delay) == n_slots-n_items)
#
#    item_prod_idx = [solver.IntVar(0, n_items, 'item_prod_idx%d' % i)  for i in range(n_items)]
#    solver.Add(solver.AllDifferent(item_prod_idx))
#    
#    item_prod_date = [solver.IntVar(0, item_due_date[i], 'item_prod_date%d' % i)  for i in range(n_items)]
#    for i in range(n_items):
#        solver.Add(solver.Sum(item_prev_delay[:i]) + item_prod_idx[i] == item_prod_date[i])
#    solver.Add(solver.AllDifferent(item_prod_date))
#
#    machine_program = [solver.IntVar(0, n_products, 'machine_program%d' % i)  for i in range(n_items)]
#    for i in range(n_items):
#        solver.Add(solver.Element(machine_program, item_prod_idx[i]) == item_product_type[i])
#    
#    inventory_total_diff = solver.Sum([item_due_date[i] - item_prod_date[i] for i in range(n_items)])
#    inventory_obj = inventory_total_diff * inventory_cost
#    
#    index_var = [solver.IntVar(0, n_products*n_products-1, 'index_var%d' % i)  for i in range(n_items-1)]
#    for i in range(n_items-1):
#        solver.Add(index_var[i] == machine_program[i]*n_products + machine_program[i+1]);
#    transition_obj = solver.Sum([solver.Element(transition_cost, index_var[i]) for i in range(n_items-1)])
#    objective_var = solver.Sum([inventory_obj, transition_obj]).Var()
#    objective = solver.Minimize(objective_var, 1)
#    
#    db = solver.Phase(item_prev_delay + item_prod_idx + item_prod_date + machine_program, solver.CHOOSE_MIN_SIZE_HIGHEST_MAX, solver.ASSIGN_RANDOM_VALUE)
#    search_log = solver.SearchLog(100000, objective_var)
#    global_limit = solver.TimeLimit(FLAGS.time_limit)
#    restart_monitor = solver.LubyRestart(5)
#    solver.NewSearch(db, [objective, search_log, global_limit, restart_monitor])
#    
#    while solver.NextSolution():
#        print 'Objective:', objective_var.Value()
#        s = GetSolution(item_prod_date, item_product_type, n_items, n_slots)
#        print ' '.join(['%2d' % v for v in s])
#    solver.EndSearch()

if __name__ == '__main__':
    app.run()
