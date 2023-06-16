#include <map>
#include <stack>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

FILE *input = NULL;
FILE *output = NULL;

int lastCell = 99;
int currentCommandCell = 0;
int variablesNum = 0;
int position = 0;

class Vari {
private:
  string title;
  int address;

public:
  Vari() {}
  Vari(string tit, int addr) {
    title = tit;
    address = addr;
  }
  string getTit() { return title; }
  int getAddr() { return address; }
};

struct CommandAsmb {
  CommandAsmb(int cell, char *command, int operand) {
    this->cell = cell;
    this->command = command;
    this->operand = operand;
  }
  int cell;
  char *command;
  int operand;
};

vector<Vari> variables;
vector<CommandAsmb> commands;
vector<int> queueGoTo;
map<int, int> startCommands;
stack<int> Operands;
stack<char> Functions;

int presenceVarCheck(string str);
int Input(char *operand);
int Print(char *operand);
int GoTo(char *operand);
int End(char *operand);
int Let(char *str_operands);
int If(char *str_operands);
void popFunction();
bool canPop(char op1);
int getPriority(char op);
string getToken(string s);
string readInteger(string s);
void readWhiteSpace(string s);

int translate_bsc(char *to_translate, char *save_file) {
  input = fopen(to_translate, "r");
  if (!input) {
    std::cerr << "Ошибка чтения файла " << to_translate << std::endl;
    return -1;
  }
  output = fopen(save_file, "w");

  int previous_addr = -1;
  bool checkMainEnd = false;
  while (!feof(input)) {
    char line[200];
    fgets(line, 200, input);

    // адресс ячейки памяти
    char *str_line = strtok(line, "\n");
    char *str_address = strtok(str_line, " ");
    int address = 0;
    address = atoi(str_address);
    if (strlen(str_address) > 2 || address < 0 || address > 99) {
      return -1;
    }
    if (address < previous_addr) {
      return -1;
    }
    previous_addr = address;

    // команда
    char *str_command = strtok(NULL, " ");
    if (!strcmp(str_command, "REM")) {
      continue;
    } else if (!strcmp(str_command, "INPUT")) {
      char *str_operand = strtok(NULL, " \n");
      startCommands[address] = currentCommandCell;
      if (Input(str_operand))
        return -1;
    } else if (!strcmp(str_command, "PRINT")) {
      char *str_operand = strtok(NULL, " \n");
      startCommands[address] = currentCommandCell;
      if (Print(str_operand))
        return -1;
    } else if (!strcmp(str_command, "GOTO")) {
      char *str_operand = strtok(NULL, " ");
      startCommands[address] = currentCommandCell;
      if (GoTo(str_operand))
        return -1;
    } else if (!strcmp(str_command, "END")) {
      char *str_operand = strtok(NULL, "");
      startCommands[address] = currentCommandCell;
      if (End(str_operand))
        return -1;
      checkMainEnd = true;
      break;
    } else if (!strcmp(str_command, "LET")) {
      char *str_operand = strtok(NULL, "");
      startCommands[address] = currentCommandCell;
      if (Let(str_operand))
        return -1;
    } else if (!strcmp(str_command, "IF")) {
      char *str_operand = strtok(NULL, "");
      startCommands[address] = currentCommandCell;
      if (If(str_operand))
        return -1;
    }

    if (currentCommandCell > lastCell) {
      return -1;
    }
  }
  if (!checkMainEnd) {
    return -1;
  }
  fclose(input);
  for (auto &i : queueGoTo) {
    if (startCommands.find(commands[i].operand) ==
        startCommands.end()) { // Если не нашел
      return -1;
    } else {
      short addressOperand = startCommands.find(commands[i].operand)->second;
      commands[i].operand = addressOperand;
    }
  }
  // Запись команд
  int gg = 0;
  for (auto &i : commands) {
    if (gg == 0) {
      fprintf(output, "%.2d %s %.2d", i.cell, i.command, i.operand);
      gg++;
    } else
      fprintf(output, "\n%.2d %s %.2d", i.cell, i.command, i.operand);
  }
  // Запись переменных
  for (int i = variables.size() - 1; i >= 0; i--) {
    if (!((variables[i].getTit()[0] >= 'A') &&
          (variables[i].getTit()[0] <= 'Z')))
      fprintf(output, "\n%.2d = +%.4X", variables[i].getAddr(),
              stoi(variables[i].getTit()));
  }
  fclose(output);

  return 0;
}

int Input(char *operand) {
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z'))) {
    return -1;
  }
  int addressOperand = presenceVarCheck(operand);
  commands.emplace_back(
      CommandAsmb(currentCommandCell++, (char *)"READ", addressOperand));
  return 0;
}

int Print(char *operand) {
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z'))) {
    return -1;
  }
  int addressOperand = presenceVarCheck(operand);
  commands.emplace_back(
      CommandAsmb(currentCommandCell++, (char *)"WRITE", addressOperand));
  return 0;
}

int GoTo(char *operand) {
  int address = atoi(operand);
  if (strlen(operand) > 2 || address < 0 || address > 99) {
    return -1;
  }
  if (startCommands.find(address) == startCommands.end()) {
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"JUMP", address));
    queueGoTo.emplace_back(commands.size() - 1);
  } else {
    int addressOperand = startCommands.find(address)->second;
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"JUMP", addressOperand));
  }
  return 0;
}

int End(char *operand) {
  if (operand != NULL) {
    return -1;
  }
  commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"HALT", 0));
  return 0;
}

