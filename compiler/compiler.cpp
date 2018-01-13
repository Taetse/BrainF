#include <fstream>
#include <list>
#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

string buildString(string);

int main(int argc, char const *argv[])
{
  string cmd, code, temp, finale = "#include<iostream>\n#include<vector>\nint main() {\nstd::vector<char> data(1, 0);\nint index = 0;\nchar* dataPtr = &data[index];\n";

  for(int a = 2; a < argc; a++)
  {
    temp = argv[a];
    cmd += (temp + " ");
  }

  ifstream inFile(argv[1]);
  while(inFile >> temp)
    code += temp;
  inFile.close();
  finale += buildString(code);

  finale += "\n\treturn 0;\n}\n";

  ofstream outFile("etrewjfd9opqgfdfdhnjvgfdsoggbnfmds.cpp");
  outFile << finale << "\n";
  outFile.close();

  system(("g++ etrewjfd9opqgfdfdhnjvgfdsoggbnfmds.cpp " + cmd).c_str());
  remove("etrewjfd9opqgfdfdhnjvgfdsoggbnfmds.cpp");

  outFile.open(argv[1]);
  outFile << code << endl;
  outFile.close();

  return 0;
}

string buildString(string input) {
  string finale, fin, temp1 = "\tfor(int a = 0; a < ", temp2 = "; a++)\n\t\tdata.push_back(0);\n";

  int length = 0;
  for(int a = 0; a < input.length(); a++)
  {
    if(input[a] == '[') finale += "\twhile(*dataPtr) {\n";

    else if(input[a] == '>')
    {
      finale += "\tdataPtr = &data[++index];\n";
      length++;
    }

    else if(input[a] == '<') finale += "\tdataPtr = &data[--index];\n";

    else if(input[a] == '+') finale += "\t++*dataPtr;\n";

    else if(input[a] == '-') finale += "\t--*dataPtr;\n";

    else if(input[a] == '.') finale += "\tstd::cout << *dataPtr;\n";

    else if(input[a] == ',') finale += "\tstd::cin >> *dataPtr;\n";

    else if(input[a] == ']') finale += "\t}\n";

    else continue;
  }
  fin = (temp1 + to_string(length) + temp2 + finale);
  return fin;
}
