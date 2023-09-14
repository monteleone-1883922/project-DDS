#include <random>
#include "process.h"
#include <omnetpp.h>
#include "proposal_m.h"
#include "collect_m.h"
#include "decide_m.h"
#include "maintain_m.h"
#include <algorithm>
#include <iostream>
#include <fstream>


using namespace omnetpp;

class process: public cSimpleModule {

private:
    int *PV; //store the Proposed Value
    int numSubmodules; //number of processes in the network
    int myNum; //the number chosen
    bool infected = 0; //shows if the process is infected
    int decided;
    int *SV;
    int **Ev;
    int receivedPV = 0;
    int receivedSV = 0;
    int receivedEv = 0;
    int receivedMaintain = 0;
    int maintainRounds = 0;
    int *RV;
    int s = 0;
    std::string logFile;
    int numInfected;
    std::vector<int> infectableProcesses;
    int indexCorrectProcess;
    int *result;
    std::mt19937 infectionRng;
    cModule *network;
    bool log;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void printVector(int id, int *vector, std::string nomeVettore, int numSubmodules);
    virtual int canDecide( int minNumber, int *list);
    virtual void propose(int v, int numProcesses, bool infected);
    virtual void collect(int v, int numProcesses, bool infected);
    virtual void decide(int v, int *SV,int numSubmodules, bool infected);
    virtual void maintain(int decision, int numProcesses, bool infected);
    virtual void createNewArrayInfectable(int numProcesses, std::vector<int> *processes, int correctProcess);
    virtual bool generateInfections(std::vector<int> *processes, int numInfected, int process,
            std::mt19937* rng,std::ofstream& file,bool log);
    virtual int* createAndInitArray(int size);
    virtual void initArray(int* array, int size);
    virtual int** createAndInit2dArray(int size);
    virtual void logVector(std::ofstream& out, int *vector, int numSubmodules);

};

Define_Module(process);



bool process::generateInfections(std::vector<int> *processes, int numInfected,
        int process, std::mt19937 *rng, std::ofstream& file, bool log) {

    std::shuffle(processes->begin(), processes->end(), *rng);
    if (log) {
        std::string vec = "";
        for (int i = 0; i < numSubmodules-1; i++) {
            vec += std::to_string((*processes)[i]) + " | ";
        }
        file << "list of infected = " << vec << std::endl;
    }
    for (int i = 0; i < numInfected; i++) {
        if ((*processes)[i] == process) {
            return 1;
        }
    }
    return 0;
}


int* process::createAndInitArray(int size){
    int* array = new int[size]();
    initArray(array, size);
    return array;
}

void process::initArray(int* array, int size){

    for (int i = 0; i < size; i++) {
        array[i] = -1;
    }
}

int** process::createAndInit2dArray(int size){
    int** array = new int*[size]();
    for (int i = 0; i < size; i++) {
        array[i] = createAndInitArray(size);
    }
    return array;
}



void process::createNewArrayInfectable(int numProcesses, std::vector<int> *processes, int correctProcess) {


    for (int i = 0 ; i < numProcesses; i++){
        if (i < correctProcess)
            processes->push_back(i);
        else
            processes->push_back(i+1);
    }
    processes->pop_back();
}


void process::printVector(int id, int *vector, std::string nomeVettore,
        int numSubmodules) {

    std::string vec = "";
    for (int i = 0; i < numSubmodules; i++) {
        vec += std::to_string(vector[i]) + " | ";
    }
    EV << id << " vettore " + nomeVettore + " = " << vec << std::endl;
}


void process::logVector(std::ofstream& out, int *vector, int numSubmodules) {

    std::string vec = "";
    for (int i = 0; i < numSubmodules; i++) {
        vec += std::to_string(vector[i]) + " | ";
    }
    out << vec << std::endl;
}

