network Topology
{
    parameters:
        int numInfected = default(1);
        int seed = default(42);
        int maxMaintainRounds = 1;
        bool logs = default(false);
        int numProcesses = default(6);
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