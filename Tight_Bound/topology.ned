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
        process[6]: process ;

    connections:
		process[0].gate++ <--> Link <--> process[5].gate++; 
		process[0].gate++ <--> Link <--> process[4].gate++; 
		process[0].gate++ <--> Link <--> process[3].gate++; 
		process[0].gate++ <--> Link <--> process[2].gate++; 
		process[0].gate++ <--> Link <--> process[1].gate++; 
		process[1].gate++ <--> Link <--> process[5].gate++; 
		process[1].gate++ <--> Link <--> process[4].gate++; 
		process[1].gate++ <--> Link <--> process[3].gate++; 
		process[1].gate++ <--> Link <--> process[2].gate++; 
		process[2].gate++ <--> Link <--> process[5].gate++; 
		process[2].gate++ <--> Link <--> process[4].gate++; 
		process[2].gate++ <--> Link <--> process[3].gate++; 
		process[3].gate++ <--> Link <--> process[5].gate++; 
		process[3].gate++ <--> Link <--> process[4].gate++; 
		process[4].gate++ <--> Link <--> process[5].gate++; 
}