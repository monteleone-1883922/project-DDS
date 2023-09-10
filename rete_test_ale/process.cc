#include <random>
#include "process.h"
#include <omnetpp.h>
#include "int_m.h"
#include "vector_m.h"

using namespace omnetpp;

class process : public cSimpleModule {

private:
    int **variabile2; //qui vengono salvati tutti i valori ricevuti dagli altri moduli
    int num = 0; // rappresenta il numero di messaggi inviati
    int numSubmodules; //rappresenta il numero di moduli nella rete
    int *myNum; //rappresenta il numero randomico scelto dal modulo
    bool infected = 0; //indica se il modulo è o no infetto


protected:
    virtual void initialize() ;
    virtual void handleMessage(cMessage *msg);
    virtual void printVector(int id,int** vector, std::string nomeVettore);
    virtual void setArray(VectorMsg msg, int* array);

};

Define_Module(process);


void process::setArray(VectorMsg msg, int* array){
    for (int i =0;i<sizeof(myNum); i++){
        msg->setVector(i,array[i]);
    }
}


void process::printVector(int id,int** vector, std::string nomeVettore){
    // stampa i valori di un vettore
    std::string vec = "";
    for(int i = 0; i<sizeof(vector); i++){
        for (int j = 0; j < sizeof(vector[0]); j++){
            vec += std::to_string(vector[i][j]) + " | ";
        }

            }
    EV << id << " vettore " + nomeVettore + " = " << vec;
}

void process::initialize() {
        // si salva il numero di moduli presenti e inizializza il vettore in modo che abbia una cella per ogni modulo
        cModule* topLevelModule = getModuleByPath("Topology");
        numSubmodules = topLevelModule->getSubmoduleVectorSize("process");
        variabile2 = new int*[numSubmodules]();
        for (int i=0; i<sizeof(variabile2); i++){
            variabile2[i] = new int[numSubmodules]();
        }
        myNum = new int[numSubmodules]();
        // Inizializza il generatore di numeri casuali con un seed
        std::random_device rd;
        std::mt19937 generator(rd());

        // Definisci la distribuzione per generare numeri tra 0 e 1 inclusi
        std::uniform_int_distribution<int> distribution(0, 10);

        // Genera un numero casuale tra 0 e 1
        int myNum1 = distribution(generator);
        for (int i =0; i<sizeof(myNum); i++){
            myNum[i] = myNum1;
        }

        //stampa il numero scelto dal modulo per verificare alla fine che si trovi nella posizione giusta di ciascun vettore
        EV << getIndex() << " sceglie num " << myNum1 << "\n";

        //crea il nuovo messaggio da spedire e setta valore da spedire e sender (potrebbe essere implementato anche in una funzione a parte)
        VectorMsg* msg = new VectorMsg("starter");
        msg->setVectorArraySize( sizeof(myNum));
        for (int i =0;i<sizeof(myNum); i++){
            msg->setVector(i,myNum[i]);
        }
        msg->setSender(getIndex());

        //spedisce il messaggio sul primo gate
        send(msg,"gate$o", 0);


}
void process::handleMessage(cMessage* msg) {
        cGate* arrivalGate = msg->getArrivalGate();

        //puo essere usato per identificare il tipo di messaggio
        std::string mes = msg->getName();

        //fa un cast safe al tipo indicato
        VectorMsg *my_msg = check_and_cast<VectorMsg *>(msg);
        // inserisce il numero ricevuto nell array del modulo ricevente nella posizione dedicata al modulo sende
        for (int i =0; i< my_msg->getVectorArraySize(); i++){
                variabile2[my_msg->getSender()][i]=my_msg->getVector(i);
        }

        for(int i = 0; i<numSubmodules; i++){
            WATCH(variabile2[i]);
        }
        //crea il nuovo messaggio da spedire e setta valore da spedire e sender (potrebbe essere implementato anche in una funzione a parte)
        VectorMsg* msg2 = new VectorMsg("mid");
        msg2->setVectorArraySize( sizeof(myNum));
                for (int i =0;i<sizeof(myNum); i++){
                    msg2->setVector(i,myNum[i]);
                }
        msg2->setSender(getIndex());

        // finché il numero di messaggi inviati non supera quello dei gates disponibili per inviare i messaggi invia un messagggio al prossimo gate
        if (num < gateSize("gate")-1){

            send(msg2, "gate$o",++num);
        }
        else{
            // se ha finito i gates disponibili su cui inviare messaggi si invia un messaggio da solo
            if (num < gateSize("gate")){
                scheduleAt(simTime(),msg2);
                num++;
            }
            else{
                // una volta inviati tutti i messaggi stampa il vettore variabile 2 in modo da modtrare il risultato dello scambio di messaggi
                printVector(getIndex(),variabile2,"variabile2");
            }
        }

    }



    /*
     if(arrivalGate == gate("in3")){
     std::string mes = msg->getFullName();
     send(msg, "ou0");
     }*/

