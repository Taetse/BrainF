#include <cstring>
#include <sys/mman.h>
#include <vector>
#include <stack>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;

int main(int argc, char const *argv[])
{
  unsigned int loopCount = 1;
  bool pendingLoop = false;
  string input, tempS;

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
  vector<unsigned char> code;

  //mov    rdx,0x0
  unsigned char init[] = { 0x48, 0xC7, 0xC2, 0x00, 0x00, 0x00, 0x00 };
  memcpy(&init[3], &array, 4);
  code.insert(code.end(), &init[0], &init[7]);

  stack<int> index;

  for(unsigned a = 0; a < input.length(); a++) {
    if((input[a] == input[a + 1]) && (input[a] == '>' || input[a] == '<' || input[a] == '+' || input[a] == '-')) {
      pendingLoop = true;
      loopCount++;
      continue;
    }

    if(input[a] == '>') {
      //add    rdx, 1
      unsigned char temp[] = { 0x48, 0x83, 0xC2, 0x00 };
      memcpy(&temp[3], &loopCount, 1);
      code.insert(code.end(), &temp[0], &temp[4]);
    }

    else if(input[a] == '<') {
      //sub    rdx, 1
      unsigned char temp[] = { 0x48, 0x83, 0xea, 0x00 };
      memcpy(&temp[3], &loopCount, 1);
      code.insert(code.end(), &temp[0], &temp[4]);
    }

    else if(input[a] == '+') {
      //add    [rdx], 1
      unsigned char temp[] = { 0x80, 0x02, 0x00 };
      memcpy(&temp[2], &loopCount, 1);
      code.insert(code.end(), &temp[0], &temp[3]);
    }

    else if(input[a] == '-') {
      //sub    [rdx], 1
      unsigned char temp[] = { 0x80, 0x2a, 0x00 };
      memcpy(&temp[2], &loopCount, 1);
      code.insert(code.end(), &temp[0], &temp[3]);
    }

    else if(input[a] == '.') {
      //mov  rax, 1: mov  rsi, rdx: mov  rdi, 1: push rdx: mov  rdx, 1: syscall: pop rdx
      unsigned char temp[] = { 0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,
          0x48, 0x89, 0xD6, 0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x52,
          0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05, 0x5A };
      code.insert(code.end(), &temp[0], &temp[28]);
    }

    else if(input[a] == '[') {
      index.push(code.size());
      unsigned char temp[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 }; //jmp    0x5
      code.insert(code.end(), &temp[0], &temp[5]);
    }

    else if(input[a] == ']') {
      if(!index.size()) {
        cout << "Error: bracket missmatch\n";
        throw "error";
      }
      int dist = -((code.size() + 9) - (index.top() + 5));
      //cmp    BYTE [edx],0x0: jne    0x9
      unsigned char temp[] = { 0x80, 0x3A, 0x00, 0x0F, 0x85, 0x00, 0x00, 0x00, 0x00 };
      memcpy(&temp[5], &dist, 4);
      dist = (code.size() - index.top() - 5);
      memcpy(&code[index.top() + 1], &dist, 4);
      code.insert(code.end(), &temp[0], &temp[9]);
      index.pop();
    }

    if(pendingLoop) {
      pendingLoop = false;
      loopCount = 1;
    }
  }
  unsigned char temp[] = { 0xc3 };
  code.insert(code.end(), temp, &temp[1]);


  void *mem = mmap(NULL, code.size(), PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
  memcpy(mem, &code[0], code.size());
  auto start = chrono::high_resolution_clock::now();

  reinterpret_cast<void(*)()>(mem)();

  auto finish = chrono::high_resolution_clock::now();
  auto microseconds = chrono::duration_cast<chrono::microseconds>(finish-start);
  cout << "done in " << microseconds.count() / 1000 << " ms" << endl;
  delete [] array;
  munmap(mem, code.size());

  return 0;
}