int process::canDecide(int minNumber, int *list) {

    int count0 = 0;
    int count1 = 0;
    for (int i = 0; i < numSubmodules; i++) {

        if (list[i] != -1) {
            if (list[i] == 0) {
                count0++;
                if (count0 >= minNumber)
                    return 0;
            } else {
                count1++;
                if (count1 >= minNumber)
                    return 1;
            }
        }
    }

    return -1;

}
void process::maintain(int decision,int numProcesses, bool infected) {
    for (int i = 0; i < numProcesses; i++) {
        Maintain *msg = new Maintain("Maintain");
        if (!infected) {
            msg->setFinalDecision(decision);
        }
        else{
            int w = intuniform(-1,1);
            msg->setFinalDecision(w);
        }
        msg->setSender(getIndex());
        if (i == numSubmodules - 1) {
            scheduleAt(simTime(), msg);
        } else {
            send(msg, "gate$o", i);
        }
    }
}
void process::propose(int v,int numProcesses, bool infected) {

    for (int i = 0; i < numProcesses; i++) {
        ProposalMsg *msg = new ProposalMsg("propose");
        if (!infected) {
            msg->setValue(v);
        } else {

            // generate a random int number between 0 and 1
            int w = intuniform(-1,1);
            msg->setValue(w);
            EV << "mando sul gate" << i << "il valore " << w << endl;
        }

        msg->setSender(getIndex());
        if (i == numSubmodules - 1) {
            scheduleAt(simTime(), msg);
        } else {
            send(msg, "gate$o", i);
        }
    }
}
void process::collect(int v,int numProcesses, bool infected) {

    for (int i = 0; i < numProcesses; i++) {
        CollectMsg *msg = new CollectMsg("collect");
        if (!infected) {
            msg->setValue(v);
        } else {

            // generate a random int number between 0 and 1
            int w = intuniform(-1,1);
            msg->setValue(w);
            EV << "mando sul gate" << i << "il valore " << w << endl;
        }
        msg->setSender(getIndex());
        if (i == numSubmodules - 1) {
            scheduleAt(simTime(), msg);
        } else {
            send(msg, "gate$o", i);
        }
    }
}
void process::decide(int value, int *Sv,int numSubmodules, bool infected) {
    Decide *msg = new Decide("decide");
    msg->setDataArraySize((numSubmodules));
    //put SV into the message
    for (int i = 0; i < numSubmodules; i++) {
        if (!infected)
            msg->setData(i, Sv[i]);
        else {
            msg->setData(i, intuniform(-1,1));
        }

        msg->setSender(getIndex());
    }
    //send SV to all processes
    for (int j = 0; j < numSubmodules; j++) {
        if (j == numSubmodules - 1)
            scheduleAt(simTime(), msg);
        else
            send(msg->dup(), "gate$o", j);
    }

}

