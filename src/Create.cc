// メッセージを繰り返し生産するノード

#include "Create.h"
Define_Module(Create);

void Create::initialize()
{
    beatMsg = new cMessage("beat");
    scheduleAt(0.0, beatMsg);
}

void Create::handleMessage(cMessage *msg)
{
    // CREATEノードに届くメッセージは全てbeatMsg
    ASSERT(msg == beatMsg);

    // 次のモジュールにメッセージを送信
    send(new cMessage("data"), "out");

    //　createIntervalTimeの期間毎に"heartbeat"メッセージを自身に送信する
    double d = par("createIntervalTime");
    scheduleAt(simTime() + d, beatMsg);
    leadtime.collect(d);
}

//　ノードの統計情報を表示
void Create::finish()
{
    EV << "Create Jobs Count:   " << leadtime.getCount() << endl;
    EV << "Create Min Leadtime: " << leadtime.getMin() << endl;
    EV << "Create Avg Leadtime: " << leadtime.getMean() << endl;
    EV << "Create Max Leadtime: " << leadtime.getMax() << endl;
    EV << "Standard deviation:  " << leadtime.getStddev() << endl;

    leadtime.recordAs("Create LeadTime");
}
