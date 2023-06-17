#include <map>
#include <stack>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

FILE *input = NULL;
FILE *output = NULL;

int lastCell = 99; // Последняя ячейка памяти
int currentCommandCell = 0; // Текущая ячейка памяти для команды
int variablesNum = 0; // Количество переменных
int position = 0; // Позиция при чтении выражений

// Класс для представления переменных
class Vari {
private:
  string title; // Название переменной
  int address; // Адрес переменной
public:
  Vari() {}
  Vari(string tit, int addr) {
    title = tit;
    address = addr;
  }
  string getTit() { return title; }
  int getAddr() { return address; }
};

// Структура для представления команды ассемблера
struct CommandAsmb {
  CommandAsmb(int cell, char *command, int operand) {
    this->cell = cell;
    this->command = command;
    this->operand = operand;
  }
  int cell; // Адрес ячейки памяти
  char *command;
  int operand;
};

vector<Vari> variables; // Вектор переменных
vector<CommandAsmb> commands; // Вектор команд ассемблера
vector<int> queueGoTo; // Очередь для команды GOTO
map<int, int> startCommands; // Словарь для хранения адресов начала команд
stack<int> Operands; // Стек операндов
stack<char> Functions; // Стек операций

// Функция для проверки наличия переменной
int presenceVarCheck(string str);

// Функции для обработки различных команд
int Input(char *operand);
int Print(char *operand);
int GoTo(char *operand);
int End(char *operand);
int Let(char *str_operands);
int If(char *str_operands);

// Вспомогательные функции для работы с выражениями
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

  int previous_addr = -1; // Для проверки правильности адресов ячеек памяти в файле
  bool checkMainEnd = false;
  while (!feof(input)) {
    char line[200]; // Строка для хранения текущей команды из файла
    fgets(line, 200, input); // Считываем строку

    // адресс ячейки памяти
    char *str_line = strtok(line, "\n"); // Разбиваем строку на отдельные токены
    char *str_address = strtok(str_line, " "); // Получаем адрес
    int address = 0;
    address = atoi(str_address); // Преобразование адреса в числовой формат
    if (strlen(str_address) > 2 || address < 0 || address > 99) { // Проверка на корректность значения адреса
      return -1; // Если значение не корректно, то завершаем выполнение с ошибкой
    }
    if (address < previous_addr) { // Проверка на то, что адреса идут по порядку
      return -1; // Если не идут, то завершаем выполнение с ошибкой
    }
    previous_addr = address; // Сохраняем адрес

    // команда
    char *str_command = strtok(NULL, " "); // Получаем команду
    if (!strcmp(str_command, "REM")) { // Если команда REM
      continue; // Пропускаем строку
    } else if (!strcmp(str_command, "INPUT")) { // Если команда INPUT
      char *str_operand = strtok(NULL, " \n"); // Получаем операнд
      startCommands[address] = currentCommandCell; // Добавляем команду в словарь startCommands
      if (Input(str_operand)) // Если вызов функции Input вернул ошибку
        return -1; // завершаем выполнение с ошибкой
    } else if (!strcmp(str_command, "PRINT")) { // Если команда PRINT
      char *str_operand = strtok(NULL, " \n"); // Получаем операнд
      startCommands[address] = currentCommandCell; // Добавляем команду в словарь startCommands
      if (Print(str_operand)) // Если вызов функции Print вернул ошибку
        return -1; // завершаем выполнение с ошибкой
    } else if (!strcmp(str_command, "GOTO")) { // Если команда GOTO
      char *str_operand = strtok(NULL, " "); // Получаем операнд
      startCommands[address] = currentCommandCell; // Добавляем команду в словарь startCommands
      if (GoTo(str_operand)) // Если вызов функции GoTo вернул ошибку
        return -1; // завершаем выполнение с ошибкой
    } else if (!strcmp(str_command, "END")) { // Если команда END
      char *str_operand = strtok(NULL, ""); // Получаем операнд
      startCommands[address] = currentCommandCell; // Добавляем команду в словарь startCommands
      if (End(str_operand)) // Если вызов функции End вернул ошибку
        return -1; // завершаем выполнение с ошибкой
      checkMainEnd = true; // Помечаем, что в программе есть команда END
      break; // Завершаем выполнение цикла
    } else if (!strcmp(str_command, "LET")) { // Если команда LET
      char *str_operand = strtok(NULL, ""); // Получаем операнд
      startCommands[address] = currentCommandCell; // Добавляем команду в словарь startCommands
      if (Let(str_operand)) // Если вызов функции Let вернул ошибку
        return -1; // завершаем выполнение с ошибкой
    } else if (!strcmp(str_command, "IF")) { // Если команда IF
      char *str_operand = strtok(NULL, "");  // Получаем операнд
      startCommands[address] = currentCommandCell; // Добавляем команду в словарь startCommands
      if (If(str_operand)) // Если вызов функции If вернул ошибку
        return -1; // завершаем выполнение с ошибкой
    }

    if (currentCommandCell > lastCell) { // Проверяем, что мы не вышли за пределы массива команд
      return -1; // Если вышли, завершаем с ошибкой
    }
  }
  if (!checkMainEnd) { // Проверяем, что в программе есть команда END
    return -1; // Если нет, завершаем выполнение с ошибкой
  }
  fclose(input); // Закрываем входной файл
  for (auto &i : queueGoTo) { // Проходим по хранилищу адресов для GoTo
    if (startCommands.find(commands[i].operand) == startCommands.end()) { // Если адрес не был найден
      return -1; // Завершаем выполнение с ошибкой
    } else {
      short addressOperand = startCommands.find(commands[i].operand)->second; // Получаем адрес операнда из startCommands
      commands[i].operand = addressOperand; // Изменяем значение операнда в хранилище команд
    }
  }
  // Запись команд
  int gg = 0; // Вспомогательная переменная для форматирования вывода
  for (auto &i : commands) { // Проходим по хранилищу команд
    if (gg == 0) { // Если вывод первой команды
      fprintf(output, "%.2d %s %.2d", i.cell, i.command, i.operand); // Форматированный вывод команды
      gg++; // Переводим gg на следующую итерацию цикла
    } else
      fprintf(output, "\n%.2d %s %.2d", i.cell, i.command, i.operand); // Форматированный вывод команды
  }
  // Запись переменных
  for (int i = variables.size() - 1; i >= 0; i--) { // Проходим по хранилищу переменных в обратном порядке
    if (!((variables[i].getTit()[0] >= 'A') && (variables[i].getTit()[0] <= 'Z'))) // Проверяем, что переменная корректна
      fprintf(output, "\n%.2d = +%.4X", variables[i].getAddr(), stoi(variables[i].getTit())); // Форматированный вывод переменной
  }
  fclose(output); // Закрываем выходной файл

  return 0; // Возвращаем успешное выполнение функции
}


