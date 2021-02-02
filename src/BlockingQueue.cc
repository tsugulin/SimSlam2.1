// ���t�̏ꍇ�͑O�H�����u���b�N����^�C�v�̑҂��s��

#include "BlockingQueue.h"
Define_Module(BlockingQueue);
#include "BalkingQueue.h"

void BlockingQueue::initialize()
{
    // �ϐ�������
    onProduction = false;   // ���݂͐��Y���ł͂Ȃ�

    // �L���[�̏�����
    queue.setName("queue");             // GUI�ɑ҂��s�񒷂���\�����邽�߂̖��O
    numMaxQueue = (int)par("numQueue"); // omnetpp.ini�̑҂��s�񐔂��擾

    // �҂��s��̕��ϒ������v�Z����N���X��������
    queuelen.init(simTime(), numMaxQueue);

    // �O�H���̃|�C���^���擾
    cModule *mod = gate("in")->getPreviousGate()->getOwnerModule();
    beforeNode = check_and_cast<BalkingQueue *>(mod);
}

void BlockingQueue::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() ) {
        // �Z���t���b�Z�[�W=���Y������
        simtime_t_cref d = simTime() - msg->getTimestamp();     // ��������
        proctime.collect(d);                                    // ���v���ɕۊ�
        send(msg, "out");       // ��H���ɑ���
        onProduction = false;   // ���Y�����Ƃ���
    }
    else {
        // �O�H������͂������b�Z�[�W�̏ꍇ
        queuelen.set(simTime(), queue.getLength()); // �҂��s�񒷂��̉��d���ϒl�̌v�Z
        msg->setTimestamp(simTime());   // ���[�h�^�C���̊J�n���Ԃ��Z�b�g
        queue.insert(msg);              // �҂��s��̍Ō���Ƀ��b�Z�[�W��ۊ�
    }
    // ���Y���s��
    startProduction();
}

// �O�H������⍇�����������ꍇ�C�󂯓���ۂ��񓚂���
bool BlockingQueue::checkStatus(void)
{
    Enter_Method("checkStatus");
    return (queue.getLength() < numMaxQueue) ? true : false; // �L���[�ɗ]�T�������OK�Ɖ񓚂���
}

// �݌ɂ�����ΐ��Y���J�n����
void BlockingQueue::startProduction(void)
{
    Enter_Method("startProduction");
    if (!onProduction) {                // ���Y���Ŗ������
        if (queue.getLength() > 0) {    // �݌ɂ��݂��
            // ���Y�\�ȏꍇ
            queuelen.set(simTime(), queue.getLength());             // �҂��s�񒷂��̉��d���ϒl�̌v�Z
            cMessage *msg = check_and_cast<cMessage *>(queue.pop());    // �҂��s��擪�̃��b�Z�[�W�����o��
            beforeNode->requestMessage();                           // �݌ɂɗ]�T���o�����̂őO�H���Ƀ��b�Z�[�W��v������
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
