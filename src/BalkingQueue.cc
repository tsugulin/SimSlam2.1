// ���t�̏ꍇ�͕ʃ��[�g�ւ̉I���v������^�C�v�̑҂��s��

#include "BalkingQueue.h"
Define_Module(BalkingQueue);
#include "BlockingQueue.h"

void BalkingQueue::initialize()
{
    // �ϐ�������
    onProduction = false;   // ���݂͐��Y���ł͂Ȃ�

    // �L���[�̏�����
    queue.setName("queue");             // GUI�ɑ҂��s�񒷂���\�����邽�߂̖��O
    numMaxQueue = (int)par("numQueue"); // omnetpp.ini�̑҂��s�񐔂��擾

    // �҂��s��̕��ϒ������v�Z����N���X��������
    queuelen.init(simTime(), numMaxQueue);

    // ��H���̃|�C���^���擾
    cModule *mod = gate("out_inner")->getNextGate()->getOwnerModule();
    nextNode = check_and_cast<BlockingQueue *>(mod);
}

void BalkingQueue::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // �Z���t���b�Z�[�W=���Y������
        if (nextNode->checkStatus()) {
            simtime_t_cref d = simTime() - msg->getTimestamp();     // ��������
            proctime.collect(d);                                    // ���v���ɕۊ�
            send(msg, "out_inner");     // ��H���ɑ���
            onProduction = false;       // ���Y�����Ƃ���
        }
        else
            complete.insert(msg);   // ���Y�����Ƃ��ă��C���ɑؗ�
    }
    else {
        // �O�H�����畨���͂�������Ă̏ꍇ
        if (queue.getLength() >= numMaxQueue)
            send(msg, "out_outer");     // �L���[�Ɋi�[�ł������𒴂��Ă���Ȃ�ΊO����ɑ���
        else {
            // �Г��ɂĐ��Y����ꍇ
            queuelen.set(simTime(), queue.getLength()); // �҂��s�񒷂��̉��d���ϒl�̌v�Z
            msg->setTimestamp(simTime());   // ���[�h�^�C���̊J�n���Ԃ��Z�b�g
            queue.insert(msg);              // �҂��s��̍Ō���Ƀ��b�Z�[�W��ۊ�
        }
    }
    startProduction();      // �\�ł���ΐ��Y���J�n����
}

// ��H������v�����������ꍇ�C�����������b�Z�[�W������Ύ��o���Č�H���ɑ���
void BalkingQueue::requestMessage(void)
{
    Enter_Method("requestMessage");
    if (complete.getLength() > 0) {
        cMessage *msg = check_and_cast<cMessage *>(complete.pop());    // ���b�Z�[�W�����o��
        simtime_t_cref d = simTime() - msg->getTimestamp();     // ��������
        proctime.collect(d);                                    // ���v���ɕۊ�
        send(msg, "out_inner");     // ��H���ɑ���
        onProduction = false;       // ���Y�����Ƃ���
    }
    startProduction();              // �\�ł���ΐ��Y���J�n����
}

// �݌ɂ�����ΐ��Y���J�n����
void BalkingQueue::startProduction(void)
{
    Enter_Method("startProduction");
    if (!onProduction) {                // ���Y���Ŗ������
        if (queue.getLength() > 0) {    // �݌ɂ��݂��
            queuelen.set(simTime(), queue.getLength());             // �҂��s�񒷂��̉��d���ϒl�̌v�Z
            cMessage *msg = check_and_cast<cMessage *>(queue.pop());    // �҂��s��擪�̃��b�Z�[�W�����o��
            simtime_t_cref d = simTime() - msg->getTimestamp();     // �L���[�ؗ�����
            waittime.collect(d);                                    // ���v���ɕۊ�
            onProduction = true;                                    // ���Y���Ƃ���
            msg->setTimestamp(simTime());                           // ���[�h�^�C���̊J�n���Ԃ��Z�b�g
            scheduleAt(simTime() + par("productionTime"), msg);     // ���Y�I�����SelfMessage�𑗂�
        }
    }
    if (ev.isGUI()) getDisplayString().setTagArg("i",1, queue.isEmpty() ? "" : "cyan3");    // �݌ɂɍ��킹�ăm�[�h�̐F��ύX
}

//�@�m�[�h�̓��v����\��
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
