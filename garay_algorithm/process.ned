//
simple process {

//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

		parameters:
            @signal[decision](type="int");
            @statistic[rounds](title="num rounds"; source="decision"; record=vector; interpolationmode=none);
            @signal[roundTime](type="SimTime");
            @statistic[time](title="hop count"; source="roundTime"; record=vector,stats; interpolationmode=none);
        
        gates:
        	inout gate[];

}