int Input(char *operand) {
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z'))) { // Проверяем, является ли операнд допустимым
    return -1; // Если операнд не допустимый, возвращаем -1
  }
  int addressOperand = presenceVarCheck(operand); // Получаем адрес операнда
  commands.emplace_back(
      CommandAsmb(currentCommandCell++, (char *)"READ", addressOperand)); // Генерируем код для чтения значения переменной
  return 0;
}


int Print(char *operand) {
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z'))) { // Проверяем, является ли операнд допустимым
    return -1; // Если операнд не допустимый, возвращаем -1
  }
  int addressOperand = presenceVarCheck(operand); // Получаем адрес операнда
  commands.emplace_back(
      CommandAsmb(currentCommandCell++, (char *)"WRITE", addressOperand)); // Генерируем код для записи значения переменной в вывод
  return 0;
}


int GoTo(char *operand) {
  int address = atoi(operand); // Преобразуем строковое значение адреса в целое число
  // Проверяем, является ли адрес допустимым
  if (strlen(operand) > 2 || address < 0 || address > 99) {
    return -1; // Если адрес не допустимый, возвращаем -1
  }
  if (startCommands.find(address) == startCommands.end()) { // Если адрес не присутствует в словаре startCommands
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"JUMP", address)); // Генерируем код для безусловного перехода на указанный адрес
    queueGoTo.emplace_back(commands.size() - 1); // Добавляем адрес команды в очередь GoTo для последующей замены, если потребуется
  } else {
    int addressOperand = startCommands.find(address)->second; // Получаем адрес команды, соответствующей указанному адресу
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"JUMP", addressOperand)); // Генерируем код для безусловного перехода на нужную команду
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
  // Разбиваем операнд на отдельные части с помощью strtok
  char *value1 = strtok(operand, " "); // первое значение операнда
  char *condition = strtok(NULL, " "); // условие
  char *value2 = strtok(NULL, " "); // второе значение операнда
  char *command = strtok(NULL, " "); // команда после If (обычно GOTO)
  char *str_address = strtok(NULL, ""); // строковое значение адреса перехода

  // Проверяем, являются ли значения операнда и команда допустимыми
  int addressvalue1 = presenceVarCheck(value1);
  int addressvalue2 = presenceVarCheck(value2);
  int basic_address = atoi(str_address); // адрес перехода
  int goto_address = 0;
  if (!(!((value1[0] >= 'A') && (value1[0] <= 'Z')) || // Проверяем, являются ли значения операнда и команда допустимыми
        !((value2[0] >= 'A') && (value2[0] <= 'Z'))) &&
      !(!((value1[0] >= '0') && (value1[0] <= '9')) ||
        !((value2[0] >= '0') && (value2[0] <= '9'))) &&
      strcmp(command, "GOTO") != 0) {
    return -1; // Если значения операнда и команда не допустимы, возвращаем -1
  }
  // Если адрес перехода присутствует в словаре startCommands, берем значение из словаря иначе, добавляем новый адрес в очередь
  if (startCommands.find(basic_address) == startCommands.end()) {
    goto_address = basic_address;
    queueGoTo.emplace_back(commands.size() + 2);
  } else {
    goto_address = startCommands.find(basic_address)->second;
  }
  // Генерируем код в зависимости от условия
  if (!strcmp(condition, "=")) {
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"LOAD", addressvalue1)); // загружаем значение первого операнда
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"SUB", addressvalue2)); // отнимаем значение второго операнда
    commands.emplace_back(
        CommandAsmb(currentCommandCell++, (char *)"JZ", goto_address)); // если результат равен нулю, переходим по адресу goto_address
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
        CommandAsmb(currentCommandCell++, (char *)"JNEG", goto_address)); // если результат отрицательный, переходим по адресу goto_address
  }
  return 0;
}

