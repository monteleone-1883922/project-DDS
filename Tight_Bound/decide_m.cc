//
// Generated file, do not edit! Created by opp_msgtool 6.0 from decide.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "decide_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(Decide)

Decide::Decide(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

Decide::Decide(const Decide& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

Decide::~Decide()
{
    delete [] this->data;
}

Decide& Decide::operator=(const Decide& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void Decide::copy(const Decide& other)
{
    delete [] this->data;
    this->data = (other.data_arraysize==0) ? nullptr : new int[other.data_arraysize];
    data_arraysize = other.data_arraysize;
    for (size_t i = 0; i < data_arraysize; i++) {
        this->data[i] = other.data[i];
    }
    this->value = other.value;
    this->sender = other.sender;
}

void Decide::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    b->pack(data_arraysize);
    doParsimArrayPacking(b,this->data,data_arraysize);
    doParsimPacking(b,this->value);
    doParsimPacking(b,this->sender);
}

void Decide::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    delete [] this->data;
    b->unpack(data_arraysize);
    if (data_arraysize == 0) {
        this->data = nullptr;
    } else {
        this->data = new int[data_arraysize];
        doParsimArrayUnpacking(b,this->data,data_arraysize);
    }
    doParsimUnpacking(b,this->value);
    doParsimUnpacking(b,this->sender);
}

size_t Decide::getDataArraySize() const
{
    return data_arraysize;
}

int Decide::getData(size_t k) const
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    return this->data[k];
}

void Decide::setDataArraySize(size_t newSize)
{
    int *data2 = (newSize==0) ? nullptr : new int[newSize];
    size_t minSize = data_arraysize < newSize ? data_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        data2[i] = this->data[i];
    for (size_t i = minSize; i < newSize; i++)
        data2[i] = 0;
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

void Decide::setData(size_t k, int data)
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    this->data[k] = data;
}

void Decide::insertData(size_t k, int data)
{
    if (k > data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    size_t newSize = data_arraysize + 1;
    int *data2 = new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        data2[i] = this->data[i];
    data2[k] = data;
    for (i = k + 1; i < newSize; i++)
        data2[i] = this->data[i-1];
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

void Decide::appendData(int data)
{
    insertData(data_arraysize, data);
}

void Decide::eraseData(size_t k)
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    size_t newSize = data_arraysize - 1;
    int *data2 = (newSize == 0) ? nullptr : new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        data2[i] = this->data[i];
    for (i = k; i < newSize; i++)
        data2[i] = this->data[i+1];
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

int Decide::getValue() const
{
    return this->value;
}

void Decide::setValue(int value)
{
    this->value = value;
}

int Decide::getSender() const
{
    return this->sender;
}

void Decide::setSender(int sender)
{
    this->sender = sender;
}

class DecideDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_data,
        FIELD_value,
        FIELD_sender,
    };
  public:
    DecideDescriptor();
    virtual ~DecideDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(DecideDescriptor)

DecideDescriptor::DecideDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Decide)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

DecideDescriptor::~DecideDescriptor()
{
    delete[] propertyNames;
}

bool DecideDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Decide *>(obj)!=nullptr;
}

const char **DecideDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *DecideDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int DecideDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int DecideDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_data
        FD_ISEDITABLE,    // FIELD_value
        FD_ISEDITABLE,    // FIELD_sender
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *DecideDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "data",
        "value",
        "sender",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int DecideDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "data") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "value") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "sender") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *DecideDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_data
        "int",    // FIELD_value
        "int",    // FIELD_sender
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **DecideDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *DecideDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int DecideDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        case FIELD_data: return pp->getDataArraySize();
        default: return 0;
    }
}

void DecideDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        case FIELD_data: pp->setDataArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Decide'", field);
    }
}

const char *DecideDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string DecideDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        case FIELD_data: return long2string(pp->getData(i));
        case FIELD_value: return long2string(pp->getValue());
        case FIELD_sender: return long2string(pp->getSender());
        default: return "";
    }
}

void DecideDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        case FIELD_data: pp->setData(i,string2long(value)); break;
        case FIELD_value: pp->setValue(string2long(value)); break;
        case FIELD_sender: pp->setSender(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Decide'", field);
    }
}

omnetpp::cValue DecideDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        case FIELD_data: return pp->getData(i);
        case FIELD_value: return pp->getValue();
        case FIELD_sender: return pp->getSender();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Decide' as cValue -- field index out of range?", field);
    }
}

void DecideDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        case FIELD_data: pp->setData(i,omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_value: pp->setValue(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sender: pp->setSender(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Decide'", field);
    }
}

const char *DecideDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr DecideDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void DecideDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Decide *pp = omnetpp::fromAnyPtr<Decide>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Decide'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

