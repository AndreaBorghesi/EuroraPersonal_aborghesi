#!/usr/bin/python

'''
@Author: Andrea Borghesi
    Job Profiling script - taken a list of job which executed on Eurora in a time interval, compute average load of the whole machine
'''

import os
from subprocess import Popen, PIPE
import datetime
import numpy as np
import pickle
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

LOGNAME = "jobs.log"
DATESFILE="date.tmp"

# 31 Marzo
#DIR="/media/sda4/eurora/data_analysis/job_profiling/dataPBS_31Marzo/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/plots_pbs/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/stats_pbs/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/pbs_dump/"
# 1 Aprile 1400 1600
#DIR="/media/sda4/eurora/data_analysis/job_profiling/dataPBS_1Aprile_1400_1600/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1400_1600/plots_pbs/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1400_1600/stats_pbs/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1400_1600/pbs_dump/"
# 1 Aprile 1730 1730
#DIR="/media/sda4/eurora/data_analysis/job_profiling/dataPBS_1Aprile_1730_1930/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1730_1930/plots_pbs/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1730_1930/stats_pbs/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1730_1930/pbs_dump/"
# 2 Aprile 
#DIR="/media/sda4/eurora/data_analysis/job_profiling/dataPBS_2Aprile_1310_1510/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/plots_pbs/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/stats_pbs/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/pbs_dump/"
# 3 Aprile 
DIR="/media/sda4/eurora/data_analysis/job_profiling/dataPBS_3Aprile_1600_1800/"
PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/plots_pbs/"
STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/stats_pbs/"
DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/pbs_dump/"


STEP=15    # defines the granularity of the sampling for the job profile (in seconds)
QUEUE_CUTOFF=0   # defines after how many second a job is considered to actually be in queue 
NCORES=976

# average queue times in seconds
AVG_QTIME_DEBUG = 1800   
AVG_QTIME_LOGPAR = 86400
AVG_QTIME_PAR = 21600

tot_time_in_q = 0              # sum the time spent in queue by jobs
tot_time_in_q_weighted = 0
avg_time_in_q = 0              # average time spent in queue by jobs
avg_time_in_q_weighted = 0
tot_time_in_q_DEBUG = 0              
tot_time_in_q_weighted_DEBUG = 0
avg_time_in_q_DEBUG = 0        
avg_time_in_q_weighted_DEBUG = 0
tot_time_in_q_PAR = 0              
tot_time_in_q_weighted_PAR = 0
avg_time_in_q_PAR = 0        
avg_time_in_q_weighted_PAR = 0
tot_time_in_q_LONGPAR = 0              
tot_time_in_q_weighted_LONGPAR = 0
avg_time_in_q_LONGPAR = 0        
avg_time_in_q_weighted_LONGPAR = 0

tot_cores_used = 0             # sum of the number of cores used by all jobs
tot_gpu_used = 0
tot_mic_used = 0
tot_running_jobs = 0
tot_perc_cores_used = 0
avg_cores_used = 0             # sum of tot_cores_used divided by the number instant of times considered
avg_gpu_used = 0
avg_mic_used = 0
avg_perc_cores_used = 0
avg_running_jobs = 0



# read interval datetimes from file
datefile = DIR + DATESFILE
try:
    # read start_datetime and end_datetime from file
    with open(datefile,'r') as df:
        line = df.read().rstrip("\n")
except:
    print "Can't read " + datesfile + "\n"
dates = line.split(";")
start_datetime = datetime.datetime.strptime(dates[0],"%Y-%m-%d %H:%M:%S")
end_datetime = datetime.datetime.strptime(dates[1].rstrip(),"%Y-%m-%d %H:%M:%S")

jobs_log = DIR + LOGNAME

cmd = 'cat ' + jobs_log
status = Popen(cmd, shell=True, stdin=open(os.devnull), stdout=PIPE, stderr=PIPE)

