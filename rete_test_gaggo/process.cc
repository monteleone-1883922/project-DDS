#include <random>
#include "process.h"
#include <omnetpp.h>
#include "int_m.h"
#include "vector_m.h"
#include "proposal_m.h"
#include "collect_m.h"
#include "prova_m.h"

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
    int *Ev;
    int recivedPV=0;
    int recivedSV=0;
    int *RV;
    int c;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void printVector(int id, int *vector, std::string nomeVettore,
            int numSubmodules);
    virtual int canIdecide(int id);
    virtual void propose(int v);
    virtual void collect(int v);
    virtual bool fullArray(int *array);
    virtual void decide(int v,int *SV);
    virtual void setArray(int* array, int size);

};

Define_Module(process);


void process::printVector(int id, int *vector, std::string nomeVettore,
        int numSubmodules) {
    // stampa i valori di un vettore
    std::string vec = "";
    for (int i = 0; i < numSubmodules; i++) {
        vec += std::to_string(vector[i]) + " | ";
    }
    EV << id << " vettore " + nomeVettore + " = " << vec;
}
//la funzione controlla se almeno un numero è stato mandato da un numero suff. di processi corretti
int process::canIdecide(int id) {

    int count;
    for (int i = 0; i < numSubmodules; i++) {
        count = 0;
        int value = PV[i];
        if (PV[i] != -1) {
            for (int j = 0; j < numSubmodules; j++) {
                if (PV[j] == PV[i])
                    count++;
                if (count >= numSubmodules - 2 * infected) {
                    EV << id << ": Ho deciso il valore " << PV[j] << "!!!"
                              << endl;
                    return PV[j];
                }

            }
        }
    }

    EV << id << " Non ho deciso " << endl;
    return -1;

}
void process::propose(int v) {

    for (int i = 0; i < 4; i++) {
        ProposalMsg *msg = new ProposalMsg("starter");
        msg->setIntMsg(v);
        msg->setSender(getIndex());
        if (i == 3) {
            scheduleAt(simTime(), msg);
        } else {
            send(msg, "gate$o", i);
        }
    }
}
void process::collect(int v) {

    for (int i = 0; i < 4; i++) {
        CollectMsg *msg = new CollectMsg("starter");
        msg->setIntMsg(v);
        msg->setSender(getIndex());
        if (i == 3) {
            scheduleAt(simTime(), msg);
        } else {
            send(msg, "gate$o", i);
        }
    }
}
void process::decide(int value, int *SV){}
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
    PV = new int[numSubmodules]();
    for (int i = 0; i < numSubmodules; i++) {
        PV[i] = -1;
    }
    SV = new int[numSubmodules]();
    for (int i = 0; i < numSubmodules; i++) {
        SV[i] = -1;
    }
    /* SCOMMENTARE
    // Inizializza il generatore di numeri casuali con un seed
    std::random_device rd;
    std::mt19937 generator(rd());

    // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
    std::uniform_int_distribution<int> distribution(0, 1);

    // Genera un numero casuale tra 0 e 1
    myNum = distribution(generator);

    //stampa il numero scelto dal modulo per verificare alla fine che si trovi nella posizione giusta di ciascun vettore
    EV << getIndex() << " sceglie num " << myNum << "\n";

    //crea il nuovo messaggio da spedire e setta valore da spedire e sender (potrebbe essere implementato anche in una funzione a parte)
    IntegerMsg *msg = new IntegerMsg("starter");
    msg->setIntMsg(myNum);
    msg->setSender(getIndex());
*/
    //spedisce il messaggio a tutti
    //propose(myNum);
    if(getIndex()==0){
    int* arrayToSend = new int[5] {1, 2, 3, 4, 5};
    Prova* mesg = new Prova();
    int arraySize=4;
    mesg->setDataArraySize((arraySize));
   for (int i = 0; i < arraySize; i++) {
        mesg->setData(i, i);  // Popola l'array con i dati desiderati
    }
    send(mesg, "gate$o", 0);
    send(mesg, "gate$o", 2);
    send(mesg, "gate$o", 1);}


}
void process::handleMessage(cMessage *msg) {

    cGate *arrivalGate = msg->getArrivalGate();

    //puo essere usato per identificare il tipo di messaggio
    std::string mes = msg->getName();

    //fa un cast safe al tipo indicato
    /* SCOMMENTARE
    if (dynamic_cast<ProposalMsg*>(msg)) {

        ProposalMsg *my_msg = check_and_cast<ProposalMsg*>(msg);

        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sender
        PV[my_msg->getSender()] = my_msg->getIntMsg();
        recivedPV++; //per ora non viene utilizzato
        printVector(getIndex(), PV, "PV", numSubmodules);
        if (fullArray(PV)) {
            EV << "Ho ricevuto tutte le proposte \n";
            decided = -1;
            myNum = canIdecide(getIndex());
            collect(myNum);
        }
    }

    /*
    for (int i = 0; i < numSubmodules; i++) {
        WATCH(PV[i]);
    }*/
/* SCOMMENTARE
    if (dynamic_cast<CollectMsg*>(msg)) {
        CollectMsg *cMsg = check_and_cast<CollectMsg *>(msg);
        SV[cMsg->getSender()] = cMsg->getIntMsg();
        recivedSV++;
        printVector(getIndex(), SV, "SV", numSubmodules);
        if(recivedSV==numSubmodules){
            decided=-1;
            decide(myNum,SV);
        }

    }*/

        if (dynamic_cast<Prova*>(msg)) {

            Prova* myMsg = check_and_cast<Prova*>(msg);
            int arraySize = myMsg->getDataArraySize();
            int risultato[arraySize];
            for (int i = 0; i < arraySize; i++) {
              int element = myMsg->getData(i);
              risultato[i]=element;
              printVector(getIndex(), risultato, "risultato", numSubmodules);
        }

        }


        }



