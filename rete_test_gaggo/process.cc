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
    int numInfected = 2;
    int *processInfected;
    int *infectableProcess;
    int indexCorrectProcess = 2;
    int *setProcess;
    int *result;

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
    virtual int* createNewArrayInfectable(int arraySize, int *init, int pos);
};

Define_Module(process);
//INPUT: Array di partenza e un elemento in posizion "POS" da eliminare
//OUTPUT Un array di dimesione n-1 senza quel elemento
int* process::createNewArrayInfectable(int arraySize, int *init, int pos) {
    //printVector(0, init, "createNewArrayInfectable - RICEVUTO:", arraySize);
    int *newArray = new int[arraySize - 1];
    for (int i = 0; i < arraySize - 1; i++) {
        if (i < pos)
            newArray[i] = init[i];
        else {
            newArray[i] = init[i + 1];
        }
    }
    //printVector(0, newArray, "createNewArrayInfectable- OUTPUT:", arraySize-1);
    return newArray;
}
//OUTPUT: Restituisce se il processo è stato infettato
bool process::updateInfectedStatus() {
    int indice = getIndex();
    for (int i = 0; i < numInfected; i++) {
        if (indice == processInfected[i])
            return true;
    }
    return false;
}
//OUTPUT: crea un array con gli array infetti
void process::updateInfected(int *arrayUpdated, int num) {
    std::random_device rd;
    std::mt19937 generator(rd());
    //EV <<"updateInfected:::::numero di processi infettabili" <<num <<endl;
    if (num >= numSubmodules - numInfected) {
        std::uniform_int_distribution<int> distributionInf(0, num - 1); //NON MODIFICARE
        int indexInf = distributionInf(generator);
        // EV <<"updateInfected::::indice che verrà infettato: " <<indexInf <<endl;
        processInfected[numSubmodules - num - 1] = arrayUpdated[indexInf];
        updateInfected(createNewArrayInfectable(num, arrayUpdated, indexInf),
                num - 1);
    } else {
        //infected = updateInfectedStatus();
        sendNewInfected(processInfected);
        //printVector(getIndex(), processInfected, "updateInfected::::Nuova Lista processi infetti", numInfected);
    }
}
//Manda a tutti i processi la lista dei processi infetti
void process::sendNewInfected(int *inf) {
    Infected *mesg = new Infected("Infected List");
    mesg->setProcessArraySize(numInfected);
    for (int i = 0; i < numInfected; i++) {
        mesg->setProcess(i, inf[i]);  // Popola l'array con i dati desiderati
    }
    for (int j = 0; j < numSubmodules; j++) {
        if (j == numSubmodules - 1)
            scheduleAt(simTime(), mesg);
        else
            send(mesg->dup(), "gate$o", j);
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

    //inizializza le liste
    PV = new int[numSubmodules]();
    for (int i = 0; i < numSubmodules; i++) {
        PV[i] = -1;
    }
    SV = new int[numSubmodules]();
    for (int i = 0; i < numSubmodules; i++) {
        SV[i] = -1;
    }
    Ev = new int*[numSubmodules];  // Array di puntatori a array
    for (int i = 0; i < numSubmodules; i++) {
        Ev[i] = new int[numSubmodules](); // Allocazione di ogni array interno e inizializzazione a zero
        for (int j = 0; j < numSubmodules; j++) {
            Ev[i][j] = -1;
        }
    }

    // Inizializza il generatore di numeri casuali con un seed
    std::random_device rd;
    std::mt19937 generator(rd());

    // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
    std::uniform_int_distribution<int> distribution(0, 1);

    // Genera un numero casuale tra 0 e 1
    myNum = distribution(generator);

    //INIZIALIZZO GLI INFETTI
    processInfected = new int[numInfected];

    if (getIndex() == 0) {
        setProcess = new int[numSubmodules]();
        for (int i = 0; i < numSubmodules; i++) {
            setProcess[i] = i;
        }
        // printVector(0, setProcess, "INIT:::Set dei processi:", numSubmodules);
        infectableProcess = createNewArrayInfectable(numSubmodules, setProcess,
                indexCorrectProcess);

        //printVector(0, infectableProcess, "INIT:::Set dei processi infettabili:", numSubmodules-1);
        updateInfected(infectableProcess, numSubmodules - 1);
        printVector(getIndex(), setProcess, "INIT:::Set dei processi: ",
                numSubmodules);
        printVector(getIndex(), infectableProcess,
                "INIT:::Set dei processi infettabili:", numSubmodules - 1);
        printVector(getIndex(), processInfected,
                "INIT:::Set dei processi infettati:", numInfected);
    }
    /*
     //TEST PER VEDERE LE POSIZIONI
     if (getIndex() == 0) {
     EV << ":Sono 0 " << endl;
     int *arrayToSend = new int[4] { 0, 0, 0, 0 };
     decide(3, arrayToSend);
     }
     if (getIndex() == 1) {
     int *arrayToSend1 = new int[4] { 1, 1, 1, 1 };
     EV << ":Sono 1 " << endl;
     decide(3, arrayToSend1);
     }
     if (getIndex() == 2) {
     int *arrayToSend2 = new int[4] { 2, 2, 2, 2 };
     EV << ":Sono 2 " << endl;
     decide(3, arrayToSend2);
     }
     if (getIndex() == 3) {
     int *arrayToSend3 = new int[4] { 3, 3, 3, 3 };
     EV << ":Sono 3 " << endl;
     decide(3, arrayToSend3);
     }
     */
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

        if (recivedPV == numSubmodules) {
            EV << "Ho ricevuto tutte le proposte \n";
            printVector(getIndex(), PV, "PV", numSubmodules);
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

                if (getIndex() == 0) {
                    updateInfected(infectableProcess, numSubmodules - 1);
                    printVector(getIndex(), setProcess,
                            "DEC:::Set dei processi: ", numSubmodules);
                    printVector(getIndex(), infectableProcess,
                            "DEC:::Set dei processi infettabili:",
                            numSubmodules - 1);
                    printVector(getIndex(), processInfected,
                            "DEC:::Set dei processi infettati:", numInfected);
                }
            } else {
                decided = myNum;
                //MAINTAINING ROUND
                maintain(decided);
            }
        }

    }
    if (dynamic_cast<Maintain*>(msg)) {
        Maintain *my_msg = check_and_cast<Maintain*>(msg);

        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sender
        result[my_msg->getSender()] = my_msg->getFinalDecision();
        recivedMaintain++; //per ora non viene utilizzato
        if (recivedMaintain == numSubmodules) {
            if (getIndex() == 0) {
                updateInfected(infectableProcess, numSubmodules - 1);
                printVector(getIndex(), setProcess, "MAINT:::Set dei processi: ",
                        numSubmodules);
                printVector(getIndex(), infectableProcess,
                        "MAINT:::Set dei processi infettabili:",
                        numSubmodules - 1);
                printVector(getIndex(), processInfected,
                        "MAINT:::Set dei processi infettati:", numInfected);
            }
            EV << "Ho ricevuto tutte le proposte di maintain \n";
            decided = canIdecide(getIndex(), numSubmodules - 2 * numInfected,
                    result);
            recivedMaintain = 0;
            maintain(decided);
        }
    }
}

