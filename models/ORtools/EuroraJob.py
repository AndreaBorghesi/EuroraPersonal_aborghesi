''' Andrea Borghesi, University of Bologna '''
''' Class to represent a job in EURORA system '''

NNODE = 64

import datetime

class Job():
    def __init__ (self,jobId,jobName,userName,queue,startTime,jobResources,runStartTime,endTime,nodeReq,cpuReq,memReq,timeReq,deleted,dependency,exitStatus):
        self.jobId = jobId                          # job identifier (string) 
        self.jobName = jobName                      # job name (string)
        self.userName = userName                    # user name -- who launches the job (string)
        self.queue = queue                          # the queue in which a job belongs (string)
        self.startTime = startTime                  # when a job enters in the queue (string)

        self.jobResources = jobResources            # resources used by a job --> job sub-sets
        job_resources = self.jobResources.split('#')
        resources_temp = []
        for job_res in job_resources:
            job_res_split = job_res.split(';')
            resources_temp.append((job_res_split))
        self.resources = [x for x in resources_temp if x!=[""]]
        gpu_used = 0
        mic_used = 0
        nodes_used = 0
        cores_used = 0
        mem_used = 0
        for r in self.resources:
            nodes_used += 1 
            cores_used += int(r[1])
            gpu_used += int(r[2])
            mic_used += int(r[3])
            mem_used += int(r[4])
        self.nNodes = nodes_used                    # number of nodes used --> numbers of sub-jobs (string)
        self.nCores = cores_used                    # number of cores actually used on a specific node (string)
        self.nGpus = gpu_used                       # number of gpus actually used on a specific node (string)
        self.nMics = mic_used                       # number of mics actually used on a specific node (string)
        self.memUsed = mem_used                     # amount of memory s actually used on a specific node (string)

        self.runStartTime = runStartTime            # when a job begins its execution (string)
        self.endTime = endTime                      # when a job terminates (string)

        self.nodeReq = int(nodeReq)                 # number of nodes REQUESTED by the user (int)
        self.cpuReq = int(cpuReq)                   # number of cores REQUESTED by the user (int)
        self.memReq = int(memReq)*1024*1024         # amount of memory REQUESTED by the user (int)
        self.timeReq = timeReq                      # completion time estimated by the user (string)
        self.deleted = deleted                      # a job could be deleted before its completion (string)
        self.dependency = dependency                # job possible dependecies (string)
        self.exitStatus = exitStatus                # exit status (string)

        self.startTime_asDate = datetime.datetime.strptime(startTime,"%Y-%m-%d %H:%M:%S")
        self.endTime_asDate = datetime.datetime.strptime(endTime,"%Y-%m-%d %H:%M:%S")
        self.runStartTime_asDate = datetime.datetime.strptime(runStartTime,"%Y-%m-%d %H:%M:%S")
        self.realDuration = self.endTime_asDate - self.runStartTime_asDate
        hhmm = self.timeReq.split(':')
        self.timeReq_asTime = datetime.timedelta(hours=int(hhmm[0]),minutes=int(hhmm[1]))

        self.isScheduled = 0
        self.usedNodes = []   # a vector with NNodes element, 1 if a job uses that node, 0 if it does not
        for i in range(NNODE):
            self.usedNodes.append(0)

    
    def getDuration_asTime (self, atTime):
        if(self.startTime_asDate + min(self.realDuration,self.timeReq_asTime) <= atTime):
            return min(self.realDuration,self.timeReq_asTime)
        return self.timeReq_asTime


    def getDuration_asInt (self, atTime):
        return int((self.getDuration_asTime(atTime)).total_seconds())

    
    def getStart_asInt(self, atTime):
        return int((self.startTime_asDate - atTime).total_seconds())

        
    def getEnd_asInt(self, atTime):
        return int((self.endTime_asDate - atTime).total_seconds())


    def getRunStart_asInt(self, atTime):
        return int((self.runStartTime_asDate - atTime).total_seconds())


    # Actual node allocation for scheduled jobs 
    def allocate(self,nodes):
        self.usedNodes = nodes


    def humanPrintJob (self):
        outstr = "JobId: " + self.jobId + " -- JobName: " + self.jobName + " - User: " + self.userName + " - StartTime: " + self.startTime + " - RunStartTime: " + self.runStartTime + " - EndTime: " + self.endTime + " - ResourcesUsed: " + str(self.resources) + " - NNodesUsed: " + str(self.nNodes) + " - NCpusUsed: " + str(self.nCores) + " MemUsed: " + str(self.memUsed) + " - NodeReq: " + str(self.nodeReq) + " - CpuReq: " + str(self.cpuReq) + " MemReq: " + str(self.memReq) + " - TimeReq: " + self.timeReq + " - RealDuration: " + str(self.realDuration) + " - Deleted: " + self.deleted + " - Dependency: " + self.dependency + " - ExitStatus: " + self.exitStatus
        return outstr

    def logPrintJob (self):
        outstr = self.jobId + ";" + self.jobName + ";" + self.userName + ";" + self.startTime + "__" + self.jobResources + "__" + self.runStartTime + ";" + self.endTime + ";" + str(self.nodeReq) + ";" + str(self.cpuReq) + ";" + self.timeReq + ";" + self.deleted + ";" + self.dependency + ";" + self.exitStatus
        return outstr

''' Class to read from files info about nodes '''
class JobReader():
    def __init__ (self):
        self.jobs = {}

    def readJobs(self, nodesFile):
        # Read nodes properties 
        with open(nodesFile,'r') as f:
            i = 0
            for line in f:
                data =  line.split('__')
                job_info = data[0].split(';')
                job_id_string = job_info[0]
                job_name = job_info[1]
                user = job_info[2]
                queue = job_info[3]
                st = job_info[4]
                # resources split in __init__
                job_times_req = data[2].split(';')
                rt = job_times_req[0]
                et = job_times_req[1]
                node_req = job_times_req[2]
                cpu_req = job_times_req[3]
                mem_req = job_times_req[4]
                time_req = job_times_req[5]
                deleted = job_times_req[6]
                dependency = job_times_req[7]
                exit_status = job_times_req[8].rstrip()

                j = Job(job_id_string,job_name,user,queue,st,data[1],rt,et,node_req,cpu_req,mem_req,time_req,deleted,dependency,exit_status)
                self.jobs[i] = j
                i+=1


    def printTest(self):
        for j,job in self.jobs.items():
            print j 
            print job.humanPrintJob()