PBS_job_exec_time_series = []
node_profile = []   # contains the number of node used on eurora (a value for each STEP seconds)
cpu_profile = []    # contains the number of cores used on eurora (a value for each STEP seconds)
dates = []          # contains the dates
nodes = []          # contains the number of used nodes
cpus = []           # contains the number of used cpus
queue_time = []
job_exec = []

for line in iter(status.stdout.readline,''):
    #print "Before split.."
    data =  line.split('__')

    #print "Before parsing.."

    # parsing the data from job.log
    job_info = data[0].split(';')
    job_id_string = job_info[0]
    job_name = job_info[1]
    user = job_info[2]
    queue = job_info[3]
    st = job_info[4]
    start_time = datetime.datetime.strptime(st,"%Y-%m-%d %H:%M:%S")
            
    job_resources = data[1].split('#')
    resources_temp = []
    for job_res in job_resources:
        job_res_split = job_res.split(';')
        resources_temp.append((job_res_split))
    resources = [x for x in resources_temp if x!=[""]]

    gpu_req = 0
    mic_req = 0
    for r in resources:
        gpu_req += int(r[2])
        mic_req += int(r[3])

    job_times_req = data[2].split(';')
    rt = job_times_req[0]
    run_start_time = datetime.datetime.strptime(rt,"%Y-%m-%d %H:%M:%S")
    et = job_times_req[1]
    if(et!="None"):  # it could happen that some jobs are still running when parsed and insertered into the DB
        end_time = datetime.datetime.strptime(et,"%Y-%m-%d %H:%M:%S")
    node_req = int(job_times_req[2])
    cpu_req = int(job_times_req[3])
    mem_req = int(job_times_req[4])
    time_req = job_times_req[5].rstrip()
    hhmm = time_req.split(':')
    time_req_as_delta = datetime.timedelta(hours=int(hhmm[0]),minutes=int(hhmm[1]))

    if(start_time + time_req_as_delta < end_time):
        end_time = start_time + time_req_as_delta

    #print "Putting value in list.."
    #PBS_job_exec_time_series.append((start_time,run_start_time,end_time,node_req,cpu_req,time_req_as_delta,queue))
    PBS_job_exec_time_series.append((job_id_string,start_time,run_start_time,end_time,node_req,cpu_req,mem_req,time_req_as_delta,gpu_req,mic_req,queue))

#print PBS_job_exec_time_series

# now in PBS_job_exec_time_series we have the running start time and the end time for each job, together with the related node and cpu requested
current_time = start_datetime
step = datetime.timedelta(seconds=STEP)
min_queue = datetime.timedelta(seconds=QUEUE_CUTOFF)
while current_time < end_datetime:  # for every STEP seconds in the time interval we chose our job from
    used_nodes = 0   # number of nodes currently used
    used_cpus = 0    # number of cores currently used
    in_queue = 0
    gpus_req_jobs_in_queue = 0
    gpus_req_all_jobs = 0
    mics_req_jobs_in_queue = 0
    mem_req_jobs_in_queue = 0
    mics_req_all_jobs = 0
    mem_req_all_jobs = 0
    exec_jobs = 0
    for (jid,st,rt,et,nnodes,cores,mem_req,treq,gpu_req,mic_req,q) in PBS_job_exec_time_series:
    #for (st,rt,et,n,c,treq,q) in PBS_job_exec_time_series:
        if (current_time >= rt and current_time <= et):    # PBS consider occupied resources only during REAL execution time
        #if (current_time >= rt and current_time <= (st + treq)):    # PBS consider occupied resources for the whole estimated execution time
#            used_nodes += n
#            used_cpus +=c
#            exec_jobs += 1
            used_nodes += nnodes
            used_cpus +=cores
            exec_jobs += 1
            gpus_req_all_jobs += gpu_req
            mics_req_all_jobs += mic_req
            mem_req_all_jobs += mem_req
            tot_cores_used+=cores
            tot_gpu_used+=gpu_req
            tot_mic_used+=mic_req
            tot_running_jobs+=1
        if (st <= current_time <= rt and (rt-st)>=min_queue):
            in_queue += 1
    dates.append(current_time)
