#include <cstdio>
#include <cstring>
#include <iostream>

int size = 0;
char** stack;

void Size() { std::cout << size << std::endl; }
void Back() {
  if (size == 0) {
    std::cout << "error" << std::endl;
    return;
  }
  std::cout << stack[size - 1] << std::endl;
}
void Clear() {
  if (size != 0) {
    for (int i = 0; i < size; ++i) {
      delete[] stack[i];
    }
    delete[] stack;
    size = 0;
  }
  std::cout << "ok" << std::endl;
}
void Pop() {
  if (size == 0) {
    std::cout << "error" << std::endl;
    return;
  } else if (size == 1) {
    std::cout << stack[size - 1] << std::endl;
    delete[] stack[0];
    delete[] stack;
    --size;
    return;
  }
  std::cout << stack[size - 1] << std::endl;
  char** tmp = new char*[size - 1];
  for (int i = 0; i < size - 1; ++i) {
    tmp[i] = new char[strlen(stack[i]) + 1];
    strcpy(tmp[i], stack[i]);
  }
  for (int i = 0; i < size; ++i) {
    delete[] stack[i];
  }
  delete[] stack;
  stack = new char*[size - 1];
  for (int i = 0; i < size - 1; ++i) {
    stack[i] = new char[strlen(tmp[i]) + 1];
    strcpy(stack[i], tmp[i]);
  }
  for (int i = 0; i < size - 1; ++i) {
    delete[] tmp[i];
  }
  delete[] tmp;
  --size;
}
void Push(char* string) {
  if (size == 0) {
    stack = new char*[1];
    stack[0] = new char[strlen(string) + 1];
    strcpy(stack[0], string);
    std::cout << "ok" << std::endl;
    ++size;
    return;
  }
  char** tmp = new char*[size];
  for (int i = 0; i < size; ++i) {
    tmp[i] = new char[strlen(stack[i]) + 1];
    strcpy(tmp[i], stack[i]);
  }
  for (int i = 0; i < size; ++i) {
    delete[] stack[i];
  }
  delete[] stack;
  stack = new char*[size + 1];
  for (int i = 0; i < size; ++i) {
    stack[i] = new char[strlen(tmp[i]) + 1];
    strcpy(stack[i], tmp[i]);
  }
  stack[size] = new char[strlen(string) + 1];
  strcpy(stack[size], string);
  ++size;
  std::cout << "ok" << std::endl;
}

char* ReadInput() {
  char buffer;
  char* input = new char[1];
  input[0] = '\0';
  char* tmp;
  while(true) {
    buffer = getchar();
    if (buffer == '\n' || buffer == ' ') {
      break;
    }
    tmp = new char[strlen(input) + 2];
    std::memcpy(tmp, input, sizeof(char) * (strlen(input) + 1));
    tmp[strlen(input)] = buffer;
    tmp[strlen(input) + 1] = '\0';
    delete[] input;
    input = new char[strlen(tmp) + 1];
    std::memcpy(input, tmp, sizeof(char) * (strlen(tmp) + 1));
    delete[] tmp;
  }
  return input;
}

int main() {
  char* command = ReadInput();
  char* buffer;
  while (strcmp(command, "exit")) {
    if (!strcmp(command, "size")) {
      Size();
      delete[] command;
    } else if (!strcmp(command, "back")) {
      Back();
      delete[] command;
    } else if (!strcmp(command, "clear")) {
      Clear();
      delete[] command;
    } else if (!strcmp(command, "pop")) {
      Pop();
      delete[] command;
    } else if (!strcmp(command, "push")) {
      buffer = ReadInput();
      Push(buffer);
      delete[] buffer;
      delete[] command;
    }
    command = ReadInput();
  }
  std::cout << "bye" << std::endl;
  if (size != 0) {
    for (int i = 0; i < size; ++i) {
      delete[] stack[i];
    }
    delete[] stack;
  }
  delete[] command;
}