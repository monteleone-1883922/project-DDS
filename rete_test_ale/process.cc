#include <random>
#include "process.h"
#include <omnetpp.h>
#include "int_m.h"
#include "vector_m.h"
#include "proposal_m.h"
#include "collect_m.h"
#include "decide_m.h"
#include "maintain_m.h"
#include "infected_m.h"

using namespace omnetpp;

class process: public cSimpleModule {

private:
    int *PV; //qui vengono salvati tutti i valori ricevuti dagli altri moduli
    int num = 0; // rappresenta il numero di messaggi inviati
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
    int *setProcess;
    std::mt19937 infectionRng;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void printVector(int id, int *vector, std::string nomeVettore,
            int numSubmodules);
    virtual int canIdecide(int id, int minNumber, int *list);
    virtual void propose(int v);
    virtual void collect(int v);
    virtual bool fullArray(int *array);
    virtual void decide(int v, int *SV);
    virtual void maintain(int decision);
    virtual bool updateInfectedStatus();
    virtual void sendNewInfected(int *inf);
    virtual void updateInfected(int *arrayUpdated, int num);
    virtual void createNewArrayInfectable(int numProcesses, std::vector<int> processes, int correctProcess);
    virtual bool generateInfections(std::vector<int> processes, int numInfected, int process, std::mt19937* rng);
    virtual int* createAndInitArray(int size);
    virtual int** createAndInit2dArray(int size);

};

Define_Module(process);



bool process::generateInfections(std::vector<int> processes, int numInfected, int process, std::mt19937* rng){


    std::shuffle(processes.begin(), processes.end(), *rng);
    for( int i = 0; i<numInfected; i++){
        if( processes[i] == process){
            return 1;
        }
    }
    return 0;
}


int* process::createAndInitArray(int size){
    int* array = new int[size]();
    for (int i = 0; i < size; i++) {
        array[i] = -1;
    }
    return array;
}

int** process::createAndInit2dArray(int size){
    int** array = new int[size]();
    for (int i = 0; i < size; i++) {
        array[i] = createAndInitArray(size);
    }
    return array;
}