int If(char *operand) {
  char *value1 = strtok(operand, " ");
  char *condition = strtok(NULL, " ");
  char *value2 = strtok(NULL, " ");
  char *command = strtok(NULL, " ");
  char *str_address = strtok(NULL, "");
  int addressvalue1 = presenceVarCheck(value1);
  int addressvalue2 = presenceVarCheck(value2);
  int basic_address = atoi(str_address);
  int goto_address = 0;
  if (!(!((value1[0] >= 'A') && (value1[0] <= 'Z')) ||
        !((value2[0] >= 'A') && (value2[0] <= 'Z'))) &&
      !(!((value1[0] >= '0') && (value1[0] <= '9')) ||
        !((value2[0] >= '0') && (value2[0] <= '9'))) &&
      strcmp(command, "GOTO") != 0) {
    return -1;
  }
  if (startCommands.find(basic_address) == startCommands.end()) {
    goto_address = basic_address;
    queueGoTo.emplace_back(commands.size() + 2);
  } else {
    goto_address = startCommands.find(basic_address)->second;
  }
  if (!strcmp(condition, "=")) {
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"LOAD", addressvalue1));
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"SUB", addressvalue2));
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"JZ", goto_address));
  } else {
    if (!strcmp(condition, "<")) {
      commands.emplace_back(
          CommandAsmb(currentCommandCell++, (char *)"LOAD", addressvalue1));
      commands.emplace_back(
          CommandAsmb(currentCommandCell++, (char *)"SUB", addressvalue2));
    } else {
      commands.emplace_back(
          CommandAsmb(currentCommandCell++, (char *)"LOAD", addressvalue2));
      commands.emplace_back(
          CommandAsmb(currentCommandCell++, (char *)"SUB", addressvalue1));
    }
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"JNEG", goto_address));
  }

  return 0;
}

int Let(char *operand) {
  position = 0;
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z')) &&
      (operand[1] != '=' || operand[2] != '=')) {
    return -1;
  }
  char *var = strtok(operand, " ");
  char *value = strtok(NULL, " =");
  char *epresh = strtok(NULL, "");
  if (epresh == NULL) {
    int valueAddress = presenceVarCheck(value);
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"LOAD", valueAddress));

    int varAddress = presenceVarCheck(var);
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"STORE", varAddress));
  } else {
    string expr = epresh;
    expr = "(" + (string)value + " " + expr + ")";
    string token, prevToken;
    do {
      token = getToken(expr);
      // унарный + и -
      if (prevToken == "(" && (token == "+" || token == "-")) {
        int address = presenceVarCheck((string) "0");
        Operands.push(address);
      }

      if (isdigit(token[0]) ||
          ((token[0] >= 'A') && (token[0] <= 'Z'))) // Если операнд
      {
        int address = presenceVarCheck(token);
        Operands.push(address);
      } else // Если операция
      {
        if (token == ")") {
          while (Functions.size() > 0 && Functions.top() != '(')
            popFunction();

          Functions.pop();
        } else {
          while (canPop(token[0]))
            popFunction();

          Functions.push(token[0]);
        }
      }
      prevToken = token;

    } while (position != (int)expr.length());
    int varAddress = presenceVarCheck(var);
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"STORE", varAddress));
    Operands.pop();
  }
  return 0;
}

void popFunction() {
  int B = Operands.top();
  Operands.pop();
  int A = Operands.top();
  Operands.pop();
  char oper = Functions.top();
  Functions.pop();

  int address = lastCell - variablesNum;
  variablesNum++;
  switch (oper) {
  case '+':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"ADD", B));
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"STORE", address));
    break;
  case '-':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"SUB", B));
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"STORE", address));
    break;
  case '*':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"MUL", B));
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"STORE", address));
    break;
  case '/':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A));
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"DIVIDE", B));
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"STORE", address));
    break;
  }
  Operands.push(address);
}

bool canPop(char op1) {
  if (Functions.size() == 0)
    return false;
  int p1 = getPriority(op1);
  int p2 = getPriority(Functions.top());

  return p1 >= 0 && p2 >= 0 && p1 >= p2;
}

int getPriority(char op) {
  switch (op) {
  case '(':
    return -1;
  case '*':
  case '/':
    return 1;
  case '+':
  case '-':
    return 2;
  default:
    return -1;
  }
}

string getToken(string s) {
  readWhiteSpace(s);
  string tok = "";

  if (position == (int)s.length())
    return tok;
  if (isdigit(s[position]))
    return readInteger(s);
  else
    return tok += s[position++];
}

string readInteger(string s) {
  string res = "";
  while (position < (int)s.length() && isdigit(s[position]))
    res += s[position++];

  return res;
}

void readWhiteSpace(string s) {
  while (position < (int)s.length() && isspace(s[position]))
    position++;
}

int presenceVarCheck(string str) {
  for (int i = 0; i < variablesNum; i++) {
    if (variables[i].getTit() == str) {
      return variables[i].getAddr();
    }
  }
  variables.emplace_back(Vari(str, lastCell - variablesNum));
  variablesNum++;
  return variables[variablesNum - 1].getAddr();
}

int main(int argc, char *argv[]) {
  std::cout << std::endl;
  if (argc != 3) {
    std::cout << "Неверное количество аргументов!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string fileName = argv[1];
  size_t slashPos = fileName.find_last_of("/");
  if (slashPos != std::string::npos) {
    fileName = fileName.substr(slashPos + 1);
  }
  size_t dotPos = fileName.find_last_of(".");
  if (dotPos != std::string::npos) {
    fileName = fileName.substr(0, dotPos);
  }
  fileName += ".sa";
  std::string term_command = "mv temp_asmb.sa project/assembler/" + fileName;
  translate_bsc(argv[1], (char *)"temp_asmb.sa");
  system(term_command.c_str());
  std::cout << "Трансляция в SimpleAssembler завершена!" << std::endl;
  std::cout << "Нажмите Enter для трансляции из SimpleAssembler в машинный код";
  std::cin.get();
  std::cout << std::endl;
}
  