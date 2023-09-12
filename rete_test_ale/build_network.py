import sys,os

DEFINITION_PT1="""network Topology
{
    parameters:
        """
DEFINITION_PT2="""int numinfected = {infected};
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

    if len(sys.argv) < 3 or not sys.argv[1].isnumeric() or not sys.argv[2].isnumeric():
        raise Exception("you should specify num of modules and num of infected modules")

    network = DEFINITION_PT1 + DEFINITION_PT2.format(infected=sys.argv[2]) + DEFINITION_PT3 + DEFINITION_PT4.format(processes=sys.argv[1])
    for i in range(int(sys.argv[1])-1):
        for j in range(int(sys.argv[1])-1 - i):
            network += "\t\tprocess[i].gate++ <--> Link <--> process[i].gate++; \n"
    network += "}"


    with open("Network.ned","w") as ntw:
        ntw.write(network)