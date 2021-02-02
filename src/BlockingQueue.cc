// 満杯の場合は前工程をブロックするタイプの待ち行列

#include "BlockingQueue.h"
Define_Module(BlockingQueue);
#include "BalkingQueue.h"

void BlockingQueue::initialize()
{
    // 変数初期化
    onProduction = false;   // 現在は生産中ではない

    // キューの初期化
    queue.setName("queue");             // GUIに待ち行列長さを表示するための名前
    numMaxQueue = (int)par("numQueue"); // omnetpp.iniの待ち行列数を取得

    // 待ち行列の平均長さを計算するクラスを初期化
    queuelen.init(simTime(), numMaxQueue);

    // 前工程のポインタを取得
    cModule *mod = gate("in")->getPreviousGate()->getOwnerModule();
    beforeNode = check_and_cast<BalkingQueue *>(mod);
}

void BlockingQueue::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // セルフメッセージ=生産完了時
        simtime_t_cref d = simTime() - msg->getTimestamp();     // 処理時間
        proctime.collect(d);                                    // 統計情報に保管
        send(msg, "out");       // 後工程に送る
        onProduction = false;   // 生産完了とする
    }
    else {
        // 前工程から届いたメッセージの場合
        queuelen.set(simTime(), queue.getLength()); // 待ち行列長さの加重平均値の計算
        msg->setTimestamp(simTime());   // リードタイムの開始時間をセット
        queue.insert(msg);              // 待ち行列の最後尾にメッセージを保管
    }
    // 生産を行う
    startProduction();
}

// 前工程から問合せがあった場合，受け入れ可否を回答する
bool BlockingQueue::checkStatus(void)
{
    Enter_Method("checkStatus");
    return (queue.getLength() < numMaxQueue) ? true : false; // キューに余裕があればOKと回答する
}

// 在庫があれば生産を開始する
void BlockingQueue::startProduction(void)
{
    Enter_Method("startProduction");
    if (!onProduction) {                // 生産中で無ければ
        if (queue.getLength() > 0) {    // 在庫が在れば
            // 生産可能な場合
            queuelen.set(simTime(), queue.getLength());             // 待ち行列長さの加重平均値の計算
            cMessage *msg = check_and_cast<cMessage *>(queue.pop());    // 待ち行列先頭のメッセージを取り出す
            beforeNode->requestMessage();                           // 在庫に余裕が出来たので前工程にメッセージを要求する
            simtime_t_cref d = simTime() - msg->getTimestamp();     // キュー滞留時間
            waittime.collect(d);                                    // 統計情報に保管
            onProduction = true;                                    // 生産中とする
            msg->setTimestamp(simTime());                           // リードタイムの開始時間をセット
            scheduleAt(simTime() + par("productionTime"), msg);     // 生産終了後にSelfMessageを送る
        }
    }
    if (ev.isGUI()) getDisplayString().setTagArg("i",1, queue.isEmpty() ? "" : "cyan3");    // 在庫に合わせてノードの色を変更
}

//　ノードの統計情報を表示
void BlockingQueue::finish()
{
    EV << "Queue2 AVG Length:   " << queuelen.get(simTime()) << endl;
    EV << "Queue2 Jobs Count:   " << waittime.getCount() << endl;
    EV << "Queue2 Min WaitTime: " << waittime.getMin() << endl;
    EV << "Queue2 Avg WaitTime: " << waittime.getMean() << endl;
    EV << "Queue2 Max WaitTime: " << waittime.getMax() << endl;
    EV << "Standard deviation:  " << waittime.getStddev() << endl;
    EV << "Queue2 Min ProcTime: " << proctime.getMin() << endl;
    EV << "Queue2 Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Queue2 Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation:  " << proctime.getStddev() << endl;

    waittime.recordAs("Queue2 WaitTime");
    proctime.recordAs("Queue2 ProcTime");
}
