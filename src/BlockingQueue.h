#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include <omnetpp.h>
using namespace omnetpp;
#define ev  (*cSimulation::getActiveEnvir())
#include "Wavg.h"

class BalkingQueue;

class BlockingQueue : public cSimpleModule
{
  private:
    bool onProduction;
    cQueue queue;
    int numMaxQueue;
    BalkingQueue *beforeNode;
    cStdDev waittime;
    cStdDev proctime;
    Wavg queuelen;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void startProduction();

  public:
    virtual bool checkStatus(void);
};

#endif /* BLOCKINGQUEUE_H_ */
