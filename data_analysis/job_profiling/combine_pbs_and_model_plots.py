#!/usr/bin/python

'''
@Author: Andrea Borghesi
    Job Profiling script - taken a list of job which executed on Eurora in a time interval, compute average load of the whole machine
'''

import os
from subprocess import Popen, PIPE
import pickle
import datetime
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

LOGNAME = "jobs.log"
DATESFILE="date.tmp"
# 31 Marzo
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/plots_print/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/stats/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/pbs_dump/"
## 1 Aprile 14:00-16:00
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1400_1600/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1400_1600/pbs_dump/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1400_1600/plots_print/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1400_1600/stats/"
# 1 Aprile 17:30-19:30
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1730_1930/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1730_1930/plots_print/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1730_1930/stats/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/1_Aprile_1730_1930/pbs_dump/"
# 2 Aprile 13:10-15:10
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/plots_print/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/stats/"
#DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/pbs_dump/"
# 3 Aprile 16:00-18:00
DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/"
PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/plots_print/"
STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/stats/"
DUMPDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/pbs_dump/"


STEP=15    # defines the granularity of the sampling for the job profile (in seconds)
QUEUE_CUTOFF=0   # defines after how many second a job is considered to actually be in queue 
NCORES=1024
# average queue times in seconds
AVG_QTIME_DEBUG = 1800   
AVG_QTIME_LOGPAR = 86400
AVG_QTIME_PAR = 21600

# import values from PBS
pbs_dump_file=DUMPDIR + "PBS_dump.p"
PBS_job_exec_time_series = pickle.load(open(pbs_dump_file,"rb"))

for subdir, dirs, files in os.walk(DIR):
    for jobs_log in files:
        if (jobs_log.endswith(".log") and jobs_log.startswith("Makespan_WT_NL")):

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

            #print start_datetime
            #print end_datetime

            #jobs_log = DIR + LOGNAME

            cmd = 'cat ' + subdir + "/" + jobs_log
            #print cmd
            status = Popen(cmd, shell=True, stdin=open(os.devnull), stdout=PIPE, stderr=PIPE)
            
            #  ------------ OUR MODEL ---------
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
            #  -----------  PBS -----------
            PBS_node_profile = []   # contains the number of node used on eurora (a value for each STEP seconds)
            PBS_cpu_profile = []    # contains the number of cores used on eurora (a value for each STEP seconds)
            PBS_dates = []          # contains the dates
            PBS_nodes = []          # contains the number of used nodes
            PBS_cpus = []           # contains the number of used cpus
            PBS_queue_time = []
            PBS_job_exec = []
            PBS_gpus_queue = []     # contains the number of gpus requested by nodes in queue
            PBS_mics_queue = []     # contains the number of mics requested by nodes in queue

            for line in iter(status.stdout.readline,''):
                #print line
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
                #print hhmmss
            #    if(len(hhmmss)==1):
            #        time_req_as_delta = datetime.timedelta(seconds=int(hhmmss[0]))
            #    if(len(hhmmss)==2):
            #        time_req_as_delta = datetime.timedelta(minutes=int(hhmmss[0]),seconds=int(hhmmss[1]))
            #    if(len(hhmmss)==3):
            #        time_req_as_delta = datetime.timedelta(hours=int(hhmmss[0],minutes=int(hhmmss[1]),seconds=int(hhmmss[2])))
                #print time_req_as_delta

                if(start_time + time_req_as_delta < end_time):
                    end_time = start_time + time_req_as_delta
                    #print job_id_string
                #print "END TIME " + str(end_time)

                #print "Putting value in list.."
                job_exec_time_series.append((job_id_string,start_time,run_start_time,end_time,node_req,cpu_req,mem_req,time_req_as_delta,gpu_req,mic_req,queue))

            #print job_exec_time_series


            # ------------ Our Model -------------------

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



            # ------------ PBS -------------------
            current_time = start_datetime
            step = datetime.timedelta(seconds=STEP)
            min_queue = datetime.timedelta(seconds=QUEUE_CUTOFF)
            while current_time < end_datetime:  # for every STEP seconds in the time interval we chose our job from
                PBS_used_nodes = 0   # number of nodes currently used
                PBS_used_cpus = 0    # number of cores currently used
                PBS_in_queue = 0
                PBS_gpus_req_jobs_in_queue = 0
                PBS_gpus_req_all_jobs = 0
                PBS_mics_req_jobs_in_queue = 0
                PBS_mem_req_jobs_in_queue = 0
                PBS_mics_req_all_jobs = 0
                PBS_mem_req_all_jobs = 0
                PBS_exec_jobs = 0
                for (jid,st,rt,et,nnodes,cores,mem_req,treq,gpu_req,mic_req,q) in PBS_job_exec_time_series:
                    if (current_time >= rt and current_time <= et):    # PBS consider occupied resources only during REAL execution time
                    #if (current_time >= rt and current_time <= (st + treq)):    # PBS consider occupied resources for the whole estimated execution time
                        PBS_used_nodes += nnodes
                        PBS_used_cpus +=cores
                        PBS_exec_jobs += 1
                        PBS_gpus_req_all_jobs += gpu_req
                        PBS_mics_req_all_jobs += mic_req
                        PBS_mem_req_all_jobs += mem_req
                    if (st <= current_time <= rt and (rt-st)>=min_queue):
                        PBS_in_queue += 1
                        PBS_gpus_req_jobs_in_queue += gpu_req
                        PBS_mics_req_jobs_in_queue += mic_req
                        PBS_mem_req_jobs_in_queue += mem_req

                PBS_nodes.append(PBS_used_nodes)
                PBS_cpus.append(PBS_used_cpus)
                PBS_queue_time.append(PBS_in_queue) 
                PBS_job_exec.append(PBS_exec_jobs)
                PBS_node_profile.append((current_time,PBS_used_nodes))
                PBS_cpu_profile.append((current_time,PBS_used_cpus))
                PBS_gpus_queue.append(PBS_gpus_req_jobs_in_queue)
                PBS_mics_queue.append(PBS_mics_req_jobs_in_queue)
                current_time += step

            times_in_queue_weighted = []
            times_in_queue = []
            for (jid,st,rt,et,nnodes,cores,mem_req,treq,gpu_req,mic_req,q) in job_exec_time_series:
                # for a fair comparison with pbs
