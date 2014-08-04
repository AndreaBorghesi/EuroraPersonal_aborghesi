#!/usr/bin/python

'''
@Author: Andrea Borghesi
    Job Profiling script - compare multiple models
'''

import os
from subprocess import Popen, PIPE
import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np

LOGNAME = "jobs.log"
DATESFILE="date.tmp"
DIR="/media/sda4/eurora/data_analysis/job_profiling/modelComparison/log/"
PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/modelComparison/plots/"
STEP=15    # defines the granularity of the sampling for the job profile (in seconds)
QUEUE_CUTOFF=0   # defines after how many second a job is considered to actually be in queue 
# average queue times in seconds
AVG_QTIME_DEBUG = 1800   
AVG_QTIME_LOGPAR = 86400
AVG_QTIME_PAR = 21600

models_ncores = []
models_inQueue = []
models_runJobs = []
models_dates = []

for subdir, dirs, files in os.walk(DIR):
    for jobs_log in files:
        if (jobs_log.endswith(".log") and not jobs_log.startswith("Stats_") and not jobs_log.startswith("._")):
 
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

            cmd = 'cat ' + subdir + "/" + jobs_log
            #print cmd
            status = Popen(cmd, shell=True, stdin=open(os.devnull), stdout=PIPE, stderr=PIPE)

            job_exec_time_series = []
            node_profile = []   # contains the number of node used on eurora (a value for each STEP seconds)
            cpu_profile = []    # contains the number of cores used on eurora (a value for each STEP seconds)
            dates = []          # contains the dates
            nodes = []          # contains the number of used nodes
            cpus = []           # contains the number of used cpus
            queue_time = []
            job_exec = []
            gpus_queue = []     # contains the number of gpus requested by nodes in queue
            mics_queue = []     # contains the number of mics requested by nodes in queue

            for line in iter(status.stdout.readline,''):
                data =  line.split('__')

                # parsing the data from job.log
                job_info = data[0].split(';')
                job_id_string = job_info[0]
                job_name = job_info[1]
                user = job_info[2]
                queue = job_info[3]
                st = job_info[4]
                start_time = datetime.datetime.strptime(st,"%Y-%m-%d %H:%M:%S")
                #print "START TIME " + str(start_time)
                        
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
                #print mem_req
                time_req = job_times_req[5].rstrip()
                #print time_req
                hhmm = time_req.split(':')
                time_req_as_delta = datetime.timedelta(hours=int(hhmm[0]),minutes=int(hhmm[1]))

                if(run_start_time + time_req_as_delta < end_time):
                    end_time = run_start_time + time_req_as_delta
                    #print job_id_string
                #print "END TIME " + str(end_time)

                #print "Putting value in list.."
                job_exec_time_series.append((job_id_string,start_time,run_start_time,end_time,node_req,cpu_req,mem_req,time_req_as_delta,gpu_req,mic_req,queue))

            # now in job_exec_time_series we have the running start time and the end time for each job, together with the related node and cpu requested
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
                for (jid,st,rt,et,nnodes,cores,mem_req,treq,gpu_req,mic_req,q) in job_exec_time_series:
                    if (current_time >= rt and current_time <= et):    # PBS consider occupied resources only during REAL execution time
                    #if (current_time >= rt and current_time <= (st + treq)):    # PBS consider occupied resources for the whole estimated execution time
                        used_nodes += nnodes
                        used_cpus +=cores
                        exec_jobs += 1
                        gpus_req_all_jobs += gpu_req
                        mics_req_all_jobs += mic_req
                        mem_req_all_jobs += mem_req
                        #print jid
                    if (st <= current_time <= rt and (rt-st)>=min_queue):
                        in_queue += 1
                        gpus_req_jobs_in_queue += gpu_req
                        mics_req_jobs_in_queue += mic_req
                        mem_req_jobs_in_queue += mem_req

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
                gpus_queue.append(gpus_req_jobs_in_queue)
                mics_queue.append(mics_req_jobs_in_queue)
                current_time += step

            times_in_queue_weighted = []
            times_in_queue = []
            for (jid,st,rt,et,nnodes,cores,mem_req,treq,gpu_req,mic_req,q) in job_exec_time_series:
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
            #print graph_dates

            name=jobs_log[:-4]

            models_ncores.append((name,cpus))
            models_inQueue.append((name,queue_time))
            models_runJobs.append((name,job_exec))
            models_dates = graph_dates

# plotting: plot one line in the graph for each model
fig=plt.figure()
plt.ylabel('Number of cores active')
plt.xlabel('Time')
i=1
for (n,m) in models_ncores:
    #plt.figure()
    dates_as_int = np.arange(len(m))
    #plt.plot_date(models_dates,m,xdate=True,ls='-',dash_joinstyle='round',drawstyle='steps-mid')
    plt.plot(dates_as_int,m,linewidth=2,label=n)
    lgd=plt.legend(loc='center left',bbox_to_anchor=(1.0,0.5))
    i+=1
#plt.show()
plotfile = PLOTDIR + "ALL_MODELS_jobs_nCores.pdf"
fig.savefig(plotfile, bbox_extra_artists=(lgd,), bbox_inches='tight')

fig=plt.figure()
plt.ylabel('Number of jobs waiting (more than %d secs)' % (QUEUE_CUTOFF))
plt.xlabel('Time')
i=1
for (n,m) in models_inQueue:
    #plt.figure()
    dates_as_int = np.arange(len(m))
    #plt.plot_date(models_dates,m,xdate=True,ls='-',dash_joinstyle='round',drawstyle='steps-mid')
    plt.plot(dates_as_int,m,linewidth=2,label=n)
    lgd=plt.legend(loc='center left',bbox_to_anchor=(1.0,0.5))
    i+=1
#plt.show()
plotfile = PLOTDIR + "ALL_MODELS_jobs_inQueue.pdf"
fig.savefig(plotfile, bbox_extra_artists=(lgd,), bbox_inches='tight')

fig=plt.figure()
plt.ylabel('Number of running jobs')
plt.xlabel('Time')
i=1
for (n,m) in models_runJobs:
    #plt.figure()
    dates_as_int = np.arange(len(m))
    #plt.plot_date(models_dates,m,xdate=True,ls='-',dash_joinstyle='round',drawstyle='steps-mid')
    plt.plot(dates_as_int,m,linewidth=3,label=n)
    lgd=plt.legend(loc='center left',bbox_to_anchor=(1.0,0.5))
    i+=1
#plt.show()
plotfile = PLOTDIR + "ALL_MODELS_jobs_running.pdf"
fig.savefig(plotfile, bbox_extra_artists=(lgd,), bbox_inches='tight')