#    print current_time
#    print used_nodes
#    print used_cpus
    nodes.append(used_nodes)
    cpus.append(used_cpus)
    queue_time.append(in_queue) 
    job_exec.append(exec_jobs)
    node_profile.append((current_time,used_nodes))
    cpu_profile.append((current_time,used_cpus))
    current_time += step



times_in_queue_weighted = []
times_in_queue = []
for (jid,st,rt,et,nnodes,cores,mem_req,treq,gpu_req,mic_req,q) in PBS_job_exec_time_series:
    # for a fair comparison with PBS
    #if(rt>end_datetime):
    #    rt=end_datetime
    tt = rt-st
    #print rt
    if(q=="debug"):
        avg_q=AVG_QTIME_DEBUG
    if(q=="parallel"):
        avg_q=AVG_QTIME_PAR
    if(q=="longpar" or q=="np_longpar"):
        avg_q=AVG_QTIME_LOGPAR
    if(q=="reservation"):
        avg_q=AVG_QTIME_DEBUG
    ttw = (rt-st)/avg_q
    #if(0<tt.total_seconds()<1000):
    if(0<tt.total_seconds()):
#        print tt.total_seconds()
        tot_time_in_q+=tt.total_seconds()
        tot_time_in_q_weighted+=ttw.total_seconds()
        times_in_queue.append(tt.total_seconds())
        times_in_queue_weighted.append(ttw.total_seconds())
        if(q=="debug"):
            tot_time_in_q_DEBUG+=tt.total_seconds()
            tot_time_in_q_weighted_DEBUG+=ttw.total_seconds()
        if(q=="parallel"):
            tot_time_in_q_PAR+=tt.total_seconds()
            tot_time_in_q_weighted_PAR+=ttw.total_seconds()
        if(q=="longpar" or q=="np_longpar"):
            tot_time_in_q_LONGPAR+=tt.total_seconds()
            tot_time_in_q_weighted_LONGPAR+=ttw.total_seconds()
avg_time_in_q=float(tot_time_in_q)/len(PBS_job_exec_time_series)
avg_time_in_q_weighted=float(tot_time_in_q_weighted)/len(PBS_job_exec_time_series)
avg_time_in_q_DEBUG=float(tot_time_in_q_DEBUG)/len(PBS_job_exec_time_series)
avg_time_in_q_weighted_DEBUG=float(tot_time_in_q_weighted_DEBUG)/len(PBS_job_exec_time_series)
avg_time_in_q_PAR=float(tot_time_in_q_PAR)/len(PBS_job_exec_time_series)
avg_time_in_q_weighted_PAR=float(tot_time_in_q_weighted_PAR)/len(PBS_job_exec_time_series)
avg_time_in_q_LONGPAR=float(tot_time_in_q_LONGPAR)/len(PBS_job_exec_time_series)
avg_time_in_q_weighted_LOGNPAR=float(tot_time_in_q_weighted_LONGPAR)/len(PBS_job_exec_time_series)


   
#print node_profile
#print cpu_profile

#for x in node_profile:
#    print x

#print dates

# convert dates to matplotlib format
graph_dates = mdates.date2num(dates)

dates_as_int = np.arange(len(graph_dates))       # nummber if instants of time used

avg_cores_used=float(tot_cores_used)/len(graph_dates)
avg_gpu_used=float(tot_gpu_used)/len(graph_dates)
avg_mic_used=float(tot_mic_used)/len(graph_dates)
avg_running_jobs=float(tot_running_jobs)/len(graph_dates)

# calc avg load of system
for core in cpus:
    perc_cores_used_per_istant = float(core)/NCORES
    tot_perc_cores_used+=perc_cores_used_per_istant
avg_perc_cores_used=tot_perc_cores_used/len(cpus)

print PBS_job_exec_time_series
print cpus
            

dumpName = DUMPDIR + "PBS_dump.p"
pickle.dump(PBS_job_exec_time_series,open(dumpName,"wb"))

