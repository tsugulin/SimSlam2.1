simple Create
{
    parameters:
        volatile double createIntervalTime @unit(s) = default(0);
    gates:
        output out;
}