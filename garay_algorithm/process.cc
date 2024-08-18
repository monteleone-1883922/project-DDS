#include <random>
#include "process.h"
#include <omnetpp.h>
#include "sendValue_m.h"
#include "kingSend_m.h"
#include "sendList_m.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace omnetpp;

class process: public cSimpleModule {

private:
    int *MV; //store the received values in a vector
    int recivedMV = 0; //number of values received
    int numSubmodules; //number of processes in the network
    int value; //the value that a process propose
    bool infected = 0; //bool that represent if a process is infected
    int **echo; //vector containing the MV received
    int recivedEcho = 0; //number of echo received
    int round = 0; //number of round
    int c;
    int k; //index of the king
    std::string logFile;
    bool log = 1;
    int numInfected;
    int indexCorrectProcess;
    std::vector<int> infectableProcesses;
    bool cured = false;
    std::mt19937 infectionRng;
    cModule *network;
    int *fix;
    simsignal_t decisionSignal;
    simsignal_t timeRoundSignal;
    SimTime roundStartTime;
    bool emittedDecision = false;
    json results;
    json rounds = json::array();
    json roundJson;
    int infecctionSpeed;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void printVector(int id, int *vector, std::string nomeVettore,
            int numSubmodules);
    virtual void sendValue(int v, int numProcesses, bool infected);
    virtual void sendMV(int *array,int numProcesses, bool infected);
    virtual void kingSend(int v,int numProcesses, bool infected);
    virtual bool updateInfectedStatus(int * array);
    virtual void createNewArrayInfectable(int numProcesses, std::vector<int> *processes, int correctProcess);
    virtual int countOne(int *array);
    virtual bool generateInfections(std::vector<int> *processes, int numInfected, int process,
            std::mt19937* rng, std::ofstream& file, bool log);
    virtual int* createAndInitArray(int size);
    virtual void initArray(int *array, int size);
    virtual int** createAndInit2dArray(int size);
    virtual void logVector(std::ofstream& out, int *vector, int numSubmodules);
    virtual void finish() override;
};

Define_Module(process);


