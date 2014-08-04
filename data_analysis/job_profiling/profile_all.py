#!/usr/bin/python

'''
@Author: Andrea Borghesi
    Job Profiling script - taken a list of job which executed on Eurora in a time interval, compute average load of the whole machine
'''

import os
from subprocess import Popen, PIPE
import datetime
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

LOGNAME = "jobs.log"
DATESFILE="date.tmp"

# ORIGINAL THOMAS MODELS
# 31 Marzo
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/plots2/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/31_Marzo_corretto/stats/"
# 2 Aprile 13:10-15:10
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/2_Aprile_1310_1510/stats/"
# 3 Aprile 13:10-15:10
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/3_Aprile_1600_1800/stats/"

# ER MODELS
#AVERAGE ENERGY (CPU AND MEM APPS)
# 31 Marzo
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_31_Marzo_corretto/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_31_Marzo_corretto/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_31_Marzo_corretto/stats/"
# 2 Aprile 13:10-15:10
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_2_Aprile_1310_1510/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_2_Aprile_1310_1510/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_2_Aprile_1310_1510/stats/"
# 3 Aprile 13:10-15:10
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_3_Aprile_1600_1800/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_3_Aprile_1600_1800/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_3_Aprile_1600_1800/stats/"
#WORST CASE
# 31 Marzo
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_31_Marzo_corretto/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_31_Marzo_corretto/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_31_Marzo_corretto/stats/"
# 2 Aprile 13:10-15:10
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_2_Aprile_1310_1510/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_2_Aprile_1310_1510/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_2_Aprile_1310_1510/stats/"
# 3 Aprile 13:10-15:10
#DIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_3_Aprile_1600_1800/"
#PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_3_Aprile_1600_1800/plots/"
#STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/thomasResults/ER_WC_3_Aprile_1600_1800/stats/"
DIR="/media/sda4/eurora/data_analysis/job_profiling/temp/"
PLOTDIR="/media/sda4/eurora/data_analysis/job_profiling/temp/plots/"
STATSDIR="/media/sda4/eurora/data_analysis/job_profiling/temp/stats/"


#ENergy Specs INfo
ENERGYDIR="/media/sda4/eurora/models/thomass_with_energyRankedNodes/"
CPUBOUND_ENERGIES="Energy_CPU_Bound.log"
MEMBOUND_ENERGIES="Energy_Mem_Bound.log"

STEP=15    # defines the granularity of the sampling for the job profile (in seconds)
QUEUE_CUTOFF=0   # defines after how many second a job is considered to actually be in queue 
NCORES=1024
# average queue times in seconds
AVG_QTIME_DEBUG = 1800   
AVG_QTIME_LOGPAR = 86400
AVG_QTIME_PAR = 21600


#parse energies info
cpu_en_file = ENERGYDIR + CPUBOUND_ENERGIES
mem_en_file = ENERGYDIR + MEMBOUND_ENERGIES
cpuBound_energies = {}
memBound_energies = {}
# since on EURORA we have some nodes not working or we miss some data (for example on nodes used only for debug), 
# but in our model we want to energetically rank all nodes we make some assumpionts for the missing values
for i in  range(1,32):
    cpuBound_energies[str(i)]=52500.0
    memBound_energies[str(i)]=61500.0
for i in  range(33,64):
    cpuBound_energies[str(i)]=62500.0
    memBound_energies[str(i)]=103000.0
f = open(cpu_en_file,"r")
lines = f.readlines()
f.close()
for line in lines:
    data = line.split(";")
    cpuBound_energies[str(data[0])]=float(data[1].rstrip())
f = open(mem_en_file,"r")
lines = f.readlines()
f.close()
for line in lines:
    data = line.split(";")
    memBound_energies[str(data[0])]=float(data[1].rstrip())

