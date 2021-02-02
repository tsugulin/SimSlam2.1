// ���b�Z�[�W���J��Ԃ����Y����m�[�h

#include "Create.h"
Define_Module(Create);

void Create::initialize()
{
    beatMsg = new cMessage("beat");
    scheduleAt(0.0, beatMsg);
}

void Create::handleMessage(cMessage *msg)
{
    // CREATE�m�[�h�ɓ͂����b�Z�[�W�͑S��beatMsg
    ASSERT(msg == beatMsg);

    // ���̃��W���[���Ƀ��b�Z�[�W�𑗐M
    send(new cMessage("data"), "out");

    //�@createIntervalTime�̊��Ԗ���"heartbeat"���b�Z�[�W�����g�ɑ��M����
    double d = par("createIntervalTime");
    scheduleAt(simTime() + d, beatMsg);
    leadtime.collect(d);
}

//�@�m�[�h�̓��v����\��
void Create::finish()
{
    EV << "Create Jobs Count:   " << leadtime.getCount() << endl;
    EV << "Create Min Leadtime: " << leadtime.getMin() << endl;
    EV << "Create Avg Leadtime: " << leadtime.getMean() << endl;
    EV << "Create Max Leadtime: " << leadtime.getMax() << endl;
    EV << "Standard deviation:  " << leadtime.getStddev() << endl;

    leadtime.recordAs("Create LeadTime");
}
