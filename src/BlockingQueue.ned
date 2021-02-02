simple BlockingQueue
{
    parameters:
        int numQueue = default(0);
        volatile double productionTime @unit(s) = default(0);
    gates:
        input in;
        output out;
}