int Let(char *operand) { // начало определения функции Let, которая принимает операнд в виде строки
  position = 0; // инициализация переменной position
  if (!((operand[0] >= 'A') && (operand[0] <= 'Z')) && (operand[1] != '=' || operand[2] != '=')) { // если первый символ операнда не является заглавной латинской буквой или два следующих символа не являются знаком равенства "==", то возвращаем ошибку
    return -1;
  }
  char *var = strtok(operand, " "); // режем операнд на части, разделенные пробелом, и записываем первую часть в переменную var
  char *value = strtok(NULL, " ="); // записываем следующую часть после пробела или знака равенства в переменную value
  char *epresh = strtok(NULL, ""); // оставшуюся часть записываем в переменную epresh
  if (epresh == NULL) { // если epresh пуста
    int valueAddress = presenceVarCheck(value); // получаем адрес переменной value
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", valueAddress)); // добавляем команду "LOAD" в текущую ячейку
    int varAddress = presenceVarCheck(var); // получаем адрес переменной var
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"STORE", varAddress)); // добавляем команду "STORE" в текущую ячейку
  } else { // если epresh не пуста
    string expr = epresh; // записываем epresh в строковую переменную expr
    expr = "(" + (string)value + " " + expr + ")"; // добавляем к expr начало и конец выражения
    string token, prevToken; // инициализируем строковые переменные token и prevToken
    do {
      token = getToken(expr); // получаем следующий токен из expr
      if (prevToken == "(" && (token == "+" || token == "-")) { // если предыдущий токен - открывающая скобка, и текущий токен - плюс или минус, то
        int address = presenceVarCheck((string) "0"); // получаем адрес переменной "0"
        Operands.push(address); // кладем адрес в стек операндов
      }

      if (isdigit(token[0]) || ((token[0] >= 'A') && (token[0] <= 'Z'))) { // если первый символ токена - цифра или заглавная латинская буква, то
        int address = presenceVarCheck(token); // получаем адрес переменной token
        Operands.push(address); // кладем адрес в стек операндов
      } else { // если первый символ токена - знак операции
        if (token == ")") { // если токен - закрывающая скобка
          while (Functions.size() > 0 && Functions.top() != '(') // пока стек функций не пуст и верхняя функция на стеке не является открывающей скобкой
            popFunction(); // выталкиваем функции из стека
          Functions.pop(); // выталкиваем открывающую скобку из стека
        } else { // если токен - не закрывающая скобка
          while (canPop(token[0])) // пока можно извлекать функции из стека
            popFunction(); // выталкиваем функции из стека
          Functions.push(token[0]); // помещаем знак операции в стек функций
        }
      }
      prevToken = token; // запоминаем текущий токен как предыдущий

    } while (position != (int)expr.length()); // повторяем цикл, пока не обработаем все токены в expr
    int varAddress = presenceVarCheck(var); // получаем адрес переменной var
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"STORE", varAddress)); // добавляем команду "STORE" в текущую ячейку
    Operands.pop(); // извлекаем адрес из стека операндов
  }
  return 0; // возвращаем успешное выполнение функции Let
}

