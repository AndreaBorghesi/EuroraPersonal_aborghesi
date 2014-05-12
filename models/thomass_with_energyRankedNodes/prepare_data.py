#! /usr/bin/python

'''
Author Andrea Borghesi
-- Simple script which just convert the data needed for energy ranking from ffrat to tbrid format
'''

import sys
import re
import os
from subprocess import Popen, PIPE
import datetime

LOGSDIR = "/media/sda4/eurora/models/thomass_with_energyRankedNodes"

for subdir, dirs, files in os.walk(LOGSDIR):
    for log in files:
        if log.endswith(".log"):
            f = open(log,"r")
            lines=f.readlines()
            f.close()
            prepared_logs = log + "processed"
            f=open(prepared_logs, 'w')
            for line in lines:
                data = line.split(';')
                wrtxt = data[0] + " " + data[1] 
                f.write(wrtxt)
            f.close()
        

