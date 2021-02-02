simple BalkingQueue
{
    parameters:
        int numQueue = default(0);
        volatile double productionTime @unit(s) = default(0);
	gates:
        input in;
        output out_inner;
        output out_outer;
}
