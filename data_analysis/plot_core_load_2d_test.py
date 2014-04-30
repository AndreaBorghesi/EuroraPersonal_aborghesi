#! /usr/bin/python

'''
Author: Andrea Borghesi
    Plot load data 
'''

import pickle
import matplotlib.pyplot as plt
from datetime import datetime

DDIR="/media/sda4/eurora/data_analysis/data/20140212_000000_20140214_235959/5min_sample_time/"
PLOTDIR=DDIR+"plots/"

NNODE = 64 

# we analyse the data for each node
for node in range(1,NNODE+1):
    try:
        # load dictionary with load data from file
        dict_file = DDIR + "%03d" % (node) + "_dump.p"
        avgLoadDict = pickle.load(open(dict_file,"rb"))
        
        # create 4 subplot, each one plotting load values for 4 cores
        #fig=plt.figure()
        f, (ax1, ax2, ax3, ax4) = plt.subplots(4, sharex=True, sharey=True) 

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
                dates, values = zip(*measurementsList)
                if (core_int<4):
                    ax1.plot(dates, values, label='Core %s' % (core))
                    ax1.legend(loc='center left', bbox_to_anchor=(1, 0.5), fancybox=True, shadow=True)
                if (core_int >= 4 and core_int<8):
                    ax2.plot(dates, values, label='Core %s' % (core))
                    ax2.legend(loc='center left', bbox_to_anchor=(1, 0.5), fancybox=True, shadow=True)
                if (core_int >= 8 and core_int<12):
                    ax3.plot(dates, values, label='Core %s' % (core))
                    ax3.legend(loc='center left', bbox_to_anchor=(1, 0.5), fancybox=True, shadow=True)
                if (core_int>=12):
                    ax4.plot(dates, values, label='Core %s' % (core))
                    ax4.legend(loc='center left', bbox_to_anchor=(1, 0.5), fancybox=True, shadow=True)
                #plt.plot(dates, values, label='Core %s' % (core))
        #plt.ylabel('Load')
        #plt.xlabel('Time')
        #plt.legend()
        f.subplots_adjust(hspace=0)
        plt.setp([a.get_xticklabels() for a in f.axes[:-1]], visible=False)
        #plt.savefig("test.pdf",dpi=1000)
        #plt.show()
        figure = plt.gcf()
        figure.set_size_inches(17,15)

        plotfile = PLOTDIR + "%03d" % (node) + "_plot.pdf"
        plt.savefig(plotfile,dpi=100)
                

    except:
        print "Something bad happened somewhere.. (well, to be more precise, while parsing node %03d" % (node)
