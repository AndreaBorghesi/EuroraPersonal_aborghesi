#! /usr/bin/python

'''
Author: Andrea Borghesi
    Plot load data 
'''

import pickle
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime

DDIR="/media/sda4/eurora/data_analysis/data/20140219_000000_140000/5min_sample_time/"

NNODE = 3

# we analyse the data for each node
for node in range(3,NNODE+1):
    try:
        # load dictionary with load data from file
        dict_file = DDIR + "%03d" % (node) + "_dump.p"
        avgLoadDict = pickle.load(open(dict_file,"rb"))
        
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')

        for key,values in avgLoadDict.items():
#            print key
#            print values
            for v in values:
                #print v
                temp_dict = {}
                measurementsList = []
                for core,(ld,d1,d2) in v:
                    core_int=int(core)
                    sample_start_time = datetime.strptime(d1,"%Y-%m-%d %H:%M:%S")
                    sample_end_time = datetime.strptime(d2,"%Y-%m-%d %H:%M:%S")
                    temp_dict[sample_start_time]=ld
                    temp_dict[sample_end_time]=ld
#                    temp_dict[(core_int,ld)]=sample_start_time
#                    temp_dict[(core_int,ld)]=sample_end_time
                #if(core_int==14):
                measurementsList = [(date,ld) for date,ld in temp_dict.items()]
                #print temp_dict.items()
                measurementsList.sort()
                #print measurementsList
                dates, values =  zip(*measurementsList)
#                for d,v in zip(dates,values):
#                    print d
#                    print v
#                    print core_int
#                    ax.bar(d,core_int,zs=v,zdir='y', color=cs, alpha=0.8)
                print core_int
                print dates
                print values
                cores=np.empty(len(values))
                cores.fill(core_int)
                print cores
                ax.bar(dates,values,zs=cores,zdir='y', alpha=0.8)
                
        #plt.ylabel('Load')
        #plt.xlabel('Time')
        ax.set_xlabel('Time')
        ax.set_ylabel('Core')
        ax.set_zlabel('Load')
        #plt.legend()
        plt.show()
                

    except:
        print "Something bad happened somewhere.."