for subdir, dirs, files in os.walk(DIR):
    for jobs_log in files:
        if (jobs_log.endswith(".log") and not jobs_log.startswith("Stats_") and not jobs_log.startswith("._")):

            print jobs_log

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

            tot_cores_used = 0             # sum of the number of cores used by all jobs - per each time sample
            tot_gpu_used = 0
            tot_mic_used = 0
            tot_perc_cores_used = 0
            tot_running_jobs = 0
            avg_cores_used = 0             # sum of tot_cores_used divided by the number instant of times considered
            avg_gpu_used = 0
            avg_mic_used = 0
            avg_perc_cores_used = 0
            avg_running_jobs = 0

            N_jobs = 0
            N_jobs_DEBUG = 0
            N_jobs_PAR = 0
            N_jobs_LONGPAR = 0

            energy_consumed_WC = 0           
            energy_consumed_AVG = 0



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

                    # energies
                    nodeId=r[0]
                    nCores=int(r[1])
                    if(int(nodeId)>0):   
                        energy_consumed_AVG += (memBound_energies[nodeId]+cpuBound_energies[nodeId])/2*nCores/16
                        energy_consumed_WC += max(memBound_energies[nodeId],cpuBound_energies[nodeId])*nCores/16


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

                if(run_start_time + time_req_as_delta < end_time):
                    end_time = run_start_time + time_req_as_delta
                    #print job_id_string
                #print "END TIME " + str(end_time)

                #print "Putting value in list.."
                job_exec_time_series.append((job_id_string,start_time,run_start_time,end_time,node_req,cpu_req,mem_req,time_req_as_delta,gpu_req,mic_req,queue))

            #print job_exec_time_series

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
                        tot_cores_used+=cores
                        tot_gpu_used+=gpu_req
                        tot_mic_used+=mic_req
                        tot_running_jobs+=1
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
            avg_time_in_q=float(tot_time_in_q)/len(job_exec_time_series)
            avg_time_in_q_weighted=float(tot_time_in_q_weighted)/len(job_exec_time_series)
            avg_time_in_q_DEBUG=float(tot_time_in_q_DEBUG)/len(job_exec_time_series)
            avg_time_in_q_weighted_DEBUG=float(tot_time_in_q_weighted_DEBUG)/len(job_exec_time_series)
            avg_time_in_q_PAR=float(tot_time_in_q_PAR)/len(job_exec_time_series)
            avg_time_in_q_weighted_PAR=float(tot_time_in_q_weighted_PAR)/len(job_exec_time_series)
            avg_time_in_q_LONGPAR=float(tot_time_in_q_LONGPAR)/len(job_exec_time_series)
            avg_time_in_q_weighted_LOGNPAR=float(tot_time_in_q_weighted_LONGPAR)/len(job_exec_time_series) 

            for (jid,st,rt,et,nnodes,cores,mem_req,treq,gpu_req,mic_req,q) in job_exec_time_series:
                N_jobs+=1
                if(q=="debug"):
                    N_jobs_DEBUG+=1
                if(q=="parallel"):
                    N_jobs_PAR+=1
                if(q=="longpar" or q=="np_longpar"):
                    N_jobs_LONGPAR+=1
 


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


            # plotting

            plt.figure()
            plt.subplot(211)
            plt.title('Number of cores active')
            plt.plot_date(graph_dates,cpus,color='blue')
            plt.subplot(212)
            plt.title('Number of running jobs') 
            plt.plot_date(graph_dates,job_exec,color='green')
            plotfile = PLOTDIR + jobs_log[:-4] + "_jobs_nCores.pdf"
            plt.savefig(plotfile,dpi=100)

            plt.figure()
            plt.subplot(211)
            plt.title('Number of jobs waiting (more than %d secs)' % (QUEUE_CUTOFF))
            plt.plot_date(graph_dates,queue_time,color='red')
            plt.subplot(212)
            plt.title('Number of running jobs') 
            plt.plot_date(graph_dates,job_exec,color='green')
            plotfile = PLOTDIR + jobs_log[:-4] + "_jobs_inQueue.pdf"
            plt.savefig(plotfile,dpi=100)

            #plt.title('Number of nodes active')
            #plt.plot_date(graph_dates,nodes,color='yellow')

            #plt.title('Number of cores active')
            #plt.plot_date(graph_dates,cpus,color='blue')

            #plt.title('Number of jobs waiting (more than %d secs)' % (QUEUE_CUTOFF))
            #plt.plot_date(graph_dates,queue_time,color='red')

            #plt.title('Number of gpus requested by jobs in queue (in queue for more than %d secs)' % (QUEUE_CUTOFF))
            #plt.plot_date(graph_dates,gpus_queue,color='orange')

            #plt.title('Number of mics requested by jobs in queue (in queue for more than %d secs)' % (QUEUE_CUTOFF))
            #plt.plot_date(graph_dates,mics_queue,color='pink')

            plt.figure()
            plt.ylabel('Number of jobs')
            plt.xlabel('Queue time')
            plt.hist(times_in_queue,bins=100)
            plotfile = PLOTDIR + jobs_log[:-4] + "_hist_queues.pdf"
            plt.savefig(plotfile,dpi=100) 
            plt.figure()
            plt.ylabel('Number of jobs')
            plt.xlabel('Queue time')
            plt.hist(times_in_queue_weighted,bins=100)
            plotfile = PLOTDIR + jobs_log[:-4] + "_hist_queues_weighted.pdf"
            plt.savefig(plotfile,dpi=100)

