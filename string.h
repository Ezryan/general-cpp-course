#include <cstring>
#include <iostream>

class String {
 private:
  size_t size_;
  size_t capacity_;
  char* string_;

  // Help-method
  void swap(String& other);
  // Allocation constructor
  String(size_t size);

 public:
  // Constructors
  String();
  String(const String& other);
  String(const char* cstyle);
  String(size_t len, char sym);
  String(char sym);

  // Operators
  String& operator=(const String& other);
  String& operator+=(char sym);
  String& operator+=(const String& other);
  char& operator[](size_t index);
  const char& operator[](size_t index) const;

  // Methods
  size_t length() const;
  size_t size() const;
  size_t capacity() const;
  void push_back(char sym);
  void pop_back();
  char& front();
  char& back();
  const char& front() const;
  const char& back() const;
  size_t find(const String& substring) const;
  size_t rfind(const String& substring) const;
  String substr(size_t start, size_t count) const;
  bool empty() const;
  void clear();
  void shrink_to_fit();
  char* data();
  const char* data() const;

  // Destructor
  ~String();
};


void String::swap(String& other) {
  std::swap(string_, other.string_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
}

String::String(size_t size) : size_(size + 1), capacity_(size_), string_(new char[size_]) {}

String::String(const char* cstyle) : String(strlen(cstyle)) {
  memcpy(string_, cstyle, size_);
}

String::String(size_t len, char sym) : String(len) {
  memset(string_, sym, len);
  string_[len] = '\0';
}

String::String() : size_(1), capacity_(2), string_(new char[2]) { string_[0] = '\0'; }

String::String(const String& other) : size_(other.size_), capacity_(other.capacity_),
                                                          string_(new char[capacity_]) {
  memcpy(string_, other.string_, size_);
}

String::String(char sym) : String(1, sym) {}

String& String::operator=(const String& other) {
  if (capacity_ >= other.size_) {
    memcpy(string_, other.string_, other.size_);
    size_ = other.size_;
    return *this;
  }
  String copy = other;
  swap(copy);
  return *this;
}

String& String::operator+=(char sym) {
  push_back(sym);
  return *this;
}

String& String::operator+=(const String& other) {
  if (capacity_ >= size_ + other.size_ - 1) {
    memcpy(string_ + (size_ - 1), other.string_, other.size_);
    size_ += other.size_ - 1;
    return *this;
  }
  char* buffer = new char[(size_ + other.size_ - 1) * 2];
  memcpy(buffer, string_, size_ - 1);
  memcpy(buffer + (size_ - 1), other.string_, other.size_);
  delete[] string_;
  string_ = buffer;
  size_ += other.size_ - 1;
  capacity_ = 2 * size_;
  return *this;
}

char& String::operator[](size_t index) { return string_[index]; }

const char& String::operator[](size_t index) const { return string_[index]; }

size_t String::length() const { return size_ - 1; }

size_t String::size() const { return size_ - 1; }

size_t String::capacity() const { return capacity_ - 1; }

void String::push_back(char sym) {
  if (size_ == capacity_) {
    char* buffer = new char[size_ * 2];
    memcpy(buffer, string_, size_);
    delete[] string_;
    string_ = buffer;
    capacity_ *= 2;
  }
  string_[size_ - 1] = sym;
  string_[size_] = '\0';
  ++size_;
}

void String::pop_back() {
  string_[size_ - 2] = '\0';
  --size_;
}

char& String::front() { return string_[0]; }

char& String::back() { return string_[size_ - 2]; }

const char& String::front() const { return string_[0]; }

const char& String::back() const { return string_[size_ - 2]; }

size_t String::find(const String& substring) const {
  for (size_t index = 0; index < length(); ++index) {
    if (index + substring.length() - 1 >= length()) {
      return length();
    }
    size_t buf_ind = 0;
    while (buf_ind < substring.length() && string_[index + buf_ind] == substring[buf_ind]) {
      ++buf_ind;
    }
    if (buf_ind == substring.length()) {
      return index;
    }
  }
  return length();
}

size_t String::rfind(const String& substring) const {
  for (int64_t index = size_ - 1 - substring.length(); index >= 0; --index) {
    size_t buf_ind = 0;
    while (buf_ind < substring.length() && string_[index + buf_ind] == substring[buf_ind]) {
      ++buf_ind;
    }
    if (buf_ind == substring.length()) {
      return index;
    }
  }
  return length();
}

String String::substr(size_t start, size_t count) const {
  String result(count);
  memcpy(result.string_, string_ + start, count);
  result.string_[count] = '\0';
  return result;
}

bool String::empty() const { return size_ == 1; }

void String::clear() {
  size_ = 1;
  string_[0] = '\0';
}

void String::shrink_to_fit() {
  char* buffer = new char[size_];
  memcpy(buffer, string_, size_);
  delete[] string_;
  string_ = buffer;
  capacity_ = size_;
}

char* String::data() { return string_; }

const char* String::data() const { return string_; }

String::~String() { delete[] string_; }

String operator+(const String& first, const String& second) {
  String result = first;
  result += second;
  return result;
}

String operator+(String& first, char sym) {
  String result = first;
  result += sym;
  return result;
}

std::ostream& operator<<(std::ostream& output, const String& string) {
  output << string.data();
  return output;
}

std::istream& operator>>(std::istream& input, String& string) {
  string = "";
  char buffer = ' ';
  while (buffer == ' ') {
    input.get(buffer);
  }
  string += buffer;
  while (input.get(buffer)) {
    if (std::isspace(buffer)) {
      break;
    }
    string.push_back(buffer);
  }
  return input;
}

bool operator==(const String& first, const String& second) {
  return strcmp(first.data(), second.data()) == 0;
}

bool operator!=(const String& first, const String& second) {
  return strcmp(first.data(), second.data()) != 0;
}

bool operator<(const String& first, const String& second) {
  return strcmp(first.data(), second.data()) < 0;
}

bool operator>(const String& first, const String& second) {
  return strcmp(first.data(), second.data()) > 0;
}

bool operator<=(const String& first, const String& second) {
  return strcmp(first.data(), second.data()) <= 0;
}

bool operator>=(const String& first, const String& second) {
  return strcmp(first.data(), second.data()) >= 0;
}
