#ifndef OPERATORS_H
#define OPERATORS_H 1

#include <iostream>
#include <vector>

using namespace std;
class Operator;

class Context
{
public:
  Context(vector<char>* _array = new vector<char>(1, 0)): index(0), array(_array) {
    dataPtr = &(*array)[index];
  }

  void increment() {
    if(++index >= array->size()) { array->push_back(0); }
    dataPtr = &array->at(index);
  }

  void decrement() {
    if(--index < 0) { throw "Error: out of bounds"; }
    dataPtr = &array->at(index);
  }

  void incrementV() {
    ++*dataPtr;
  }

  void decrementV() {
    --*dataPtr;
  }

  char getData() const {
    return *dataPtr;
  }

  void setData(char val) {
    *dataPtr = val;
  }
private:
  char* dataPtr;
  vector<char>* array;
  int index;
};

class Control
{
public:
  Control(): commandPtr(NULL) {}
  bool excecute();
  void setFlow(Operator* op);
private:
  Operator* commandPtr;
};

class Operator
{
public:
  Operator(Context* con): context(con), succesor(0) {}

  Context* getContex() {
    return context;
  }

  void setSuccesor(Operator* suc) {
    succesor = suc;
  }

  Operator* getSuccesor() {
    return succesor;
  }

  virtual Operator* getComplement() {}

  virtual void setComplement(Operator* com) {}

  virtual void excecute(Control* con) {
    con->setFlow(succesor);
  }
private:
  Operator* succesor;
  Context* context;
};

class Next: public Operator
{
public:
  Next(Context* con): Operator(con) {}

  void excecute(Control* con) {
    getContex()->increment();
    Operator::excecute(con);
  }
};

class Previous: public Operator
{
public:
  Previous(Context* con): Operator(con) {}

  void excecute(Control* con) {
    getContex()->decrement();
    Operator::excecute(con);
  }
};

class Increment: public Operator
{
public:
  Increment(Context* con): Operator(con) {}

  void excecute(Control* con) {
    getContex()->incrementV();
    Operator::excecute(con);
  }
};

class Decrement: public Operator
{
public:
  Decrement(Context* con): Operator(con) {}

  void excecute(Control* con) {
    getContex()->decrementV();
    Operator::excecute(con);
  }
};

class Output: public Operator
{
public:
  Output(Context* con): Operator(con) {}

  void excecute(Control* con) {
    cout << getContex()->getData();
    Operator::excecute(con);
  }
};

class Input: public Operator
{
public:
  Input(Context* con): Operator(con) {}

  void excecute(Control* con) {
    char in;
    cin >> in;
    getContex()->setData(in);
    Operator::excecute(con);
  }
};

class Loop: public Operator
{
public:
  Loop(Context* con): Operator(con) {}

  void excecute(Control* con) {
    getContex()->getData()? Operator::excecute(con) : con->setFlow(getComplement()->getSuccesor());
  }

  Operator* getComplement() {
    return complement;
  }

  void setComplement(Operator* com) {
    complement = com;
  }
private:
  Operator* complement;
};

class EndLoop: public Operator
{
public:
  EndLoop(Context* con): Operator(con) {}

  void excecute(Control* con) {
    con->setFlow(getComplement());
  }

  Operator* getComplement() {
    return complement;
  }

  void setComplement(Operator* com) {
    complement = com;
  }
private:
  Operator* complement;
};


bool Control::excecute() {
  if(!commandPtr) { return false; }

  commandPtr->excecute(this);
  return true;
}

void Control::setFlow(Operator* op) {
  commandPtr = op;
}

#endif
