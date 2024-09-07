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
#include <nlohmann/json.hpp>

using json = nlohmann::json;


using namespace omnetpp;

class process: public cSimpleModule {

private:
    int *PV; //store the Proposed Value
    int *check_PV;
    int numSubmodules; //number of processes in the network
    int myNum; //the number chosen
    std::vector<bool> infected; //shows if the process is infected
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
    SimTime roundStartTime;
    json results;
    json rounds = json::array();
    json roundJson;
    std::string runName;
    bool all_decided = 0;
    int infecctionSpeed;
    int microRounds = 0;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void printVector(int id, int *vector, std::string nomeVettore, int numSubmodules);
    virtual int canDecide( int minNumber, int *list);
    virtual void propose(int v, int numProcesses, bool infected, int microRounds);
    virtual void collect(int v, int numProcesses, bool infected, int microRounds);
    virtual void decide(int v, int *SV,int numSubmodules, bool infected, int microRounds);
    virtual void maintain(int decision, int numProcesses, bool infected, int microRounds);
    virtual void createNewArrayInfectable(int numProcesses, std::vector<int> *processes, int correctProcess);
    virtual bool generateInfections(std::vector<int> *processes, int numInfected, int process,
            std::mt19937* rng,std::ofstream& file,bool log);
    virtual int* createAndInitArray(int size);
    virtual void initArray(int* array, int size);
    virtual int** createAndInit2dArray(int size);
    virtual void logVector(std::ofstream& out, int *vector, int numSubmodules);
    virtual bool decisionTaken(std::vector<int> *processes, int numInfected,int *array, int decidedVal);
    virtual void finish() override;

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
        if (i != correctProcess)
            processes->push_back(i);
    }
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

        count0 += list[i] == 0;
        count1 += list[i] == 1;

        if (count1 >= minNumber || count0 >= minNumber){
            return count1 >= minNumber;
        }

    }

    return -1;

}

bool process::decisionTaken(std::vector<int> *processes, int numInfected,int *array, int decidedVal){
    int counter = 0;
    for (int i = 0; i < processes->size(); i++) {
        if (array[i] == decidedVal){
            int add = 1;
            for (int j = 0; j < numInfected; j++) {
                if ((*processes)[j] == i){
                    add = 0;
                    break;
                }
            }
            counter += add;
        }
    }
    return counter >= numSubmodules - 2 * numInfected;
}


