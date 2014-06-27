''' Andrea Borghesi, University of Bologna '''
''' Class to represent a node in EURORA system '''

NNODE = 64

class Node():
    def __init__ (self, nNode, nCores, nGpus, nMics, mem, reserved):
        self.nNodes = nNode
        self.nCores = nCores
        self.nGpus = nGpus
        self.nMics = nMics
        self.mem = mem
        self.reserved = reserved
        
        self.nominalFrequency = 0
        self.rFreqMEMbound = 0 
        self.rFreqCPUbound = 0
        self.energyMEMbound = 0
        self.energyCPUbound = 0

    def humanPrintNode (self):
        outstr = "Node: " + str(self.nNode) + " -- N.Cores: " + str(self.nCores) + " - N.Gpus: " + str(self.nGpus) + " - N.Mics: " + str(self.nMics) + " - Mem: " + str(self.mem) + " - Reserved: " + str(self.reserved) + " - Nom.Freq: " + str(self.nominalFrequency) + " - R.FreqMEM: " + str(self.rFreqMEMbound) + " - R.FreqCPU: " + str(self.rFreqCPUbound) + " - EnergyMEM: " + str(self.energyMEMbound) + " - EnergyCPU: " + str(self.energyCPUbound)
        return outstr

''' Class to read from files info about nodes '''
class NodeReader():
    def __init__ (self):
        self.nodes = {}

    def readNodes(self, nodesFile):
        " Read nodes properties "
        with open(nodesFile,'r') as f:
            i = 0
            for line in f:
                data = line.split()
                n = Node(int(data[0]),int(data[1]),int(data[2]),int(data[3]),int(data[4]),int(data[5]))
                self.nodes[i] = n
                i+=1

    def updateNodes(self, nomFreqFile, rFreqMEMFile, rFreqCPUFile, energyMEMFile, energyCPUFile):
        " Read node info about energies "
        #since on EURORA we have some nodes not working or we miss some data (for example on nodes used only for debug)
        #but in our model we want to energetically rank all nodes we make some assumpionts for the missing values
        for n,node in self.nodes.items():
            if(int(n)<32):
                node.nominalFrequency = 2100.0
                node.rFreqMEMbound = 2100.0 
                node.rFreqCPUbound = 2100.0
                node.energyMEMbound = 61500.0
                node.energyCPUbound = 52500.0
            else:
                node.nominalFrequency = 3100.0
                node.rFreqMEMbound = 3400.0 
                node.rFreqCPUbound = 3400.0
                node.energyMEMbound = 103000.0
                node.energyCPUbound = 62500.0

        with open(nomFreqFile,'r') as f:
            for line in f:
                data = line.split()
                self.nodes[data[0]].nominalFrequency = float(data[1])
        with open(rFreqMEMFile,'r') as f:
            for line in f:
                data = line.split()
                self.nodes[data[0]].rFreqMEMbound = float(data[1])
        with open(rFreqCPUFile,'r') as f:
            for line in f:
                data = line.split()
                self.nodes[data[0]].rFreqCPUbound = float(data[1])
        with open(energyMEMFile,'r') as f:
            for line in f:
                data = line.split()
                self.nodes[data[0]].energyMEMbound = float(data[1])
        with open(energyCPUFile,'r') as f:
            for line in f:
                data = line.split()
                self.nodes[data[0]].energyCPUbound = float(data[1])

    def printTest(self):
        for n,node in self.nodes.items():
            print n
            print node.humanPrintNode()

