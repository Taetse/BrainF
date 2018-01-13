#include <cstring>
#include <sys/mman.h>
#include <stack>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;

int main(int argc, char const *argv[])
{
  char current;
  int dist;
  unsigned int loopCount = 1;
  bool half = false;
  string input, tempS;
  vector<unsigned char> code;
  stack<unsigned int> index;

  if(argc > 1) {
    fstream inFile(argv[1]);
    while(inFile >> tempS)
      input+= tempS;
    inFile.close();
  }

  else while(cin >> tempS) {
    if(tempS == "!") break;
    input+= tempS;
  }

  char* array = new char[30000];
  for(int a = 0; a < 30000; ++a)
    array[a] = 0;

  auto start = chrono::high_resolution_clock::now();
  //mov    rdx,0x0
  unsigned char init[] = { 0x48, 0xC7, 0xC2, 0x00, 0x00, 0x00, 0x00 };
  memcpy(&init[3], &array, 4);
  code.insert(code.end(), init, &init[7]);


  for(unsigned a = 0; a < input.length(); ++a) {
    current = input[a];
    half = (current == '>' || current == '<' || current == '+' || current == '-');
    if( half && (current == input[a + 1])) loopCount++;

    else if(half) {
      if(loopCount > 65535) throw "Error";
      if(input[a] == '>') {
        //add   rdx, 0
        unsigned char temp[] = { 0x48, 0x81, 0xC2, 0x00, 0x00, 0x00, 0x00 };
        memcpy(&temp[3], &loopCount, 4);
        code.insert(code.end(), temp, &temp[7]);
      }

      else if(current == '<') {
        //sub   rdx, 0
        unsigned char temp[] = { 0x48, 0x81, 0xea, 0x00, 0x00, 0x00, 0x00 };
        memcpy(&temp[3], &loopCount, 4);
        code.insert(code.end(), temp, &temp[7]);
      }

      else if(current == '+') {
        //add  WORD  [rdx], x00 x00
        unsigned char temp[] = { 0x66, 0x81, 0x02, 0x00, 0x00 };
        memcpy(&temp[3], &loopCount, 2);
        code.insert(code.end(), temp, &temp[5]);
      }

      else if(current == '-') {
        //sub  WORD  [rdx], x00 x00
        unsigned char temp[] = { 0x66, 0x81, 0x2a, 0x00, 0x00 };
        memcpy(&temp[3], &loopCount, 2);
        code.insert(code.end(), temp, &temp[5]);
      }

      loopCount = 1;
    }

    else if(current == '.') {
      //mov  rax, 1: mov  rsi, rdx: mov  rdi, 1: push rdx: mov  rdx, 1: syscall: pop rdx
      unsigned char temp[] = { 0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,
          0x48, 0x89, 0xD6, 0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x52,
          0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05, 0x5A };
      code.insert(code.end(), temp, &temp[28]);
    }

    else if(current == '[') {
      index.push(code.size());
      unsigned char temp[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 }; //jmp    0x5
      code.insert(code.end(), temp, &temp[5]);
    }

    else if(current == ']') {
      if(!index.size()) throw "error";
      dist = -((code.size() + 9) - (index.top() + 5));
      //cmp    BYTE [edx],0x0: jne    0x9
      unsigned char temp[] = { 0x80, 0x3A, 0x00, 0x0F, 0x85, 0x00, 0x00, 0x00, 0x00 };
      memcpy(&temp[5], &dist, 4);
      dist = (code.size() - index.top() - 5);
      memcpy(&code[index.top() + 1], &dist, 4);
      code.insert(code.end(), temp, &temp[9]);
      index.pop();
    }

    else if(current == ',') {
      //mov  rax, 0: mov  rsi, rdx: mov  rdi, 0: push rdx: mov  rdx, 1: syscall: pop rdx
      unsigned char temp[] = { 0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,
          0x48, 0x89, 0xD6, 0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x52,
          0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05, 0x5A };
      code.insert(code.end(), temp, &temp[28]);
    }
  }

  //return
  unsigned char temp[] = { 0xc3 };
  code.insert(code.end(), temp, &temp[1]);


  void *mem = mmap(NULL, code.size(), PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
  memcpy(mem, &code[0], code.size());
  auto finish = chrono::high_resolution_clock::now();
  auto compiletime = chrono::duration_cast<chrono::microseconds>(finish-start);

  start = chrono::high_resolution_clock::now();

  reinterpret_cast<void(*)()>(mem)();

  finish = chrono::high_resolution_clock::now();
  auto runtime = chrono::duration_cast<chrono::microseconds>(finish-start);
  cout << "ran in " << runtime.count() / 1000 << " ms and compiled in " << compiletime.count() << " μs" << endl;
  delete [] array;
  munmap(mem, code.size());

  return 0;
}
