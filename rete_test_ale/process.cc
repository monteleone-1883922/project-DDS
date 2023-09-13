#include <random>
#include "process.h"
#include <omnetpp.h>
#include "int_m.h"
#include "vector_m.h"
#include "proposal_m.h"
#include "collect_m.h"
#include "decide_m.h"
#include "maintain_m.h"
#include <algorithm>


using namespace omnetpp;

class process: public cSimpleModule {

private:
    int *PV; //qui vengono salvati tutti i valori ricevuti dagli altri moduli
    int numSubmodules; //rappresenta il numero di moduli nella rete
    int myNum; //rappresenta il numero randomico scelto dal modulo
    bool infected = 0; //indica se il modulo è o no infetto
    int decided; //valore finale
    int *SV;
    int **Ev;
    int recivedPV = 0;
    int recivedSV = 0;
    int recivedEv = 0;
    int recivedMaintain = 0;
    int *RV;
    int s = 0;
    int numInfected;
    std::vector<int> infectableProcesses;
    int indexCorrectProcess;
    int *result;
    std::mt19937 infectionRng;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void printVector(int id, int *vector, std::string nomeVettore,
            int numSubmodules);
    virtual int canDecide( int minNumber, int *list);
    virtual void propose(int v, int numProcesses, bool infected);
    virtual void collect(int v, int numProcesses, bool infected);
    virtual bool fullArray(int *array);
    virtual void decide(int v, int *SV,int numSubmodules, bool infected);
    virtual void maintain(int decision, int numProcesses, bool infected);
    virtual void createNewArrayInfectable(int numProcesses, std::vector<int> *processes, int correctProcess);
    virtual bool generateInfections(std::vector<int> *processes, int numInfected, int process, std::mt19937* rng);
    virtual int* createAndInitArray(int size);
    virtual void initArray(int* array);
    virtual int** createAndInit2dArray(int size);

};

Define_Module(process);



bool process::generateInfections(std::vector<int> *processes, int numInfected, int process, std::mt19937* rng){


    std::shuffle(processes->begin(), processes->end(), *rng);
    for( int i = 0; i<numInfected; i++){
        if( (*processes)[i] == process){
            return 1;
        }
    }
    return 0;
}


int* process::createAndInitArray(int size){
    int* array = new int[size]();
    initArray(array);
    return array;
}

