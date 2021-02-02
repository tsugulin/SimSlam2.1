// 満杯の場合は別ルートへの迂回を要求するタイプの待ち行列

#include "BalkingQueue.h"
Define_Module(BalkingQueue);
#include "BlockingQueue.h"

void BalkingQueue::initialize()
{
    // 変数初期化
    onProduction = false;   // 現在は生産中ではない

    // キューの初期化
    queue.setName("queue");             // GUIに待ち行列長さを表示するための名前
    numMaxQueue = (int)par("numQueue"); // omnetpp.iniの待ち行列数を取得

    // 待ち行列の平均長さを計算するクラスを初期化
    queuelen.init(simTime(), numMaxQueue);

    // 後工程のポインタを取得
    cModule *mod = gate("out_inner")->getNextGate()->getOwnerModule();
    nextNode = check_and_cast<BlockingQueue *>(mod);
}

void BalkingQueue::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // セルフメッセージ=生産完了時
        if (nextNode->checkStatus()) {
            simtime_t_cref d = simTime() - msg->getTimestamp();     // 処理時間
            proctime.collect(d);                                    // 統計情報に保管
            send(msg, "out_inner");     // 後工程に送る
            onProduction = false;       // 生産完了とする
        }
        else
            complete.insert(msg);   // 生産完了としてラインに滞留
    }
    else {
        // 前工程から物が届いたｲﾍﾞﾝﾄの場合
        if (queue.getLength() >= numMaxQueue)
            send(msg, "out_outer");     // キューに格納できる上限を超えているならば外注先に送る
        else {
            // 社内にて生産する場合
            queuelen.set(simTime(), queue.getLength()); // 待ち行列長さの加重平均値の計算
            msg->setTimestamp(simTime());   // リードタイムの開始時間をセット
            queue.insert(msg);              // 待ち行列の最後尾にメッセージを保管
        }
    }
    startProduction();      // 可能であれば生産を開始する
}

// 後工程から要求があった場合，完了したメッセージがあれば取り出して後工程に送る
void BalkingQueue::requestMessage(void)
{
    Enter_Method("requestMessage");
    if (complete.getLength() > 0) {
        cMessage *msg = check_and_cast<cMessage *>(complete.pop());    // メッセージを取り出す
        simtime_t_cref d = simTime() - msg->getTimestamp();     // 処理時間
        proctime.collect(d);                                    // 統計情報に保管
        send(msg, "out_inner");     // 後工程に送る
        onProduction = false;       // 生産完了とする
    }
    startProduction();              // 可能であれば生産を開始する
}

// 在庫があれば生産を開始する
void BalkingQueue::startProduction(void)
{
    Enter_Method("startProduction");
    if (!onProduction) {                // 生産中で無ければ
        if (queue.getLength() > 0) {    // 在庫が在れば
            queuelen.set(simTime(), queue.getLength());             // 待ち行列長さの加重平均値の計算
            cMessage *msg = check_and_cast<cMessage *>(queue.pop());    // 待ち行列先頭のメッセージを取り出す
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
void BalkingQueue::finish()
{
    EV << "Queue1 AVG Length:   " << queuelen.get(simTime()) << endl;
    EV << "Queue1 Jobs Count:   " << waittime.getCount() << endl;
    EV << "Queue1 Min WaitTime: " << waittime.getMin() << endl;
    EV << "Queue1 Avg WaitTime: " << waittime.getMean() << endl;
    EV << "Queue1 Max WaitTime: " << waittime.getMax() << endl;
    EV << "Standard deviation:  " << waittime.getStddev() << endl;
    EV << "Queue1 Min ProcTime: " << proctime.getMin() << endl;
    EV << "Queue1 Avg ProcTime: " << proctime.getMean() << endl;
    EV << "Queue1 Max ProcTime: " << proctime.getMax() << endl;
    EV << "Standard deviation:  " << proctime.getStddev() << endl;

    waittime.recordAs("Queue1 WaitTime");
    proctime.recordAs("Queue1 ProcTime");
}
