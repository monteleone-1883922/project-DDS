#include <omnetpp.h>
#ifndef MYARRAYMESSAGE_H_
#define MYARRAYMESSAGE_H_

class MyArrayMessage : public omnetpp::cMessage {
protected:
    int* myArray;
    int arraySize;

public:
    MyArrayMessage(const char* name = nullptr, int kind = 0) : cMessage(name, kind) {
        myArray = nullptr;
        arraySize = 0;
    }

    void setArray(int* array, int size) {
        myArray = new int[size];
        arraySize = size;
        for (int i = 0; i < size; i++) {
            myArray[i] = array[i];
        }
    }

    int* getArray() const {
        return myArray;
    }

    int getArraySize() const {
        return arraySize;
    }
};
#endif /* MYARRAYMESSAGE_H_ */