void process::initialize() {


    network = getModuleByPath("Topology");
    numSubmodules = network->getSubmoduleVectorSize("process");

    std::uniform_int_distribution<int> distribution(0, numSubmodules-1);
    logFile = "results/process_" + std::to_string(getIndex()) + ".log";
    std::ofstream file(logFile, std::ios::app);


    PV = createAndInitArray(numSubmodules);
    SV = createAndInitArray(numSubmodules);
    RV = createAndInitArray(numSubmodules);
    Ev = createAndInit2dArray(numSubmodules);
    result = new int[numSubmodules]();
    log = network->par("logs");
    infectionRng.seed(network->par("seed"));
    indexCorrectProcess = distribution(infectionRng);
    createNewArrayInfectable(numSubmodules,&infectableProcesses,indexCorrectProcess);
    numInfected = network->par("numInfected");
    // generate a random int number between 0 and 1
    myNum = intuniform(0,1);
    EV << getIndex() << " chooses value " << myNum;
    if (log){
        file << "ROUND " << s << " -------------------------------------" << std::endl;
        file << "I choose " << myNum << std::endl;
        file << "PROPOSE PHASE -------------------------------------------------" << std::endl;
    }



    infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log);

    propose(myNum,numSubmodules,infected);
    file.close();

}
void process::handleMessage(cMessage *msg) {

    std::ofstream file(logFile, std::ios::app);
    if (dynamic_cast<ProposalMsg*>(msg)) {

        ProposalMsg *my_msg = check_and_cast<ProposalMsg*>(msg);
        if (!infected)
            PV[my_msg->getSender()] = my_msg->getValue();
        else
            PV[my_msg->getSender()] = intuniform(-1,1);
        receivedPV++;
        WATCH(receivedPV);
        printVector(getIndex(), PV, "PV", numSubmodules);
        if (receivedPV == numSubmodules) {
            if (log){
                file << "received proposals = ";
                logVector(file,PV,numSubmodules);
            }
            EV << "Ho ricevuto tutte le proposte \n";
            decided = -1;
            myNum = canDecide(numSubmodules - 2 * numInfected, PV);
            if (log){
                file << "my value now is " << myNum << std::endl;
            }
            receivedPV = 0;
            if (log){
                file << "COLLECT PHASE -------------------------------------------------" << std::endl;
            }
            infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log);
            initArray(SV,numSubmodules);
            collect(myNum,numSubmodules,infected);
        }
    }

    else if (dynamic_cast<CollectMsg*>(msg)) {
        CollectMsg *cMsg = check_and_cast<CollectMsg*>(msg);
        if (!infected)
            SV[cMsg->getSender()] = cMsg->getValue();
        else
            SV[cMsg->getSender()] = intuniform(-1,1);
        receivedSV++;
        printVector(getIndex(), SV, "SV", numSubmodules);
        if (receivedSV == numSubmodules) {
            if (log) {
                file << "received values from all processes = ";
                logVector(file, SV, numSubmodules);
            }
            decided = -1;
            receivedSV = 0;
            if (log){
                file << "DECIDE PHASE -------------------------------------------------" << std::endl;
            }
            infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log);
            for (int i = 0; i < numSubmodules; i++){
                initArray(Ev[i],numSubmodules);
            }
            decide(myNum, SV,numSubmodules,infected);
        }

    }

    else if (dynamic_cast<Decide*>(msg)) {

        Decide *myMsg = check_and_cast<Decide*>(msg);
        receivedEv++;
        WATCH(receivedEv);
        int sender = myMsg->getSender();
        for (int i = 0; i < numSubmodules; i++) {
            int element;
            if (!infected)
                element = myMsg->getData(i);
            else
                element = intuniform(-1,1);
            Ev[sender][i] = element;
        }
        if (receivedEv == numSubmodules) {
            if (log) {
                file << "received arrays from all processes:" << std::endl;
                for (int i = 0; i < numSubmodules; i++) {
                    logVector(file, Ev[i], numSubmodules);
                }
            }

            EV << "ho ricevuto tutto" << endl;
            printVector(getIndex(), Ev[0], "Ev[0]", numSubmodules);
            printVector(getIndex(), Ev[1], "Ev[1]", numSubmodules);
            printVector(getIndex(), Ev[2], "Ev[2]", numSubmodules);
            printVector(getIndex(), Ev[3], "Ev[3]", numSubmodules);

            initArray(RV,numSubmodules);

            for (int j = 0; j < numSubmodules; j++) {
                int count0 = 0;
                int count1 = 0;
                for (int k = 0; k < numSubmodules; k++) {
                    if (Ev[k][j] == 0)
                        count0++;
                    else if (Ev[k][j] == 1)
                        count1++;

                }
                if (count0 > 2 * numInfected)
                    RV[j] = 0;
                else if (count1 > 2 * numInfected)
                    RV[j] = 1;

            }
            if (log){
                file << "the king is " << s << std::endl;
                file << "the resulting array from the table = ";
                logVector(file, RV, numSubmodules);
            }
            int w = canDecide(3 * numInfected + 1, RV);
            if (w != -1) {
                myNum = w;
            } else {
                int c = s;
                int b = canDecide(2 * numInfected + 1, Ev[c]);
                if (b != -1) {
                    myNum = b;
                } else {
                    myNum = 0;
                }
            }
            if (log){
                file << "My value after decide is " << myNum << std::endl;
            }
            decided = -1;
            receivedEv = 0;
            EV << "FINITI 3 ROUND - S= : " << s << endl;
            s++;
            if (log) {
                file << "ROUND " << s << " -------------------------------------" << std::endl;
            }

            infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log);
            if (s == numSubmodules) {
                decided = myNum;
                if (log) {
                    file << "MAINTAIN PHASE -------------------------------------------------" << std::endl;
                }
                infectableProcesses.push_back(indexCorrectProcess);
                maintain(decided,numSubmodules,infected );
            } else {
                if (log){
                    file << "PROPOSE PHASE -------------------------------------------------" << std::endl;
                }
                propose(myNum,numSubmodules,infected);
            }
        }
    }

    else if (dynamic_cast<Maintain*>(msg)) {
        Maintain *my_msg = check_and_cast<Maintain*>(msg);

        if (!infected)
            result[my_msg->getSender()] = my_msg->getFinalDecision();
        else
            result[my_msg->getSender()] = intuniform(-1,1);
        receivedMaintain++;
        WATCH(receivedMaintain);
        if (receivedMaintain == numSubmodules) {

            EV << "Ho ricevuto tutte le proposte \n";
            decided = canDecide(numSubmodules - 2 * numInfected, result);
            if (log) {
                file << "received values from all processes = ";
                logVector(file, result, numSubmodules);
                file << "decided value = " << decided << std::endl;
            }
            receivedMaintain = 0;
            if (maintainRounds < (network->par("maxMaintainRounds").intValue())){

                if (log) {
                    file << "MAINTAIN PHASE -------------------------------------------------" << std::endl;
                }
                infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log);
                maintain(decided,numSubmodules,infected);
                maintainRounds++;
            }
            else{
                if (log){
                    file << "finished with value = " << decided;
                }
                EV << "finished with value " << decided;
                delete[] RV;
                delete[] SV;
                delete[] PV;
                delete[] result;
            }
        }
    }
    file.close();
}