#                if(rt>end_datetime):
#                    rt=end_datetime
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
                #if(0<tt.total_seconds()<1000):
                if(0<tt.total_seconds()):
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

            dates_as_int = np.arange(len(graph_dates))       # nummber if instants of time used


            # plotting

#            plt.figure()
#            plt.subplot(211)
#            plt.title('Number of cores active')
#            plt.plot_date(graph_dates,cpus,color='blue')
#            plt.subplot(212)
#            plt.title('Number of running jobs') 
#            plt.plot_date(graph_dates,job_exec,color='green')
#            plotfile = PLOTDIR + jobs_log[:-4] + "_jobs_nCores.pdf"
#            plt.savefig(plotfile,dpi=100)
#
#            plt.figure()
#            plt.subplot(211)
#            plt.title('Number of jobs waiting (more than %d secs)' % (QUEUE_CUTOFF))
#            plt.plot_date(graph_dates,queue_time,color='red')
#            plt.subplot(212)
#            plt.title('Number of running jobs') 
#            plt.plot_date(graph_dates,job_exec,color='green')
#            plotfile = PLOTDIR + jobs_log[:-4] + "_jobs_inQueue.pdf"
#            plt.savefig(plotfile,dpi=100)


            fig=plt.figure()
            plt.ylabel('Number of cores active')
            plt.xlabel('Time')
            plt.plot(dates_as_int,cpus,linewidth=2,label="Our Model")
            plt.plot(dates_as_int,PBS_cpus,linewidth=3,ls="--",label="PBS")
            lgd=plt.legend(loc='center left',bbox_to_anchor=(1.0,0.5))
            plotfile = PLOTDIR + "jobs_nCores.pdf"
            fig.savefig(plotfile, bbox_extra_artists=(lgd,), bbox_inches='tight')

            fig=plt.figure()
            plt.ylabel('Number of jobs waiting (more than %d secs)' % (QUEUE_CUTOFF))
            plt.xlabel('Time')
            plt.plot(dates_as_int,queue_time,linewidth=2,label="Our Model")
            plt.plot(dates_as_int,PBS_queue_time,linewidth=3,ls="--",label="PBS")
            lgd=plt.legend(loc='center left',bbox_to_anchor=(1.0,0.5))
            plotfile = PLOTDIR + "jobs_inQ.pdf"
            fig.savefig(plotfile, bbox_extra_artists=(lgd,), bbox_inches='tight')

            fig=plt.figure()
            plt.ylabel('Number of running jobs')
            plt.xlabel('Time')
            plt.plot(dates_as_int,job_exec,linewidth=2,label="Our Model")
            plt.plot(dates_as_int,PBS_job_exec,linewidth=3,ls="--",label="PBS")
            lgd=plt.legend(loc='center left',bbox_to_anchor=(1.0,0.5))
            plotfile = PLOTDIR + "jobs_running.pdf"
            fig.savefig(plotfile, bbox_extra_artists=(lgd,), bbox_inches='tight')




#            plt.plot_date(graph_dates,nodes)
#            plt.plot_date(graph_dates,PBS_nodes,ls="--")

            #plt.title('Number of cores active')
            #plt.plot_date(graph_dates,cpus,color='blue')

            #plt.title('Number of jobs waiting (more than %d secs)' % (QUEUE_CUTOFF))
            #plt.plot_date(graph_dates,queue_time,color='red')

            #plt.title('Number of gpus requested by jobs in queue (in queue for more than %d secs)' % (QUEUE_CUTOFF))
            #plt.plot_date(graph_dates,gpus_queue,color='orange')

            #plt.title('Number of mics requested by jobs in queue (in queue for more than %d secs)' % (QUEUE_CUTOFF))
            #plt.plot_date(graph_dates,mics_queue,color='pink')


            #plt.show()



