
#include "process.h"


using namespace omnetpp;

class process: public cSimpleModule {

private:
    int dec;
    int v;
    int* sv;
    int* ev;


protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual int propose(int v);
    virtual int collect(int *v);


};

Define_Module(Process);

void Process::initialize()
{
    // TODO - Generated method body
}

void Process::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
