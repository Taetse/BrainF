#include <fstream>
#include <stack>
#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

void translate(const string& input, ostream& out);

int main(int argc, char const *argv[])
{
  if(argc == 1)
  {
    cout << argv[0] << ": fatal error: no input files\ncompilation terminated.\n";
    return 1;
  }

  string code, temp;
  ifstream inFile(argv[1]);

  if(!inFile.is_open())
  {
    cout << argv[0] << ": error: " << argv[1] << ": No such file or directory\n" << argv[0] << ": fatal error: no input files\ncompilation terminated.\n";
    return 1;
  }

  while(inFile >> temp)
    code += temp;
  inFile.close();

  ofstream outFile("temp.asm");
  outFile << "BITS 64\n\nSECTION\t.data\narray: TIMES 30000 db 0\n\nSECTION\t.text\nglobal\t_start\n\n_start:\n\n\tmov rdx, array\n\n";
  translate(code, outFile);
  outFile << "\tmov rax,60\n\tmov rdi,0\n\tsyscall\n";
  outFile.close();

  temp = "ld p.o ";
  for(int a = 2; a < argc; a++) {
    temp += argv[a];
  }

  system(("nasm -f elf64 -o p.o temp.asm"));
  system(temp.c_str());
  remove("p.o");
  // remove("temp.asm");

  return 0;
}

void translate(const string& input, ostream& out)
{
  unsigned runningId = 0;
  stack<unsigned> loopId;
  int loopCount = 1;
  bool pendingLoop = false;

  for(int a = 0; a < input.length(); a++)
  {
    if(isspace(input[a]))
      continue;

    if((input[a] == input[a + 1]) && (input[a] == '>' || input[a] == '<' || input[a] == '+' || input[a] == '-')) {
      pendingLoop = true;
      loopCount++;
      continue;
    }

    else if(input[a] == '>') out << "\tadd rdx," << loopCount << "\n\n";

    else if(input[a] == '<') out << "\tsub rdx," << loopCount << "\n\n";

    else if(input[a] == '+') out << "\tadd byte [rdx], " << loopCount << "\n\n";

    else if(input[a] == '-') out << "\tsub byte [rdx], " << loopCount << "\n\n";

    else if(input[a] == '.') out << "\tmov  rax, 1\n\tmov  rsi, rdx\n\tmov  rdi, 1\n"
                                 << "\tpush rdx\n\tmov  rdx, 1\n\tsyscall\n\tpop rdx\n\n";

    else if(input[a] == '[') {
     loopId.push(runningId++);
     out << "\tjmp loop" << runningId - 1 << "\ncloop" << runningId - 1 << ":\n";
    }

    else if(input[a] == ']') {
     out << "loop" << loopId.top() << ":\n\tcmp byte [rdx], 0\n\tjne cloop" << loopId.top() << "\n";
     loopId.pop();
    }

    else if(input[a] == ',') out << "\tmov  rax, 0\n\tmov  rsi, rdx\n\tmov  rdi, 0\n"
                                 << "\tpush rdx\n\tmov  rdx, 1\n\tsyscall\n\tpop rdx\n\n";

    if(pendingLoop) {
      pendingLoop = false;
      loopCount = 1;
    }
  }
}
