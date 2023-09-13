import sys,os
from _ast import Or
from pip._vendor.distlib.util import OR

DEFINITION_PT1="""network Topology
{
    parameters:
        """
DEFINITION_PT2="""int numInfected = {infected};
        int seed = {seed};
        int maxMaintainRounds = {maxRounds};
        bool logs = {log};
    types:
        """
DEFINITION_PT3="""channel Link extends ned.DelayChannel{
            delay = 1000ms;
        }
    submodules:
        """
DEFINITION_PT4="""process[{processes}]: process ;

    connections:
"""


if __name__ == "__main__":
    maxRounds = 5
    seed = 42
    log = 0

    if (len(sys.argv) < 3 or not sys.argv[1].isnumeric() or
         not sys.argv[2].isnumeric() or int(sys.argv[1]) <= 0 or
          int(sys.argv[2]) <= 0 or int(sys.argv[1]) < int(sys.argv[2])):
        raise Exception("you should specify num of modules and num of infected modules")
    if len(sys.argv) >= 4:
        maxRounds = sys.argv[3]
    if len(sys.argv) >= 5:
        seed = sys.argv[4]
        
    elif len(sys.argv) >= 6:
        log = sys.argv[5]
    network = DEFINITION_PT1 + DEFINITION_PT2.format(infected=sys.argv[2],seed=seed,maxRounds=maxRounds,
                        log="true" if log == 1 else "false") + DEFINITION_PT3 + DEFINITION_PT4.format(processes=sys.argv[1])
    for i in range(int(sys.argv[1])-1):
        for j in range(int(sys.argv[1])-1,i,-1):
            network += "\t\tprocess[" + str(i) + "].gate++ <--> Link <--> process[" + str(j) + "].gate++; \n"
    network += "}"


    with open("topology.ned","w") as ntw:
        ntw.write(network)