void process::initArray(int* array){
    for (int i = 0; i < sizeof(array); i++) {
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


//INPUT: Array di partenza e un elemento in posizion "POS" da eliminare
//OUTPUT Un array di dimesione n-1 senza quel elemento
void process::createNewArrayInfectable(int numProcesses, std::vector<int> *processes, int correctProcess) {
    //printVector(0, init, "createNewArrayInfectable - RICEVUTO:", arraySize);

    for (int i = 0 ; i < numProcesses; i++){
        if (i < correctProcess)
            processes->push_back(i);
        else
            processes->push_back(i+1);
    }
}


void process::printVector(int id, int *vector, std::string nomeVettore,
        int numSubmodules) {
    // stampa i valori di un vettore
    std::string vec = "";
    for (int i = 0; i < numSubmodules; i++) {
        vec += std::to_string(vector[i]) + " | ";
    }
    EV << id << " vettore " + nomeVettore + " = " << vec << endl;
}
//la funzione controlla se almeno un numero è stato mandato da un numero suff. di processi corretti
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
    for (int i = 0; i < 4; i++) {
        Maintain *msg = new Maintain("Maintain");
        if (!infected) {
            msg->setFinalDecision(decision);
        }
        else{
            int w = uniform(0,1);
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

            // Genera un numero casuale tra 0 e 1
            int w = uniform(0,1);
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

            // Genera un numero casuale tra 0 e 1
            int w = uniform(0, 1);
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
            msg->setData(i, Sv[i]); // Popola l'array con i dati desiderati
        else {
            msg->setData(i, uniform(0,1));
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
bool process::fullArray(int *array) {

    for (int i = 0; i < numSubmodules; i++) {
        if (array[i] == -1)
            return false;
    }
    return true;
}
void process::initialize() {

    // si salva il numero di moduli presenti e inizializza il vettore in modo che abbia una cella per ogni modulo
    cModule *network = getModuleByPath("Topology");
    numSubmodules = network->getSubmoduleVectorSize("process");

    std::uniform_int_distribution<int> distribution(0, numSubmodules-1);



    //inizializza le liste
    PV = createAndInitArray(numSubmodules);
    SV = createAndInitArray(numSubmodules);
    RV = createAndInitArray(numSubmodules);
    Ev = createAndInit2dArray(numSubmodules);
    result = new int[numSubmodules]();
    infectionRng.seed(network->par("seed"));
    indexCorrectProcess = distribution(infectionRng);
    createNewArrayInfectable(numSubmodules,&infectableProcesses,indexCorrectProcess);
    numInfected = network->par("numInfected");
    // Genera un numero casuale tra 0 e 1
    myNum = uniform(0, 1);

    //INIZIALIZZO GLI INFETTI
    infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng);
    propose(myNum,numSubmodules,infected);

}
void process::handleMessage(cMessage *msg) {



    //puo essere usato per identificare il tipo di messaggio


    //fa un cast safe al tipo indicato
    // SCOMMENTARE

    if (dynamic_cast<ProposalMsg*>(msg)) {

        ProposalMsg *my_msg = check_and_cast<ProposalMsg*>(msg);

        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sender
        PV[my_msg->getSender()] = my_msg->getValue();
        recivedPV++; //per ora non viene utilizzato
        printVector(getIndex(), PV, "PV", numSubmodules);
        if (recivedPV == numSubmodules) {
            EV << "Ho ricevuto tutte le proposte \n";
            decided = -1;
            myNum = canDecide(numSubmodules - 2 * numInfected, PV);
            recivedPV = 0;
            infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng);
            collect(myNum,numSubmodules,infected);
        }
    }

    /*
     for (int i = 0; i < numSubmodules; i++) {
     WATCH(PV[i]);
     }*/
    //SCOMMENTARE
    if (dynamic_cast<CollectMsg*>(msg)) {
        CollectMsg *cMsg = check_and_cast<CollectMsg*>(msg);
        SV[cMsg->getSender()] = cMsg->getValue();
        recivedSV++;
        printVector(getIndex(), SV, "SV", numSubmodules);
        if (recivedSV == numSubmodules) {
            decided = -1;
            recivedSV = 0;
            infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng);
            decide(myNum, SV,numSubmodules,infected);
        }

    }

    if (dynamic_cast<Decide*>(msg)) {

        Decide *myMsg = check_and_cast<Decide*>(msg);
        recivedEv++; //aggiorno il contatore di messaggi ricevuti
        int sender = myMsg->getSender(); //mi salvo il sender
        //salvo l'array ricevuto nella riga del sender
        for (int i = 0; i < numSubmodules; i++) {
            int element = myMsg->getData(i);
            Ev[sender][i] = element;
        }
        if (recivedEv == numSubmodules) { //se ho ricevuto gli array da tutti
            EV << "ho ricevuto tutto" << endl;
            printVector(getIndex(), Ev[0], "Ev[0]", numSubmodules);
            printVector(getIndex(), Ev[1], "Ev[1]", numSubmodules);
            printVector(getIndex(), Ev[2], "Ev[2]", numSubmodules);
            printVector(getIndex(), Ev[3], "Ev[3]", numSubmodules);
            //setto tutti i campi di RV a null
            initArray(RV);
            //controllo se tra le colonne ci sono abbastanza valori uguali
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
            int w = canDecide(3 * numInfected + 1, RV);
            if (w != -1) {
                myNum = w;
            } else {
                int c = s; //C è uguale S DA IMPLEMENTARE
                int b = canDecide(2 * numInfected + 1, Ev[c]);
                if (b != -1) {
                    myNum = b;
                } else {
                    myNum = 0;
                }
            }
            decided = -1;
            EV << "FINITI 3 ROUND - S= : " << s << endl;
            s++;
            infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng);
            if (s == numSubmodules) {
                decided = myNum;
                maintain(decided,numSubmodules,infected );
            } else {
                propose(myNum,numSubmodules,infected);
            }
        }
    }

    if (dynamic_cast<Maintain*>(msg)) {
        Maintain *my_msg = check_and_cast<Maintain*>(msg);

        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sender
        result[my_msg->getSender()] = my_msg->getFinalDecision();
        recivedMaintain++; //per ora non viene utilizzato
        if (recivedMaintain == numSubmodules) {
            EV << "Ho ricevuto tutte le proposte \n";
            decided = canDecide(numSubmodules - 2 * infected, result);
            if (decided == -1)
                throw std::runtime_error("Maintain is not working");
            recivedMaintain = 0;
            maintain(decided,numSubmodules,infected);
        }
    }
}

