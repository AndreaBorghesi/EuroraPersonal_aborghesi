#! /usr/bin/python

'''
Author Andrea Borghesi
'''

import sys
import re
import os
from subprocess import Popen, PIPE
import datetime

LOGSDIR = "/media/sda4/eurora/data_analysis/job_traces"

for subdir, dirs, files in os.walk(LOGSDIR):
    for jobs_log in files:
        if jobs_log.endswith(".log"):
            f = open(jobs_log,"r")
            interval_start_time = jobs_log[:15]
            interval_start_time = datetime.datetime.strptime(interval_start_time,"%Y%m%d_%H%M%S")
            ift = jobs_log[:22]
            interval_finish_time_as_string = ift[:9] + ift[-6:]
#            print ift
#            print ift[:9]
#            print ift[-6:]
#            print interval_finish_time
            
            interval_finish_time = datetime.datetime.strptime(interval_finish_time_as_string,"%Y%m%d_%H%M%S")
            interval_finish_time = datetime.datetime.strftime(interval_finish_time,"%Y-%m-%d %H:%M:%S")
            print interval_start_time
            print interval_finish_time
            lines=f.readlines()
            f.close()
            prepared_jobs = jobs_log + "processed"
            f=open(prepared_jobs, 'w')
            for line in lines:
                data = line.split('__')
                if(len(data)==3 and data[2]!='\n' and data[1]!=''):
                    job_info = data[0].split(';')
                    job_id_string = job_info[0]
                    job_name = job_info[1]
                    user = job_info[2]
                    queue = job_info[3]
                    start_time = job_info[4]
                    start_time = datetime.datetime.strptime(start_time,"%Y-%m-%d %H:%M:%S")
                    if(start_time <= interval_start_time):
                        start_time = interval_start_time
                    wrtxt = job_id_string + ";" + job_name + ";" + user + ";" + queue + ";" + str(start_time) + "__"
                    job_resources = data[1].split('#')
                    if(len(job_resources)>1):
                        resources_temp = []
                        for job_res in job_resources:
                            job_res_split = job_res.split(';')
                            resources_temp.append((job_res_split))
                        resources = [x for x in resources_temp if x!=[""]]
                        str_res = ""
                        for r in resources:
                            str_res += r[0] + ";" + r[1] + ";" + r[2] + ";" + r[3] + ";" + r[4] + "#"
                        wrtxt += str_res
                        job_times_req = data[2].split(';')
                        if(len(job_times_req)==9):
                            run_start_time = job_times_req[0]
                            run_start_time = datetime.datetime.strptime(run_start_time,"%Y-%m-%d %H:%M:%S")
                            if(run_start_time <= interval_start_time):
                                run_start_time = interval_start_time
                            wrtxt += "__" + str(run_start_time) + ";"
                            end_time = job_times_req[1]
                            if(end_time=="None"): # job not finished in the timespan
                               end_time=interval_finish_time
                            if(end_time!="None"): # job  finished in the timespan considered
                                end_time = datetime.datetime.strptime(end_time,"%Y-%m-%d %H:%M:%S")
                            node_req = job_times_req[2]
                            cpu_req = job_times_req[3]
                            mem_req = job_times_req[4]
                            time_req = job_times_req[5]
                            wrtxt += str(end_time) + ";" + node_req + ";" + cpu_req + ";" + mem_req + ";" + time_req + ";"
                            deleted = job_times_req[6]
                            dependency = job_times_req[7]
                            exit_status = job_times_req[8].rstrip()
                            wrtxt += deleted + ";" + dependency + ";" + exit_status + "\n"

                f.write(wrtxt)
            f.close()
        

