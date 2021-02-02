#ifndef BALKINGQUEUE_H_
#define BALKINGQUEUE_H_

#include <omnetpp.h>
using namespace omnetpp;
#define ev  (*cSimulation::getActiveEnvir())
#include "Wavg.h"

class BlockingQueue;

class BalkingQueue : public cSimpleModule
{
  private:
    bool onProduction;
    cQueue queue;
    cQueue complete;
    int numMaxQueue;
    BlockingQueue *nextNode;
    cStdDev waittime;
    cStdDev proctime;
    Wavg queuelen;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void startProduction();

  public:
    virtual void requestMessage(void);
};

#endif /* BALKINGQUEUE_H_ */
