network Topology
{
    parameters:
        int numInfected = 1;
        int seed = 42;
        int maxMaintainRounds = 5;
        bool logs = false;
    types:
        channel Link extends ned.DelayChannel{
            delay = 1000ms;
        }
    submodules:
        process[4]: process ;
        
    connections:
        //PROCESS 0
        
        process[0].gate++ <--> Link <--> process[1].gate++;
        process[0].gate++ <--> Link <--> process[2].gate++;
        process[0].gate++ <--> Link <--> process[3].gate++;
        
        process[1].gate++ <--> Link <--> process[2].gate++;
        process[1].gate++ <--> Link <--> process[3].gate++;
        
        process[2].gate++ <--> Link <--> process[3].gate++;
        
}