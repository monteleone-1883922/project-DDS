#include <random>
#include "process.h"
#include <omnetpp.h>
#include "sendValue_m.h"
#include "kingSend_m.h"
#include "sendList_m.h"
#include "infected_m.h"

using namespace omnetpp;

class process: public cSimpleModule {

private:
    int *MV; //qui vengono salvati tutti i valori ricevuti dagli altri moduli
    int recivedMV = 0;
    int numSubmodules; //rappresenta il numero di moduli nella rete
    int value; //rappresenta il numero randomico scelto dal modulo
    bool infected = 0; //indica se il modulo è o no infetto
    int **echo;
    int recivedEcho = 0;
    int round = 0;
    int c;
    int k;
    int numInfected = 2;
    int *processInfected;
    int *infectableProcess;
    int indexCorrectProcess = 2;
    int *setProcess;
    int *ValuesFromKing;
    bool cured= true;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void printVector(int id, int *vector, std::string nomeVettore,
            int numSubmodules);
    virtual void sendValue(int v);
    virtual void reconstruction(int *array);
    virtual void kingSend(int v);
    virtual bool updateInfectedStatus();
    virtual void sendNewInfected(int *inf);
    virtual void updateInfected(int *arrayUpdated, int num);
    virtual int* createNewArrayInfectable(int arraySize, int *init, int pos);
    virtual int countOne(int *array);
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
        std::uniform_int_distribution<int> distributionInf(0, num - 1);
        int indexInf = distributionInf(generator);
        //  EV <<"updateInfected::::indice che verrà infettato: " <<indexInf <<endl;
        processInfected[numSubmodules - num - 1] = arrayUpdated[indexInf];
        updateInfected(createNewArrayInfectable(num, arrayUpdated, indexInf),
                num - 1);
    } else {
        infected = updateInfectedStatus();
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
void process::sendValue(int v) {

    for (int i = 0; i < 4; i++) {
        SendValue *msg = new SendValue("propose");
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
void process::kingSend(int v) {

    for (int i = 0; i < 4; i++) {
        KingSend *msg = new KingSend("collect");
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
void process::reconstruction(int *array) {
    SendList *mesg = new SendList();
    mesg->setDataArraySize((numSubmodules));
    std::random_device rd;
    std::mt19937 generator(rd());
    // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
    std::uniform_int_distribution<int> distribution(0, 1);
    for (int i = 0; i < numSubmodules; i++) {
        if (!infected)
            mesg->setData(i, array[i]); // Popola l'array con i dati desiderati
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
int process::countOne(int *array) {
    int counter = 0;
    for (int i = 0; i < numSubmodules; i++) {
        if (array[i] == 1)
            counter++;
    }
    return counter;
}
void process::initialize() {

    // si salva il numero di moduli presenti e inizializza il vettore in modo che abbia una cella per ogni modulo
    cModule *topLevelModule = getModuleByPath("Topology");
    numSubmodules = topLevelModule->getSubmoduleVectorSize("process");
    //inizializza le liste
    MV = new int[numSubmodules]();
    for (int i = 0; i < numSubmodules; i++) {
        MV[i] = -1;
    }
    // Inizializza il generatore di numeri casuali con un seed
    std::random_device rd;
    std::mt19937 generator(rd());

    // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
    std::uniform_int_distribution<int> distribution(0, 1);

    // Genera un numero casuale tra 0 e 1
    value = distribution(generator);

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
        EV << getIndex() << " sceglie num " << value << "\n";
        sendValue(value);
    }
    if (dynamic_cast<SendValue*>(msg)) {

        SendValue *my_msg = check_and_cast<SendValue*>(msg);

        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sender
        MV[my_msg->getSender()] = my_msg->getIntMsg();
        recivedMV++; //per ora non viene utilizzato
        printVector(getIndex(), MV, "MV", numSubmodules);
        if (recivedMV == numSubmodules) {
            EV << "Ho ricevuto tutte le proposte \n";
            c = countOne(MV);
            if (c >= numSubmodules / 2)
                value = 1;
            else {
                value = 0;
            }
            recivedMV = 0;
            reconstruction(MV);
        }
    }

    if (dynamic_cast<KingSend*>(msg)) {
        KingSend *cMsg = check_and_cast<KingSend*>(msg);
        int vKing = cMsg->getIntMsg();
        if ((vKing == 0 || vKing == 1) && c < numSubmodules - 2 * numInfected)
            value = vKing;
       round++;
    }

    if (dynamic_cast<SendList*>(msg)) {

        SendList *myMsg = check_and_cast<SendList*>(msg);
        recivedEcho++; //aggiorno il contatore di messaggi ricevuti
        int sender = myMsg->getSender(); //mi salvo il sender
        //salvo l'array ricevuto nella riga del sender
        for (int i = 0; i < numSubmodules; i++) {
            int element = myMsg->getData(i);
            echo[sender][i] = element;
        }
        if (recivedEcho == numSubmodules) { //se ho ricevuto gli array da tutti
            EV << "ho ricevuto tutto" << endl;
            printVector(getIndex(), echo[0], "Ev[0]", numSubmodules);
            printVector(getIndex(), echo[1], "Ev[1]", numSubmodules);
            printVector(getIndex(), echo[2], "Ev[2]", numSubmodules);
            printVector(getIndex(), echo[3], "Ev[3]", numSubmodules);
            //setto tutti i campi di RV a null

            if (cured) { ///DA IMPLEMENTARE
                if (round % 2 == 0) {
                    //PROCEDURE RECONSTRUCT(r)
                    recivedEcho = 0;
                    //controllo se tra le colonne ci sono abbastanza valori uguali
                    for (int j = 0; j < numSubmodules; j++) {
                        int count0 = 0;
                        int count1 = 0;
                        for (int k = 0; k < numSubmodules; k++) {
                            if (echo[k][j] == 0)
                                count0++;
                            if (echo[k][j] == 1)
                                count1++;

                        }
                        if (count0 > 2 * numInfected)
                            ValuesFromKing[j] = 0;
                        if (count1 > 2 * numInfected)
                            ValuesFromKing[j] = 1;

                    }
                    c = countOne(ValuesFromKing);
                    if (c >= numSubmodules / 2)
                        value = 1;
                    else {
                        value = 0;
                    }
                }
            }

            k = (round % numSubmodules) + 1; // (* k is the phase's king *)
            if (k == getIndex())
                kingSend(value);
        }
    }

}

