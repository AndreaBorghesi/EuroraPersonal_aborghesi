#!/usr/bin/python

'''
@Author: Andrea Borghesi
    Job Profiling script - taken a list of job which executed on Eurora in a time interval, compute average load of the whole machine
'''

import os
from subprocess import Popen, PIPE
import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

LOGNAME = "jobs.log"
DATESFILE="date.tmp"
DIR="/media/sda4/eurora/data_analysis/job_profiling/dataT/"
STEP=15    # defines the granularity of the sampling for the job profile (in seconds)
QUEUE_CUTOFF=5   # defines after how many second a job is considered to actually be in queue 
# average queue times in seconds
AVG_QTIME_DEBUG = 1800   
AVG_QTIME_LOGPAR = 86400
AVG_QTIME_PAR = 21600

# read interval datetimes from file
datefile = DIR + DATESFILE
try:
    # read start_datetime and end_datetime from file
    with open(datefile,'r') as df:
        line = df.read().rstrip("\n")
except:
    print "Can't read " + datefile + "\n"
dates = line.split(";")
start_datetime = datetime.datetime.strptime(dates[0],"%Y-%m-%d %H:%M:%S")
end_datetime = datetime.datetime.strptime(dates[1].rstrip(),"%Y-%m-%d %H:%M:%S")

jobs_log = DIR + LOGNAME

cmd = 'cat ' + jobs_log
status = Popen(cmd, shell=True, stdin=open(os.devnull), stdout=PIPE, stderr=PIPE)

job_exec_time_series = []
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

    #print "Putting value in list.."
    job_exec_time_series.append((start_time,run_start_time,end_time,node_req,cpu_req,time_req_as_delta,queue))

#print job_exec_time_series

# now in job_exec_time_series we have the running start time and the end time for each job, together with the related node and cpu requested
current_time = start_datetime
step = datetime.timedelta(seconds=STEP)
min_queue = datetime.timedelta(seconds=QUEUE_CUTOFF)
while current_time < end_datetime:  # for every STEP seconds in the time interval we chose our job from
    used_nodes = 0   # number of nodes currently used
    used_cpus = 0    # number of cores currently used
    in_queue = 0
    exec_jobs = 0
    for (st,rt,et,n,c,treq,q) in job_exec_time_series:
        if (current_time >= rt and current_time <= et):    # PBS consider occupied resources only during REAL execution time
        #if (current_time >= rt and current_time <= (st + treq)):    # PBS consider occupied resources for the whole estimated execution time
            used_nodes += n
            used_cpus +=c
            exec_jobs += 1
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

times_in_queue = []
times_in_queue_weighted = []
for (st,rt,et,n,c,treq,q) in job_exec_time_series:
    tt = rt-st
    if(q=="debug"):
        avg_q=AVG_QTIME_DEBUG
    if(q=="parallel"):
        avg_q=AVG_QTIME_PAR
    if(q=="longpar" or q=="np_longpar"):
        avg_q=AVG_QTIME_LOGPAR
    if(q=="reservation"):
        avg_q=AVG_QTIME_DEBUG
    ttw = (rt-st)/avg_q
    if(0<tt.total_seconds()<1000):
#        print tt.total_seconds()
        times_in_queue.append(tt.total_seconds())
        times_in_queue_weighted.append(ttw.total_seconds())


#print node_profile
#print cpu_profile

#for x in node_profile:
#    print x

#print dates

# convert dates to matplotlib format
graph_dates = mdates.date2num(dates)
print graph_dates

#print graph_dates

# plotting
plt.figure()
plt.subplot(211)

#plt.title('Number of nodes active')
#plt.plot_date(graph_dates,nodes,color='yellow')

plt.title('Number of cores active')
plt.plot_date(graph_dates,cpus,color='blue')

#plt.title('Number of jobs waiting (more than %d sec)' % (QUEUE_CUTOFF))
#plt.plot_date(graph_dates,queue_time,color='red')

plt.subplot(212)
plt.title('Number of running jobs') 
plt.plot_date(graph_dates,job_exec,color='green')

#plt.hist(times_in_queue,bins=1000)
#plt.hist(times_in_queue_weighted,bins=1000)
plt.show()
