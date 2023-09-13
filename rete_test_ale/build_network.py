import sys,os
from _ast import Or
from pip._vendor.distlib.util import OR

DEFINITION_PT1="""network Topology
{
    parameters:
        """
DEFINITION_PT2="""int numinfected = {infected};
        int seed = {seed};
        int maxMaintainRounds = {maxRounds};
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


if name == "main":
    maxRounds = 5
    seed = 42

    if (len(sys.argv) < 3 or not sys.argv[1].isnumeric() or
         not sys.argv[2].isnumeric() or int(sys.argv[1]) <= 0 or
          int(sys.argv[2]) <= 0 or int(sys.argv[1]) < int(sys.argv[2])):
        raise Exception("you should specify num of modules and num of infected modules")
    elif len(sys.argv) == 4:
        maxRounds = sys.argv[3]
    elif len(sys.argv) == 5:
        seed = sys.argv[4]
        maxRounds = sys.argv[3]
    network = DEFINITION_PT1 + DEFINITION_PT2.format(infected=sys.argv[2],seed=seed,maxRounds=maxRounds) + DEFINITION_PT3 + DEFINITION_PT4.format(processes=sys.argv[1])
    for i in range(int(sys.argv[1])-1):
        for j in range(int(sys.argv[1])-1 - i):
            network += "\t\tprocess[i].gate++ <--> Link <--> process[i].gate++; \n"
    network += "}"


    with open("Network.ned","w") as ntw:
        ntw.write(network)