bool process::generateInfections(std::vector<int> *processes, int numInfected, int process,
        std::mt19937* rng, std::ofstream& file, bool log){
    std::shuffle(processes->begin(), processes->end(), *rng);
    if (log) {
        std::string vec = "";
        for (int i = 0; i < numSubmodules - 1; i++) {
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
        if (i != correctProcess)
            processes->push_back(i);
    }
}


//OUTPUT: it returns if the process is infected
bool process::updateInfectedStatus(int * array) {
    int indice = getIndex();
    for (int i = 0; i < numInfected; i++) {
        if (indice == array[i])
            return true;
    }
    return false;
}


//print vector values
void process::logVector(std::ofstream& out, int *vector, int numSubmodules) {
    std::string vec = "";
    for (int i = 0; i < numSubmodules; i++) {
        vec += std::to_string(vector[i]) + " | ";
    }
    out << vec << std::endl;
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


void process::sendValue(int v,int numProcesses, bool infected) {

    for (int i = 0; i < numProcesses; i++) {
        SendValue *msg = new SendValue("propose");
        if (!infected) {
            msg->setValue(v);
        } else {
            // generate a random int number between 0 and 1
            int w = intuniform(-1, 1);
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


void process::kingSend(int v,int numProcesses, bool infected) {

    for (int i = 0; i < numProcesses; i++) {
        KingSend *msg = new KingSend("collect");
        if (!infected) {
            msg->setValue(v);
        } else {

            // generate a random int number between 0 and 1
            int w = intuniform(-1,1);
            msg->setValue(w);
            EV << "mando sul gate" << i << "il valore " << w << std::endl;
        }
        msg->setSender(getIndex());
        if (i == numSubmodules - 1) {
            scheduleAt(simTime(), msg);
        } else {
            send(msg, "gate$o", i);
        }
    }
}


void process::sendMV(int *array, int numProcesses, bool infected) {
    SendList *msg = new SendList();
    msg->setDataArraySize((numSubmodules));

    for (int i = 0; i < numSubmodules; i++) {
        if (!infected)
            msg->setData(i, array[i]);
        else {
            msg->setData(i, intuniform(-1,1));
        }

        msg->setSender(getIndex());
    }
    for (int j = 0; j < numSubmodules; j++) {
        if (j == numSubmodules - 1)
            scheduleAt(simTime(), msg);
        else
            send(msg->dup(), "gate$o", j);
    }

}


int process::countOne(int *array) {
    int counter = 0;
    for (int i = 0; i < numSubmodules; i++) {
        counter += array[i] == 1;

    }
    return counter;
}


void process::initialize() {


    network = getModuleByPath("Topology");
    numSubmodules = network->getSubmoduleVectorSize("process");
    decisionSignal = registerSignal("decision");
    timeRoundSignal = registerSignal("roundTime");
    infecctionSpeed = network->par("infectionSpeed");


    logFile = "results/process_" + std::to_string(getIndex()) + ".log";

    std::ofstream file(logFile, std::ios::app);
    std::uniform_int_distribution<int> distribution(0, numSubmodules-1);

    MV = createAndInitArray(numSubmodules);
    echo = createAndInit2dArray(numSubmodules);
    fix = createAndInitArray(numSubmodules);
    infectionRng.seed(network->par("seed"));
    indexCorrectProcess = distribution(infectionRng);
    createNewArrayInfectable(numSubmodules,&infectableProcesses,indexCorrectProcess);
    numInfected = network->par("numInfected");

    if (indexCorrectProcess == getIndex()) {
        results["correct_process"] = indexCorrectProcess;
        results["num_infected"] = numInfected;
        results["num_processes"] = numSubmodules;
    }


    // Genera un numero casuale tra 0 e 1
    value = intuniform(0, 1);

    if (log) {
            file << "ROUND " << round << " -------------------------------------" << std::endl;
            file << "I choose " << value << std::endl;

    }

    infected = generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log);
    roundJson["infected"] = infected;


    if (log){
        file << "SEND VALUE PHASE -------------------------------------------------" << std::endl;
    }

    roundStartTime = simTime();

    sendValue(value,numSubmodules,infected);
    file.close();
}

void process::handleMessage(cMessage *msg) {


    std::ofstream file(logFile, std::ios::app);
    if (dynamic_cast<SendValue*>(msg)) {

        SendValue *my_msg = check_and_cast<SendValue*>(msg);

        if (!infected) {
            MV[my_msg->getSender()] = my_msg->getValue();
        } else {
            MV[my_msg->getSender()] = intuniform(-1, 1);
        }

        recivedMV++;
        printVector(getIndex(), MV, "MV", numSubmodules);
        if (recivedMV == numSubmodules) {

            EV << "Ho ricevuto tutte le proposte \n";
            c = countOne(MV);
            value = c >= numSubmodules / 2;
            if (indexCorrectProcess == getIndex() && (c >= numSubmodules - 2 * numInfected || c <= 2 * numInfected ) && !emittedDecision) {
                emit(decisionSignal, round);
                results["rounds_to_decide"] = round;
                emittedDecision = true;
                EV << "Decision taken at round " << round << std::endl;
            }
            if (log) {
                file << "received values = ";
                logVector(file, MV, numSubmodules);
                file << "My value now is " << value << std::endl;
                file << "SEND MV PHASE -------------------------------------------------" << std::endl;
            }
            recivedMV = 0;

            sendMV(MV, numSubmodules, infected);

        }
    }
    if (dynamic_cast<SendList*>(msg)) {

        SendList *myMsg = check_and_cast<SendList*>(msg);
        recivedEcho++;
        int sender = myMsg->getSender();

        for (int i = 0; i < numSubmodules; i++) {
            int element;
            if (!infected) {
                element = myMsg->getData(i);
            } else {
                element = intuniform(-1, 1);
            }
            echo[sender][i] = element;
        }
        if (recivedEcho == numSubmodules) {
            EV << "ho ricevuto tutto" << endl;
            printVector(getIndex(), echo[0], "Ev[0]", numSubmodules);
            printVector(getIndex(), echo[1], "Ev[1]", numSubmodules);
            printVector(getIndex(), echo[2], "Ev[2]", numSubmodules);
            printVector(getIndex(), echo[3], "Ev[3]", numSubmodules);

            if (log) {
                file << "received arrays from all processes:" << std::endl;
                for (int i = 0; i < numSubmodules; i++) {
                    logVector(file, echo[i], numSubmodules);
                }
            }

            recivedEcho = 0;
            if (cured) {

                //PROCEDURE RECONSTRUCT(r)


                initArray(fix, numSubmodules);
                int count0 = 0;
                int count1 = 0;
                for (int j = 0; j < numSubmodules; j++) {
                    count0 = 0;
                    count1 = 0;
                    for (int k = 0; k < numSubmodules; k++) {
                        if (echo[k][j] == 0)
                            count0++;
                        else if (echo[k][j] == 1)
                            count1++;

                    }
                    if (count0 > numSubmodules - 2 * numInfected)
                        fix[j] = 0;
                    else if (count1 > numSubmodules - 2 * numInfected)
                        fix[j] = 1;

                }
                c = countOne(fix);
                value = c >= numSubmodules / 2;
                if (log) {
                    file << "I'm being cured" << std::endl;
                    file << "the resulting array from the table = ";
                    logVector(file, fix, numSubmodules);
                    file << "My value is now = " << value << std::endl;
                }

            }

            k = (round + 1) % numSubmodules ; // (* k is the phase's king *)
            if (log) {
                file << "king of round is " << k << std::endl;
                file << "KING SEND PHASE -------------------------------------------------" << std::endl;
            }

            if (k == getIndex())
                kingSend(value, numSubmodules, infected);
        }
    }

    if (dynamic_cast<KingSend*>(msg)) {
        KingSend *cMsg = check_and_cast<KingSend*>(msg);
        int vKing;
        if (!infected) {
            vKing = cMsg->getValue();
        } else {
            vKing = intuniform(-1, 1);
        }
        if ((vKing == 0 || vKing == 1) && c < numSubmodules - 2 * numInfected && c > 2 * numInfected)
            value = vKing;
        if (log){
            file << "king sent = " << vKing << std::endl;
            file << "My value now is " << value << std::endl;
        }

        round++;
        EV <<  "Begin round " << round << " -------------------------------------" << std::endl;
        bool oldStatus = infected;


        if (round < (network->par("maxMaintainRounds").intValue())) {
            if (log){
                file << "ROUND " << round << " -------------------------------------" << std::endl;
            }
            if (round % infecctionSpeed == 0) {
                infected = generateInfections(&infectableProcesses, numInfected,
                        getIndex(), &infectionRng, file, log);
            }
            if (log){
                file << "SEND VALUE PHASE -------------------------------------------------" << std::endl;
            }
            cured = oldStatus && !infected;
            sendValue(value, numSubmodules, infected);
        } else {
            if (log){
                file << "finished with value = " << value << std::endl;
            }
            EV  << "finished with value = " << value;

        }
        roundJson["round_time"] = (simTime() - roundStartTime).dbl();
        rounds.push_back(roundJson);
        roundJson.clear();
        roundJson["infected"] = infected;
        roundJson["cured"] = cured;
        emit(timeRoundSignal, simTime() - roundStartTime);
        roundStartTime = simTime();


    }
    file.close();

}

void process::finish()
{
    results["rounds"] = rounds;
    std::ostringstream filename;
    filename << "results_" << getIndex() << ".json";
    // Scrittura del JSON su file
    std::ofstream file(filename.str());
    if (file.is_open()) {
        file << results.dump(4);  // dump(4) per formattare con 4 spazi di indentazione
        file.close();
        EV << "File JSON salvato con successo!" << endl;
    } else {
        EV << "Errore nell'apertura del file!" << endl;
    }

    bool allFilesExist = true;
    std::vector<int> nodeIds;
    for (int i = 0; i <= numSubmodules; i++) {
        nodeIds.push_back(i);
    }
    for (int id : nodeIds) {
        std::ostringstream otherFilename;
        otherFilename << "results_" << id << ".json";
        struct stat buffer;
        if (stat(otherFilename.str().c_str(), &buffer) != 0) {
            allFilesExist = false;
            break;
        }
    }

    if (allFilesExist) {
        int result = system("python3 scripts/merge_results.py");

        // Controlla il codice di ritorno
        if (result == 0) {
            EV << "Lo script Python è stato eseguito correttamente." << endl;
        } else {
            EV << "Errore nell'esecuzione dello script Python, codice di ritorno: " << result << endl;
        }
    }
}