#            print times_in_queue_weighted
#            print len(times_in_queue_weighted)
#            print len(job_exec_time_series)

            #plt.show()



            # stats
            statsfile = STATSDIR + "Stats_" + jobs_log[:-4] + ".log"

            wrtxt = "tot_time_in_q: " +str(tot_time_in_q)
            wrtxt += "\ntot_time_in_q_weighted: " +str(tot_time_in_q_weighted)
            wrtxt += "\navg_time_in_q: " +str(avg_time_in_q)
            wrtxt += "\navg_time_in_q_weighted: " +str(avg_time_in_q_weighted)
            wrtxt += "\n\ntot_time_in_q_DEBUG: " +str(tot_time_in_q_DEBUG)
            wrtxt += "\ntot_time_in_q_weighted_DEBUG: " +str(tot_time_in_q_weighted_DEBUG)
            wrtxt += "\navg_time_in_q_DEBUG: " +str(avg_time_in_q_DEBUG)
            wrtxt += "\navg_time_in_q_weighted_DEBUG: " +str(avg_time_in_q_weighted_DEBUG)              
            wrtxt += "\n\ntot_time_in_q_PAR: " +str(tot_time_in_q_PAR)
            wrtxt += "\ntot_time_in_q_weighted_PAR: " +str(tot_time_in_q_weighted_PAR)
            wrtxt += "\navg_time_in_q_PAR: " +str(avg_time_in_q_PAR)
            wrtxt += "\navg_time_in_q_weighted_PAR: " +str(avg_time_in_q_weighted_PAR)
            wrtxt += "\n\ntot_time_in_q_LONGPAR: " +str(tot_time_in_q_LONGPAR)
            wrtxt += "\ntot_time_in_q_weighted_LONGPAR: " +str(tot_time_in_q_weighted_LONGPAR)
            wrtxt += "\navg_time_in_q_LONGPAR: " +str(avg_time_in_q_LONGPAR)
            wrtxt += "\navg_time_in_q_weighted_LONGPAR: " +str(avg_time_in_q_weighted_LONGPAR)

            wrtxt += "\n\n\ntot_cores_used: " +str(tot_cores_used)
            wrtxt += "\navg_cores_used: " +str(avg_cores_used)
            wrtxt += "\n\ntot_gpu_used: " +str(tot_gpu_used)
            wrtxt += "\navg_gpu_used: " + str(avg_gpu_used)
            wrtxt += "\n\ntot_mic_used: " +str(tot_mic_used)
            wrtxt += "\navg_mic_used: " + str(avg_mic_used)
            wrtxt += "\n\ntot_running_jobs: " +str(tot_running_jobs)
            wrtxt += "\navg_running_jobs: " + str(avg_running_jobs)
            wrtxt += "\n\navg_perc_cores_used: " + str(avg_perc_cores_used)

            wrtxt += "\n\nN_jobs: " + str(N_jobs)
            wrtxt += "\nN_jobs_DEBUG: " + str(N_jobs_DEBUG)
            wrtxt += "\nN_jobs_PAR: " + str(N_jobs_PAR)
            wrtxt += "\nN_jobs_LONGPAR: " + str(N_jobs_LONGPAR)

            wrtxt += "\n\nenergy_consumed_AVG: " + str(energy_consumed_AVG)
            wrtxt += "\n\nenergy_consumed_WC: " + str(energy_consumed_WC)



            with(open(statsfile,'w')) as sf:
                sf.write(wrtxt)
