#include <cstdlib>
#include <iostream>

bool isDifferent(int* array, int size) {
  for (int i = 0; i < size - 1; ++i) {
    for (int j = i + 1; j < size; ++j) {
      if (array[i] == array[j]) {
        return false;
      }
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  int* length = new int[argc - 1];
  for (int i = 1; i < argc; ++i) {
    length[i - 1] = atoi(argv[i]);
  }
  int** arrays = new int*[argc - 1];
  for (int i = 0; i < argc - 1; ++i) {
    arrays[i] = new int[length[i]];
  }
  for (int i = 0; i < argc - 1; ++i) {
    for (int j = 0; j < length[i]; ++j) {
      std::cin >> arrays[i][j];
    }
  }
  long long answer = 0;
  long long buffer_product;
  int* index = new int[argc - 1];
  for (int i = 0; i < argc - 1; ++i) {
    index[i] = 0;
  }

  while (true) {
    if (index[argc - 2] == length[argc - 2]) {
      break;
    }
    if (isDifferent(index, argc - 1)) {
      buffer_product = 1;
      for (int i = 0; i < argc - 1; ++i) {
        buffer_product *= arrays[i][index[i]];
      }
      answer += buffer_product;
    }
    for (int i = 0; i < argc - 2; ++i) {
      if (i == 0) {
        ++index[i];
      }
      if (index[i] < length[i]) {
        break;
      }
      if (index[i] == length[i]) {
        index[i] = 0;
        ++index[i+1];
      }
    }
  }
  delete[] length;
  delete[] index;
  for (int i = 0; i < argc - 1; ++i) {
    delete[] arrays[i];
  }
  delete[] arrays;
  std::cout << answer << std::endl;
}