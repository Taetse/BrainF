#include <fstream>
#include <list>
#include <string>

using namespace std;

#include "operators.h"

list<Operator*>* buildString(string input, Context* = new Context);

int main(int argc, char const *argv[])
{
  Control control;
  string code, temp;

  if(argc > 1)
  {
    ifstream inFile(argv[1]);
    while(inFile >> temp)
      code += temp;
    inFile.close();
    list<Operator*>* tokens = buildString(code);
    control.setFlow(tokens->front());

    while(control.excecute());
  }

  else
  {
    while(true)
    {
      cout << ">    ";
      getline(cin, code);
      if(code == "quit")
        return 0;

      list<Operator*>* tokens = buildString(code);
      control.setFlow(tokens->front());

      while(control.excecute());
    }
  }
  return 0;
}

list<Operator*>* buildString(string input, Context* con)
{
  list<Operator*>* finale = new list<Operator*>, loops;
  Operator* back;

  for(int a = 0; a < input.length(); a++, back = finale->back())
  {
    if(input[a] == '[') {
      finale->push_back(new Loop(con));
      loops.push_back(finale->back());
    }

    else if(input[a] == '>') finale->push_back(new Next(con));

    else if(input[a] == '<') finale->push_back(new Previous(con));

    else if(input[a] == '+') finale->push_back(new Increment(con));

    else if(input[a] == '-') finale->push_back(new Decrement(con));

    else if(input[a] == '.') finale->push_back(new Output(con));

    else if(input[a] == ',') finale->push_back(new Input(con));

    else if(input[a] == ']')
    {
      Operator* complement = loops.back();
      Operator* close = new EndLoop(con);

      finale->push_back(close);
      loops.push_back(close);
      (close)->setComplement(complement);
      (complement)->setComplement(close);
      loops.pop_back();
      loops.pop_back();
    }

    else continue;

    if(finale->size() > 1)
      (back)->setSuccesor(finale->back());
  }
  return finale;
}
