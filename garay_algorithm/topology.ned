network Topology
{
    parameters:
        int numInfected = default(1);
        int seed = default(41);
        bool logs = default(false);
        int numProcesses = default(7);
        int maxMaintainRounds = default(numProcesses+1);
        int infectionSpeed = default(1);
        int numExperiments = default(60);
    types:
        channel Link extends ned.DelayChannel{
            delay = exponential(1000ms);
        }
    submodules:
        process[numProcesses]: process ;

    connections:
        
        for i=0 .. numProcesses-2, for j=i+1 .. numProcesses-1 {
            process[i].gate++ <--> Link <--> process[j].gate++;
            
        }
        
}