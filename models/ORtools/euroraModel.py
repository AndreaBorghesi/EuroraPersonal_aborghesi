from google.apputils import app
import gflags
from ortools.constraint_solver import pywrapcp
import EuroraNode
from EuroraJob import Job
import EuroraJob
import EuroraQueue
import sys
import datetime

FLAGS = gflags.FLAGS

gflags.DEFINE_integer('time_limit', 30*1000, 'global time limit')

gflags.DEFINE_string('queuesFile', 'data/code', 'path to data file')
gflags.DEFINE_string('nodesFile', 'data/nodi', 'path to data file')
gflags.DEFINE_string('nomFreqFile', 'data/NFreq_CPU_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('rFreqMEMFile', 'data/RFreq_Mem_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('rFreqCPUFile', 'data/RFreq_CPU_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('energyMEMFile', 'data/Energy_Mem_Bound.logprocessed', 'path to data file')
gflags.DEFINE_string('energyCPUFile', 'data/Energy_CPU_Bound.logprocessed', 'path to data file')


def AlternativeConstraint(solver,outerInterval,intervals,n):
    pass
    outerInterval.SetStartMin(min([interval.StartMin() for interval in intervals]))
    outerInterval.SetEndMax(min([interval.EndMax() for interval in intervals]))
    ##solver.Add(solver.SumEquality([interval.PerformedExpr().Min() for interval in intervals], n * outerInterval.PerformedExpr().Min()))

    #solver.Add(solver.IsEqualCstVar(solver.Sum([interval.PerformedExpr().Var() for interval in intervals]), n * outerInterval.PerformedExpr().Var()))

    #print [interval.PerformedExpr().Var().Min() for interval in intervals]
    #print intervals
    #print str(n * outerInterval.PerformedExpr().Var().Min())



''' Compute the minumum number of nodes needed by a job, considering its requests '''
def numberOfJointNodes(job,node):
    r = job.nNodes
    if(job.nCores!=0):
        r = min(r,node.nCores/(job.nCores/job.nNodes))
    if(job.nGpus!=0):
        r = min(r,node.nGpus/(job.nGpus/job.nNodes))
    if(job.nMics!=0):
        r = min(r,node.nMics/(job.nMics/job.nNodes))
    if(job.memReq!=0):
        r = min(r,node.mem/(job.memReq/job.nNodes))
    return r


''' Produce a solution '''
def GetSolution(task,utilNodes,refTime,refTime_asDate,jobs,nodes,alternativeCons,cumlCons_cores,cumlCons_gpus,cumlCons_mics,cumlCons_mem):    
    #print jobs
    #print task
    #print utilNodes
    #print jobs[10].humanPrintJob()
    for i, job in jobs.items():
        # schedule execution starting times
        relativeStart = datetime.timedelta(seconds=task[i].StartMin())
        relativeEnd = datetime.timedelta(seconds=task[i].EndMax())
        job.runStartTime_asDate = refTime_asDate + relativeStart
        job.endTime_asDate = refTime_asDate + relativeEnd

        # allocate job in used nodes
        usedNodes = []
        for i in range(len(nodes)):
            usedNodes.append(0)
        #print "usedNodes before "
        #print usedNodes
        #print "node items len " + str(len(nodes.items()))
        for i,job in jobs.items():
            for s in range(len(usedNodes)):
                usedNodes[s] = 0
            for j,node in nodes.items():
                for k in range(numberOfJointNodes(job,node)):
                    #print str(utilNodes[(i,j,k)].MustBePerformed())
                    #print str(utilNodes[(i,j,k)])
                    if(utilNodes[(i,j,k)].MustBePerformed()):
                        usedNodes[int(j)-1]+=1   # minus one since usedNodes uses j as an index, whereas it could vary from 1 to 64
            #print "usedNodes after"
            #print usedNodes
            job.allocate(usedNodes)
        #print "\n"
        #print job.humanPrintJob()
    #print "\nafter\n" + jobs[10].humanPrintJob()

    print "After (1) solution found" 
    debugPrint(task,utilNodes,refTime,refTime_asDate,jobs,nodes,alternativeCons,cumlCons_cores,cumlCons_gpus,cumlCons_mics,cumlCons_mem)

    


def debugPrint(task,utilNodes,refTime,refTime_asDate,jobs,nodes,alternativeCons,cumlCons_cores,cumlCons_gpus,cumlCons_mics,cumlCons_mem):

    # Util nodes
    print "------ Util Nodes ------"
    countPerformed = 0
    countMaybe = 0
    countUnperformed = 0
    countBoundPerf = 0
    perfExpr = []
    for i,job in jobs.items():
        for j,node in nodes.items():
            for k in range(numberOfJointNodes(job,node)):
                #print str(utilNodes[(i,j,k)])
                if(utilNodes[(i,j,k)].MayBePerformed):
                    countMaybe+=1
                if(utilNodes[(i,j,k)].MustBePerformed()):
                    countPerformed+=1
                if(utilNodes[(i,j,k)].CannotBePerformed()):
                    countUnperformed+=1
                if(utilNodes[(i,j,k)].IsPerformedBound()):
                    countBoundPerf+=1
                perfExpr.append(utilNodes[(i,j,k)].PerformedExpr().Var().Min())
    print "N. utilNodes: " + str(len(utilNodes))
    print "N. performed: " + str(countPerformed)
    print "N. maybe performed: " + str(countMaybe)
    print "N. un-performed: " + str(countUnperformed)
    print "N. performed bound: " + str(countBoundPerf)
    #print "Performed expressions: " + str(perfExpr)

    print "------ Task ------" 
    for i,t in task.items():
        print t

    #print "----- Alternative Constraints -----"
    #for a,ac in alternativeCons.items():
    #    print ac
    #    if(ac!=None):
    #        print ac.Var()

    #print "----- Cumulative Constraints Cores -----"
    #for c,cc in cumlCons_cores.items():
    #    print cc
    #    print cc.Var()

    #print "----- Cumulative Constraints Gpus -----"
    #for c,cc in cumlCons_gpus.items():
    #    print cc
    #    print cc.Var()

    #print "----- Cumulative Constraints Mics -----"
    #for c,cc in cumlCons_mics.items():
    #    print cc
    #    print cc.Var()

    #print "----- Cumulative Constraints Mem -----"
    #for c,cc in cumlCons_mem.items():
    #    print cc
    #    print cc.Var()


def main(unused_argv):


    #refTime = datetime.datetime(2014,03,31,14,00,00)  # reference time --> should be changing in rolling horizon
    refTime = 0

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
    refTime_asDate=min([job.startTime_asDate for j,job in jr.jobs.items()])
    #print refTime_asDate
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


    solver = pywrapcp.Solver('MOdel')


    #  Task variable, one for each job
    maxHorizon = sum([job.getDuration_asInt(refTime_asDate) for j,job in jr.jobs.items()])
    task = {}
    for i, job in jr.jobs.items():
        # FixedDurationIntervalVar --> just to begin
#        print job.getDuration_asInt(refTime_asDate)
#        print i
#        print refTime
        task[i] = solver.FixedDurationIntervalVar(refTime, maxHorizon, job.getDuration_asInt(refTime_asDate),True,'Task_%i' % (i))
#        print task[i]

        if(job.isScheduled and job.runStartTime_asDate < refTime_asDate):  # if the job is already scheduled we know its start time
            task[i].SetStartMin(job.getStart_asInt(refTime_asDate))
            task[i].SetStartMax(job.getStart_asInt(refTime_asDate))
        else:
            task[i].SetStartMin(refTime)


    # job on nodes variable, one for each job, node and sub-jobs (part of job executing in one node)
    utilNodes = {}
    for i,job in jr.jobs.items():
        for j,node in nr.nodes.items():
            for k in range(numberOfJointNodes(job,node)):
                utilNodes[(i,j,k)] = solver.FixedDurationIntervalVar(refTime,maxHorizon,
                        job.getDuration_asInt(refTime_asDate),True,'UtilNode_%s_%s_%i' % (i,j,k))
                if(job.isScheduled and job.runStartTime_asDate < refTime_asDate):  # if the job is already scheduled 
                    if(job.usedNodes[j]>=1):
                        utilNodes[(i,j,k)].SetStartMin(job.getStart_asInt(refTime_asDate))
                        utilNodes[(i,j,k)].SetStartMax(job.getStart_asInt(refTime_asDate))
                        job.usedNodes[j]-=1       # one of the used nodes has been considered
                    else:
                        utilNodes[(i,j,k)].SetPerformed(False)
                else:
                    utilNodes[(i,j,k)].MayBePerformed=True
                    utilNodes[(i,j,k)].SetStartMin(refTime)


     # alternative constraint

#    self._outerInterval.SetStartMin(min([interval.StartMin() for interval in self._innerIntervals]))
#    self._outerInterval.SetEndMax(min([interval.EndMax() for interval in self._innerIntervals]))
#    solver.Add(solver.SumEqual([interval.PerformedExpr() for interval in self._innerIntervals], n * self._outerInterval.PerformedExpr()))


    alternativeCons = {}
    for i,job in jr.jobs.items():
        alternativeJobs = []
        for j,node in nr.nodes.items():
            for k in range(numberOfJointNodes(job,node)):
                alternativeJobs.append(utilNodes[(i,j,k)])
        #print "-------" + str(alternativeJobs)
        altCons = AlternativeConstraint(solver,task[i],alternativeJobs,job.nNodes)
    #    solver.Add(altCons)
        alternativeCons[i]=altCons


    # cumulative constraints cores
    cumlCons_cores = {}
    for j,node in nr.nodes.items():
        jobIntervals = []
        coresRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                coresRequested.append(job.cpuReq/job.nodeReq)
        cumul = solver.Cumulative(jobIntervals,coresRequested,node.nCores,'cumulativeCore_onNode_%s' % (j))
        solver.Add(cumul)
        cumlCons_cores[j]=cumul


    # cumulative constraints gpu
    cumlCons_gpus = {}
    for j,node in nr.nodes.items():
        jobIntervals = []
        gpusRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                gpusRequested.append(job.nGpus/job.nodeReq)
        cumul = solver.Cumulative(jobIntervals,gpusRequested,node.nGpus,'cumulativeGPU_onNode_%s' % (j))
        solver.Add(cumul)
        cumlCons_gpus[j]=cumul


    # cumulative constraints mic 
    cumlCons_mics = {}
    for j,node in nr.nodes.items():
        jobIntervals = []
        micsRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                micsRequested.append(job.nMics/job.nodeReq)
        cumul = solver.Cumulative(jobIntervals,micsRequested,node.nMics,'cumulativeMIC_onNode_%s' % (j))
        solver.Add(cumul)
        cumlCons_mics[j]=cumul


    # cumulative constraints mem 
    cumlCons_mem = {}
    for j,node in nr.nodes.items():
        jobIntervals = []
        memRequested = []
        for i,job in jr.jobs.items():
            for k in range(numberOfJointNodes(job,node)):
                jobIntervals.append(utilNodes[(i,j,k)])
                memRequested.append(job.memReq/job.nodeReq)
        cumul = solver.Cumulative(jobIntervals,memRequested,node.mem,'cumulativeMem_onNode_%s' % (j))
        solver.Add(cumul)
        cumlCons_mem[j]=cumul

    
    # Makespan objective
    obj_var = solver.Max([task[i].EndExpr() for i,job in jr.jobs.items()])
    #obj_var = solver.Max([utilNodes[(i,j,k)].EndExpr() for i,job in jr.jobs.items() for j,node in nr.nodes.items() for k in range(numberOfJointNodes(job,node))])
    objective = solver.Minimize(obj_var, 1)


    print "After model created" 
    debugPrint(task,utilNodes,refTime,refTime_asDate,jr.jobs,nr.nodes,alternativeCons,cumlCons_cores,cumlCons_gpus,cumlCons_mics,cumlCons_mem)


    # Search phase 
    db = solver.Phase(task.values() + utilNodes.values() , solver.INTERVAL_DEFAULT)
    #db = solver.Phase(task.items() + utilNodes.items(), solver.CHOOSE_FIRST_UNBOUND,solver.ASSIGN_MIN_VALUE)
    search_log = solver.SearchLog(100000, objective)
    global_limit = solver.TimeLimit(FLAGS.time_limit)
    restart_monitor = solver.LubyRestart(5)
    solver.NewSearch(db, [objective, search_log, global_limit, restart_monitor])

    print maxHorizon

    while solver.NextSolution():
        print 'Objective:', objective
        print '---------  Scheduled Jobs ----------'
        GetSolution(task,utilNodes,refTime,refTime_asDate,jr.jobs,nr.nodes,alternativeCons,cumlCons_cores,cumlCons_gpus,cumlCons_mics,cumlCons_mem)
    solver.EndSearch()



if __name__ == '__main__':
    app.run()
