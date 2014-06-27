''' Andrea Borghesi, University of Bologna '''
''' Class to represent a queue in EURORA system '''

NNODE = 64

class Queue():
    def __init__ (self, qid, maxWaitTime):
        self.qid = qid 
        self.maxWaitTime = maxWaitTime

    def humanPrintQueue (self):
        outstr = "Queue: " + self.qid + " -- Max.WaitTime: " + str(self.maxWaitTime) 
        return outstr

''' Class to read from files info about queues '''
class QueueReader():
    def __init__ (self):
        self.queues = {}

    def readQueues(self, nodesFile):
        " Read queues properties "
        with open(nodesFile,'r') as f:
            i = 0
            for line in f:
                data = line.split()
                q = Queue(data[0],int(data[1]))
                self.queues[i] = q 
                i+=1

    def printTest(self):
        for q,queue in self.queues.items():
            print q 
            print queue.humanPrintQueue()