//INPUT: Array di partenza e un elemento in posizion "POS" da eliminare
//OUTPUT Un array di dimesione n-1 senza quel elemento
void process::createNewArrayInfectable(int numProcesses, std::vector<int> processes, int correctProcess) {
    //printVector(0, init, "createNewArrayInfectable - RICEVUTO:", arraySize);

    for (int i = 0 ; i < numProcesses; i++){
        if (i < correctProcess)
            processes.push_back(i+1);
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
int process::canIdecide(int id, int minNumber, int *list) {

    int count;
    for (int i = 0; i < numSubmodules; i++) {
        count = 0;
        if (list[i] != -1) {
            for (int j = 0; j < numSubmodules; j++) {
                if (list[j] == list[i])
                    count++;
                if (count >= minNumber) {
                    EV << id << ": Ho deciso il valore " << list[j] << "!!!"
                              << endl;
                    return list[j];
                }

            }
        }
    }

    EV << id << " Non ho deciso " << endl;
    return -1;

}
void process::maintain(int decision) {
    for (int i = 0; i < 4; i++) {
        Maintain *msg = new Maintain("Maintain");
        msg->setFinalDecision(decision);
        msg->setSender(getIndex());
        if (i == numSubmodules - 1) {
            scheduleAt(simTime(), msg);
        } else {
            send(msg, "gate$o", i);
        }
    }
}
void process::propose(int v) {

    for (int i = 0; i < 4; i++) {
        ProposalMsg *msg = new ProposalMsg("propose");
        if (!infected) {
            msg->setIntMsg(v);
        } else {
            std::random_device rd;
            std::mt19937 generator(rd());
            // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
            std::uniform_int_distribution<int> distribution(0, 1);
            // Genera un numero casuale tra 0 e 1
            int w = distribution(generator);
            msg->setIntMsg(w);
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
void process::collect(int v) {

    for (int i = 0; i < 4; i++) {
        CollectMsg *msg = new CollectMsg("collect");
        if (!infected) {
            msg->setIntMsg(v);
        } else {
            std::random_device rd;
            std::mt19937 generator(rd());
            // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
            std::uniform_int_distribution<int> distribution(0, 1);
            // Genera un numero casuale tra 0 e 1
            int w = distribution(generator);
            msg->setIntMsg(w);
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
void process::decide(int value, int *Sv) {
    Decide *mesg = new Decide();
    mesg->setDataArraySize((numSubmodules));
    std::random_device rd;
    std::mt19937 generator(rd());
    // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
    std::uniform_int_distribution<int> distribution(0, 1);
    for (int i = 0; i < numSubmodules; i++) {
        if (!infected)
            mesg->setData(i, Sv[i]); // Popola l'array con i dati desiderati
        else {
            mesg->setData(i, distribution(generator));
        }

        mesg->setSender(getIndex());
    }
    for (int j = 0; j < numSubmodules; j++) {
        if (j == numSubmodules - 1)
            scheduleAt(simTime(), mesg);
        else
            send(mesg->dup(), "gate$o", j);
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
    cModule *topLevelModule = getModuleByPath("Topology");
    numSubmodules = topLevelModule->getSubmoduleVectorSize("process");

    std::uniform_int_distribution<int> distribution(0, numSubmodules-1);



    //inizializza le liste
    PV = createAndInitArray(numSubmodules);
    SV = createAndInitArray(numSubmodules);
    Ev = createAndInit2dArray(numSubmodules);
    infectionRng = infectionRng(par("seed"));
    indexCorrectProcess = distribution(infectionRng);
    createNewArrayInfectable(numSubmodules,infectableProcesses,indexCorrectProcess);
    numInfected = par("numInfected");
    // Genera un numero casuale tra 0 e 1
    myNum = uniform(0, 1);

    //INIZIALIZZO GLI INFETTI
    infected = generateInfections(infectableProcesses,numInfected, getIndex(),&infectionRng);
    EV << "ok";

}
void process::handleMessage(cMessage *msg) {



    cGate *arrivalGate = msg->getArrivalGate();

    //puo essere usato per identificare il tipo di messaggio
    std::string mes = msg->getName();

    //fa un cast safe al tipo indicato
    // SCOMMENTARE
    if (dynamic_cast<Infected*>(msg)) {
        Infected *my_msg = check_and_cast<Infected*>(msg);
        for (int i = 0; i < numInfected; i++)
            processInfected[i] = my_msg->getProcess(i);
        infected = updateInfectedStatus();
        printVector(getIndex(), processInfected,
                "Lista processi infetti ricevuta", numInfected);
        EV << getIndex() << " sceglie num " << myNum << "\n";
        propose(myNum);
    }
    if (dynamic_cast<ProposalMsg*>(msg)) {

        ProposalMsg *my_msg = check_and_cast<ProposalMsg*>(msg);

        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sender
        PV[my_msg->getSender()] = my_msg->getIntMsg();
        recivedPV++; //per ora non viene utilizzato
        printVector(getIndex(), PV, "PV", numSubmodules);
        if (recivedPV == numSubmodules) {
            EV << "Ho ricevuto tutte le proposte \n";
            decided = -1;
            myNum = canIdecide(getIndex(), numSubmodules - 2 * numInfected, PV);
            recivedPV = 0;
            collect(myNum);
        }
    }

    /*
     for (int i = 0; i < numSubmodules; i++) {
     WATCH(PV[i]);
     }*/
    //SCOMMENTARE
    if (dynamic_cast<CollectMsg*>(msg)) {
        CollectMsg *cMsg = check_and_cast<CollectMsg*>(msg);
        SV[cMsg->getSender()] = cMsg->getIntMsg();
        recivedSV++;
        printVector(getIndex(), SV, "SV", numSubmodules);
        if (recivedSV == numSubmodules) {
            decided = -1;
            recivedSV = 0;
            decide(myNum, SV);
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
            RV = new int[numSubmodules]();
            for (int i = 0; i < numSubmodules; i++) {
                RV[i] = -1;
            }
            //controllo se tra le colonne ci sono abbastanza valori uguali
            for (int j = 0; j < numSubmodules; j++) {
                int count0 = 0;
                int count1 = 0;
                for (int k = 0; k < numSubmodules; k++) {
                    if (Ev[k][j] == 0)
                        count0++;
                    if (Ev[k][j] == 1)
                        count1++;

                }
                if (count0 > 2 * numInfected)
                    RV[j] = 0;
                if (count1 > 2 * numInfected)
                    RV[j] = 1;

            }
            int w = canIdecide(getIndex(), 2 * numInfected, RV);
            if (w != -1) {
                myNum = w;
            } else {
                int c = s; //C è uguale S DA IMPLEMENTARE
                int *prova = new int[numSubmodules]();
                prova = Ev[c];
                int b = canIdecide(getIndex(), 2 * numInfected, prova);
                if (b != -1) {
                    myNum = b;
                } else {
                    myNum = 0;
                }
            }
            decided = -1;
            EV << "FINITI 3 ROUND - S= : " << s << endl;
            s++;
            if (s < numSubmodules) {
                decided = myNum;
                if (getIndex() == 0) {
                    updateInfected(infectableProcess, numSubmodules - 1);
                    printVector(getIndex(), setProcess,
                            "INIT:::Set dei processi: ", numSubmodules);
                    printVector(getIndex(), infectableProcess,
                            "INIT:::Set dei processi infettabili:",
                            numSubmodules - 1);
                    printVector(getIndex(), processInfected,
                            "INIT:::Set dei processi infettati:", numInfected);
                }
            } else {
                //MAINTAINING ROUND
                maintain(decided);
            }
        }

    }
    if (dynamic_cast<Maintain*>(msg)) {
        Maintain *my_msg = check_and_cast<Maintain*>(msg);
        int *result;
        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sender
        result[my_msg->getSender()] = my_msg->getFinalDecision();
        recivedMaintain++; //per ora non viene utilizzato
        if (recivedMaintain == numSubmodules) {
            EV << "Ho ricevuto tutte le proposte \n";
            decided = canIdecide(getIndex(), numSubmodules - 2 * infected,
                    result);
            recivedMaintain = 0;
            maintain(decided);
        }
    }
}