void process::maintain(int decision,int numProcesses, bool infected, int microRounds) {
    for (int i = 0; i < numProcesses; i++) {
        Maintain *msg = new Maintain("Maintain");
        msg->setMicroRounds(microRounds);
        if (!infected) {
            msg->setFinalDecision(decision);
        }
        else {
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


void process::propose(int v,int numProcesses, bool infected, int microRounds) {

    for (int i = 0; i < numProcesses; i++) {
        ProposalMsg *msg = new ProposalMsg("propose");
        msg->setMicroRounds(microRounds);
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


void process::collect(int v,int numProcesses, bool infected, int microRounds) {

    for (int i = 0; i < numProcesses; i++) {
        CollectMsg *msg = new CollectMsg("collect");
        msg->setMicroRounds(microRounds);
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
void process::decide(int value, int *Sv,int numSubmodules, bool infected, int microRounds) {
    Decide *msg = new Decide("decide");
    msg->setMicroRounds(microRounds);
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

    runName = "seed-" + network->par("seed").str() + "_infected-" + network->par("numInfected").str() + "_speed-" + network->par("infectionSpeed").str() + "_numProc-" + std::to_string(numSubmodules);

    check_PV = createAndInitArray(numSubmodules);
    PV = createAndInitArray(numSubmodules);
    SV = createAndInitArray(numSubmodules);
    RV = createAndInitArray(numSubmodules);
    Ev = createAndInit2dArray(numSubmodules);
    result = new int[numSubmodules]();
    infecctionSpeed = network->par("infectionSpeed").intValue();
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
    if (indexCorrectProcess == getIndex()) {
        results["correct_process"] = indexCorrectProcess;
        results["num_infected"] = numInfected;
        results["num_processes"] = numSubmodules;

    }



    infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
    roundJson["infected"] = infected;
    roundStartTime = simTime();
    propose(myNum,numSubmodules,infected[microRounds], microRounds);
    file.close();

}
void process::handleMessage(cMessage *msg) {

    std::ofstream file(logFile, std::ios::app);
    if (dynamic_cast<ProposalMsg*>(msg)) {

        ProposalMsg *my_msg = check_and_cast<ProposalMsg*>(msg);
        if (my_msg->getMicroRounds() != microRounds && microRounds % infecctionSpeed == 0) {
            infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
        }
        if (!infected[my_msg->getMicroRounds()])
            PV[my_msg->getSender()] = my_msg->getValue();
        else
            check_PV[my_msg->getSender()] = my_msg->getValue();
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
            if (infected[my_msg->getMicroRounds()]){
                int myNumCheck = canDecide(numSubmodules - 2 * numInfected, check_PV);
                if (myNumCheck != -1) {
                    all_decided = decisionTaken(&infectableProcesses,numInfected,check_PV,myNumCheck);
                    if (all_decided){
                        results["rounds_to_decide"] = s;
                        EV <<  getIndex() << " Decision taken at round " << s << " decided value " << myNum << " ---------------------------------------"<< std::endl;
                        if (log){
                            file <<  getIndex() << " Decision taken at round " << s << " decided value " << myNum << " ---------------------------------------"<< std::endl;
                            file << results.dump(4) << std::endl;
                        }
                    }
                }
            }
            myNum = canDecide(numSubmodules - 2 * numInfected, PV);
            if (myNum != -1) {
                all_decided = decisionTaken(&infectableProcesses,numInfected,PV,myNum);
                if (all_decided && !infected[my_msg->getMicroRounds()]){
                    results["rounds_to_decide"] = s;
                    EV <<  getIndex() << " Decision taken at round " << s << " decided value " << myNum << " ---------------------------------------"<< std::endl;
                    if (log){
                        file <<  getIndex() << " Decision taken at round " << s << " decided value " << myNum << " ---------------------------------------"<< std::endl;
                        file << results.dump(4) << std::endl;
                    }
                }
            }
            if (log){
                file << "my value now is " << myNum << std::endl;
            }
            receivedPV = 0;
            if (log){
                file << "COLLECT PHASE -------------------------------------------------" << std::endl;
            }
            microRounds++;
            if (microRounds % infecctionSpeed == 0 && infected.size() <= microRounds) {
                infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
            }
            roundJson["proposal_round_time"] = (simTime() - roundStartTime).dbl();
            roundStartTime = simTime();
            initArray(SV,numSubmodules);
            collect(myNum,numSubmodules,infected[microRounds], microRounds);
        }
    }

    else if (dynamic_cast<CollectMsg*>(msg)) {
        CollectMsg *cMsg = check_and_cast<CollectMsg*>(msg);
        if (cMsg->getMicroRounds() != microRounds && microRounds % infecctionSpeed == 0) {
            infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
        }
        if (!infected[cMsg->getMicroRounds()])
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
            microRounds++;
            if (microRounds % infecctionSpeed == 0 && infected.size() <= microRounds) {
                infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
            }
            for (int i = 0; i < numSubmodules; i++){
                initArray(Ev[i],numSubmodules);
            }
            roundJson["collect_round_time"] = (simTime() - roundStartTime).dbl();
            roundStartTime = simTime();
            decide(myNum, SV,numSubmodules,infected[microRounds], microRounds);
        }

    }

    else if (dynamic_cast<Decide*>(msg)) {

        Decide *myMsg = check_and_cast<Decide*>(msg);
        receivedEv++;
        WATCH(receivedEv);
        int sender = myMsg->getSender();
        if (myMsg->getMicroRounds() != microRounds && microRounds % infecctionSpeed == 0) {
            infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
        }
        for (int i = 0; i < numSubmodules; i++) {
            int element;
            if (!infected[myMsg->getMicroRounds()])
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
                    count0 += Ev[k][j] == 0;
                    count1 += Ev[k][j] == 1;

                }
                if (count0 > 2 * numInfected || count1 > 2 * numInfected) {
                    RV[j] = count1 > 2 * numInfected;
                }

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

            microRounds++;
            if (microRounds % infecctionSpeed == 0 && infected.size() <= microRounds) {
                infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
            }
            if (s == numSubmodules || all_decided) {
                decided = myNum;
                if (log) {
                    file << "MAINTAIN PHASE -------------------------------------------------" << std::endl;
                }
                infectableProcesses.push_back(indexCorrectProcess);
                roundStartTime = simTime();
                roundJson["decide_round_time"] = (simTime() - roundStartTime).dbl();
                maintain(decided,numSubmodules,infected[microRounds], microRounds );
            } else {
                if (log){
                    file << "PROPOSE PHASE -------------------------------------------------" << std::endl;
                }
                roundJson["decide_round_time"] = (simTime() - roundStartTime).dbl();
                roundStartTime = simTime();
                propose(myNum,numSubmodules,infected[microRounds], microRounds);
            }
        }
    }

    else if (dynamic_cast<Maintain*>(msg)) {
        Maintain *my_msg = check_and_cast<Maintain*>(msg);

        if (my_msg->getMicroRounds() != microRounds && microRounds % infecctionSpeed == 0) {
            infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
        }
        if (!infected[my_msg->getMicroRounds()])
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
                microRounds++;
                if (microRounds % infecctionSpeed == 0 && infected.size() <= microRounds) {
                    infected.push_back(generateInfections(&infectableProcesses,numInfected, getIndex(),&infectionRng,file,log));
                }
                roundJson["maintain_round_time"] = (simTime() - roundStartTime).dbl();
                roundStartTime = simTime();
                maintain(decided,numSubmodules,infected[microRounds], microRounds);
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


void process::finish()
{
    results["rounds"] = rounds;
    if (indexCorrectProcess == getIndex()) {

      //  std::ofstream file(logFile, std::ios::app);
     //   EV <<"size == " << results.size() << std::endl;
   //     file << "adadadadad correct is " << indexCorrectProcess << std::endl;
    //       if (network->par("seed").intValue() == 60){
     //          file << results.size() << std::endl;
   //           file << "my index is " << getIndex() << std::endl;
       //       file << "entratoooooo" << std::endl;
    //          file << results.dump(4) << std::endl;

        //  }
       //    file.close();

       }
    std::ostringstream filename;
    filename << "results/results_" << getIndex() << ".json";
    // Scrittura del JSON su file
    std::ofstream file(filename.str());
    if (file.is_open()) {
        file << results.dump(4);  // dump(4) per formattare con 4 spazi di indentazione
        file.close();
        EV << "process " << getIndex() << " File JSON salvato con successo!" << endl;
    } else {
        std::ofstream file(logFile, std::ios::app);
        file << "Errore nell'apertura del file!" << endl;
        file.close();
    }

    /*bool allFilesExist = true;
    std::vector<int> nodeIds;
    for (int i = 0; i < numSubmodules; i++) {
        nodeIds.push_back(i);
    }
    for (int id : nodeIds) {
        std::ostringstream otherFilename;
        otherFilename << "results/results_" << id << ".json";
        struct stat buffer;
        if (stat(otherFilename.str().c_str(), &buffer) != 0) {
            allFilesExist = false;
            EV << "process " << getIndex() << ": il file " << otherFilename.str() << " non esiste" << endl;
            break;
        }
    }

    if (allFilesExist) {
        EV << "process " << getIndex() << ": tutti i file sono stati creati correttamente" << endl;
        std::ostringstream command;
        command << "python3 scripts/merge_results.py " << runName << " " << round;
        int result = system(command.str().c_str());

        // Controlla il codice di ritorno
        if (result == 0) {
            EV << "Lo script Python è stato eseguito correttamente." << endl;
             for (int id : nodeIds) {
                std::ostringstream otherFilename;
                otherFilename << "results/results_" << id << ".json";
                if (std::remove(otherFilename.str().c_str()) == 0) {
                    std::cout << "File " << otherFilename.str() << " rimosso con successo." << std::endl;
                } else {
                    std::perror(("Errore nella rimozione del file " + otherFilename.str()).c_str());
                }
            }
             if (network->par("seed").intValue() == network->par("numExperiments").intValue()){
                 command.str("");
                 command.clear();
                 command << "python3 scripts/analyze_multiple_runs.py " << "merged";
                 int result = system(command.str().c_str());
             }

        }
        if (result != 0){
            EV << "Errore nell'esecuzione dello script " << command.str() << ", codice di ritorno: " << result << endl;
        }
    }*/
}