void popFunction() { // начало определения функции popFunction, которая извлекает функцию из стека функций, извлекает два операнда из стека операндов, выполняет операцию и кладет результат в стек операндов
  int B = Operands.top(); // получаем верхний операнд из стека операндов и записываем его в переменную B
  Operands.pop(); // извлекаем верхний операнд из стека операндов
  int A = Operands.top(); // получаем верхний операнд из стека операндов и записываем его в переменную A
  Operands.pop(); // извлекаем верхний операнд из стека операндов
  char oper = Functions.top(); // получаем верхнюю функцию из стека функций и записываем ее в переменную oper
  Functions.pop(); // извлекаем верхнюю функцию из стека функций

  int address = lastCell - variablesNum; // получаем адрес для записи результата операции
  variablesNum++; // увеличиваем переменную-счетчик числа переменных
  switch (oper) { // выбираем нужную операцию в зависимости от знака функции
  case '+':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A)); // добавляем команду "LOAD" с операндом A
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"ADD", B)); // добавляем команду "ADD" с операндом B
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"STORE", address)); // добавляем команду "STORE" с адресом для записи результата операции
    break;
  case '-':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A)); // добавляем команду "LOAD" с операндом A
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"SUB", B)); // добавляем команду "SUB" с операндом B
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"STORE", address)); // добавляем команду "STORE" с адресом для записи результата операции
    break;
  case '*':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A)); // добавляем команду "LOAD" с операндом A
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"MUL", B)); // добавляем команду "MUL" с операндом B
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"STORE", address)); // добавляем команду "STORE" с адресом для записи результата операции
    break;
  case '/':
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"LOAD", A)); // добавляем команду "LOAD" с операндом A
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"DIVIDE", B)); // добавляем команду "DIVIDE" с операндом B
    commands.emplace_back(CommandAsmb(currentCommandCell++, (char *)"STORE", address)); // добавляем команду "STORE" с адресом для записи результата операции
    break;
  }
  Operands.push(address); // кладем адрес для записи результата операции в стек операндов
}

bool canPop(char op1) { // начало определения функции canPop, которая проверяет возможность извлечения функции из стека функций
  if (Functions.size() == 0)
    return false;
  int p1 = getPriority(op1); // получаем приоритет текущей операции
  int p2 = getPriority(Functions.top()); // получаем приоритет верхней функции в стеке функций

  return p1 >= 0 && p2 >= 0 && p1 >= p2; // возвращаем результат проверки на возможность извлечения функции из стека функций
}

int getPriority(char op) { // начало определения функции getPriority, которая возвращает приоритет операции
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

string getToken(string s) { // начало определения функции getToken, которая получает очередной токен из выражения
  readWhiteSpace(s); // пропускаем все пробелы
  string tok = ""; // инициализируем строковую переменную tok

  if (position == (int)s.length()) // если достигнут конец выражения, то возвращаем tok
    return tok;
  if (isdigit(s[position])) // если текущий символ - цифра, то возвращаем прочитанное целое число
    return readInteger(s);
  else // иначе возвращаем tok, содержащую текущий символ
    return tok += s[position++];
}

string readInteger(string s) { // функция считывает все цифры из строки s и возвращает их в виде строки
  string res = ""; // строка, которая будет содержать результат
  while (position < (int)s.length() && isdigit(s[position])) // пока не достигнут конец строки или символ, не являющийся цифрой
    res += s[position++]; // добавить символ к результату и перейти к следующему символу
  return res; // возвращаем полученную строку
}

void readWhiteSpace(string s) { // функция пропускает все пробельные символы
  while (position < (int)s.length() && isspace(s[position])) // пока не достигнут конец строки или символ, не являющийся пробелом
    position++; // перейти к следующему символу
}

int presenceVarCheck(string str) { // функция проверяет наличие переменной с именем str в списке переменных, и если ее нет, то добавляет в список новую переменную с именем str и адресом lastCell - variablesNum
  for (int i = 0; i < variablesNum; i++) { // для каждой переменной в списке переменных
    if (variables[i].getTit() == str) { // если имя переменной совпадает с искомым именем
      return variables[i].getAddr(); // возвращаем адрес переменной
    }
  }
  variables.emplace_back(Vari(str, lastCell - variablesNum)); // добавляем новую переменную в список
  variablesNum++; // увеличиваем количество переменных на 1
  return variables[variablesNum - 1].getAddr(); // возвращаем адрес только что добавленной переменной
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
  if (translate_bsc(argv[1], (char *)"temp_asmb.sa") == 0) {
    system(term_command.c_str());
    std::cout << "Трансляция в SimpleAssembler завершена!" << std::endl;
    std::cout << "Нажмите Enter для трансляции из SimpleAssembler в машинный код";
    std::cin.get();
    std::cout << std::endl;
    system("make sa");
   }
  return 0;
}
  