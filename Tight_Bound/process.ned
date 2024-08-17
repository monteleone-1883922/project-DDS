//
simple process
{
    parameters:
        @signal[decision](type="int");
        @statistic[rounds](title="num rounds"; source="decision"; record=vector; interpolationmode=none);
        @signal[roundTime](type="SimTime");
        @statistic[time](title="hop count"; source="roundTime"; record=vector,stats; interpolationmode=none);
        
        
    gates:
        inout gate[];

}

