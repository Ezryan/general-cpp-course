#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

void Log(std::string msg) {
  std::cerr << msg << std::endl;
}

template <size_t N>
class Residue {
  static constexpr bool isPrime(size_t num);
  static constexpr bool prime_ = isPrime(N);
  static int Pow(int base, size_t exp);

 private:
  int remainder_ = 0;

 public:
  Residue() = default;
  Residue(int num);

  Residue<N>& operator+=(const Residue<N>& other);
  Residue<N>& operator-=(const Residue<N>& other);
  Residue<N>& operator*=(const Residue<N>& other);
  Residue<N>& operator/=(const Residue<N>& other);

  explicit operator int() const;

  std::weak_ordering operator<=>(const Residue<N> other) const {
    if (remainder_ == other.remainder_) {
      return std::weak_ordering::equivalent;
    }
    if (remainder_ > other.remainder_) {
      return std::weak_ordering::greater;
    }
    return std::weak_ordering::less;
  }
};

template <size_t N>
constexpr bool Residue<N>::isPrime(size_t num) {
  size_t d = 2;
  while (d * d <= num) {
    if (num % d == 0) {
      return false;
    }
    ++d;
  }
  return true;
}

template <size_t N>
int Residue<N>::Pow(int base, size_t exp) {
  if (exp == 0) {
    return 1;
  }
  if (exp % 2) {
    return (base % N) * (Pow(base % N, exp - 1));
  }
  return ((Pow(base % N, exp / 2) % N) * (Pow(base % N, exp / 2) % N)) % N;
}

template <size_t N>
Residue<N>::Residue(int num): remainder_((num % N + N) % N) {}

template <size_t N>
Residue<N>& Residue<N>::operator+=(const Residue<N>& other) {
  remainder_ = (remainder_ + other.remainder_) % N;
  return *this;
}

template <size_t N>
Residue<N>& Residue<N>::operator-=(const Residue<N>& other) {
  remainder_ = ((remainder_ - other.remainder_) % N + N) % N;
  return *this;
}

template <size_t N>
Residue<N>& Residue<N>::operator*=(const Residue<N>& other) {
  remainder_ = (remainder_ * other.remainder_) % N;
  return *this;
}

template <size_t N>
Residue<N>& Residue<N>::operator/=(const Residue<N>& other) {
  static_assert(prime_, "template parameter size_t N is not prime");
  Residue<N> reverse = Residue<N>(Pow(other.remainder_, N - 2));
  remainder_ = (remainder_ * reverse.remainder_) % N;
  return *this;
}

template <size_t N>
Residue<N>::operator int() const { return remainder_; }


template <size_t N>
Residue<N> operator+(const Residue<N>& first, const Residue<N>& second) {
  Residue<N> ret = first;
  ret += second;
  return ret;
}

template <size_t N>
Residue<N> operator-(const Residue<N>& first, const Residue<N>& second) {
  Residue<N> ret = first;
  ret -= second;
  return ret;
}

template <size_t N>
Residue<N> operator*(const Residue<N>& first, const Residue<N>& second) {
  Residue<N> ret = first;
  ret *= second;
  return ret;
}

template <size_t N>
Residue<N> operator/(const Residue<N>& first, const Residue<N>& second) {
  Residue<N> ret = first;
  ret /= second;
  return ret;
}
class BigInteger {
  friend class Rational;
  enum class Sign {
    minus,
    zero,
    plus
  };
 private:
  static const int kBase = 1e4;
  static const int kLength = 4;
  std::vector<int> num_;
  Sign sign_;
  std::string DoRightLength(std::string str) const {
    if (str.size() % kLength != 0) {
      str = std::string((kLength - str.size() % kLength) % kLength, '0') + str;
    }
    return str;
  }
  void DeleteZeroes() {
    while (num_.size() > 1 && num_.back() == 0) {
      num_.pop_back();
    }
    if (num_.size() == 1 && num_[0] == 0) {
      sign_ = Sign::zero;
    }
  }

 public:
  void swap(BigInteger &other) {
    std::swap(num_, other.num_);
    std::swap(sign_, other.sign_);
  }

  BigInteger();
  explicit BigInteger(const std::string &str);
  explicit BigInteger(int value);
  template<size_t N>
  BigInteger(Residue<N> r) : BigInteger(static_cast<int>(r)) {}

  std::string toString() const;

  explicit operator bool() const;

  BigInteger &operator+=(const BigInteger &other);
  BigInteger &operator-=(const BigInteger &other);
  BigInteger &operator*=(const BigInteger &other);
  BigInteger &operator/=(const BigInteger &other);
  BigInteger &operator%=(const BigInteger &other);

  BigInteger &operator++();
  BigInteger operator++(int);
  BigInteger &operator--();
  BigInteger operator--(int);
  BigInteger operator-() const;

  std::weak_ordering operator<=>(const BigInteger &other) const;
};

BigInteger operator ""_bi(const char *str, size_t) {
  return BigInteger(std::string(str));
}

BigInteger operator ""_bi(unsigned long long num) {
  return BigInteger(num);
}

BigInteger operator+(const BigInteger &first, const BigInteger &second) {
  BigInteger result = first;
  result += second;
  return result;
}
BigInteger operator-(const BigInteger &first, const BigInteger &second) {
  BigInteger result = first;
  result -= second;
  return result;
}
BigInteger operator*(const BigInteger &first, const BigInteger &second) {
  BigInteger result = first;
  result *= second;
  return result;
}
BigInteger operator/(const BigInteger &first, const BigInteger &second) {
  BigInteger result = first;
  result /= second;
  return result;
}
BigInteger operator%(const BigInteger &first, const BigInteger &second) {
  BigInteger result = first;
  result %= second;
  return result;
}

bool operator==(const BigInteger &first, const BigInteger &second) {
  return first.toString() == second.toString();
}
bool operator!=(const BigInteger &first, const BigInteger &second) { return !(first == second); }

std::ostream &operator<<(std::ostream &output, const BigInteger &num) {
  output << num.toString();
  return output;
}
std::istream &operator>>(std::istream &input, BigInteger &num) {
  std::string buffer;
  input >> buffer;
  num = BigInteger(buffer);
  return input;
}

BigInteger::BigInteger() : num_(1, 0), sign_(Sign::zero) {}
BigInteger::BigInteger(const std::string &string) : sign_(Sign::zero) {
  if (string == "0" || string.empty()) {
    num_.push_back(0);
    return;
  }
  std::string copy = string;
  std::string buffer;
  long long buf_ind;
  long long buf_len;
  if (copy[0] == '-') {
    sign_ = Sign::minus;
    copy.erase(0, 1);
  } else {
    sign_ = Sign::plus;
  }
  for (long long i = static_cast<long long>(copy.size()); i > 0; i -= kLength) {
    if (i <= kLength) {
      buf_ind = 0;
      buf_len = i;
    } else {
      buf_ind = i - kLength;
      buf_len = static_cast<long long>(kLength);
    }
    buffer = copy.substr(buf_ind, buf_len);
    num_.push_back(std::stoi(buffer));
  }
}
BigInteger::BigInteger(int value) {
  if (value == 0) {
    sign_ = Sign::zero;
    num_.push_back(0);
    return;
  }
  sign_ = (value > 0 ? Sign::plus : Sign::minus);
  value = (value > 0 ? value : -value);
  while (value > 0) {
    num_.push_back(value % kBase);
    value /= kBase;
  }
  DeleteZeroes();
}

std::string BigInteger::toString() const {
  if (sign_ == Sign::zero) {
    return "0";
  }
  std::string result = (sign_ == Sign::minus ? "-" : "");
  for (auto it = num_.rbegin(); it < num_.rend(); ++it) {
    if (it != num_.rbegin()) {
      result += DoRightLength(std::to_string(*it));
    } else {
      result += std::to_string(*it);
    }
  }
  return result;
}

BigInteger::operator bool() const {
  return sign_ != Sign::zero;
}

BigInteger &BigInteger::operator+=(const BigInteger &other) {
  if (other.sign_ == Sign::zero) {
    return *this;
  }

  if (sign_ == Sign::zero) {
    BigInteger cpy = other;
    swap(cpy);
    return *this;
  }

  if (other.sign_ != sign_) {

    *this -= -other;
    return *this;
  }
  bool add = false;
  for (size_t i = 0; add || i < num_.size() || i < other.num_.size(); ++i) {
    long long buf_result = 0;
    if (add) {
      buf_result = 1;
    }
    if (i < num_.size()) {
      buf_result += num_[i];
    }
    if (i < other.num_.size()) {
      buf_result += other.num_[i];
    }

    if (i < num_.size()) {
      num_[i] = buf_result % kBase;
    } else {
      num_.push_back(buf_result % kBase);
    }
    if (buf_result >= kBase) {
      add = true;
    } else {
      add = false;
    }
  }
  DeleteZeroes();
  return *this;
}
BigInteger &BigInteger::operator-=(const BigInteger &other) {
  if (other.sign_ == Sign::zero) {
    return *this;
  }

  if (other.sign_ != sign_) {
    *this += -other;
    return *this;
  }

  BigInteger copy = other;
  if (*this < copy && sign_ == Sign::plus) {

    swap(copy);
    sign_ = Sign::minus;
  } else if (*this >= copy && sign_ == Sign::minus) {

    swap(copy);
    sign_ = Sign::plus;
  }

  bool smth_wrong = false;
  for (size_t i = 0; smth_wrong || i < num_.size() || i < copy.num_.size(); ++i) {
    long long total = 0;
    long long extra = 0;
    if (smth_wrong) {
      extra = 1;
    }
    if (i < num_.size()) {
      total += num_[i];
    }
    if (i < copy.num_.size()) {
      extra += copy.num_[i];
    }
    long long buf_delta = total - extra;
    if (buf_delta >= 0) {
      smth_wrong = false;
    } else {
      smth_wrong = true;
      buf_delta += kBase;
    }
    if (i < num_.size()) {
      num_[i] = buf_delta;
    } else {
      num_.push_back(buf_delta);
    }
  }

  DeleteZeroes();
  return *this;
}
BigInteger &BigInteger::operator*=(const BigInteger &other) {
  if (other == "0"_bi) {
    *this = BigInteger("0");
    return *this;
  }

  BigInteger result(0);
  BigInteger copy = other;
  copy.sign_ = Sign::plus;
  Sign start_positive = sign_;
  sign_ = Sign::plus;
  long long buffer = 0;
  long long present;
  result.num_.resize(num_.size() + copy.num_.size());
  for (size_t i = 0; i < num_.size(); ++i) {
    for (size_t j = 0; (buffer != 0LL) || j < other.num_.size(); ++j) {
      present = buffer;
      if (j < copy.num_.size()) {
        present += result.num_[i + j] + (num_[i]) * 1LL * copy.num_[j];
      }

      result.num_[i + j] = present % kBase;
      buffer = present / kBase;
    }
  }
  result.sign_ = (start_positive == other.sign_ ? Sign::plus : Sign::minus);
  *this = result;
  DeleteZeroes();
  return *this;
}
BigInteger &BigInteger::operator/=(const BigInteger &other) {
  if (*this == "0"_bi) {
    return *this;
  }
  if ((*this >= "0"_bi ? *this : -*this) == (other >= "0"_bi ? other : -other)) {
    *this = BigInteger("1");
    sign_ = (sign_ == other.sign_ ? Sign::plus : Sign::minus);
    return *this;
  }
  if ((*this >= "0"_bi ? *this : -*this) < (other >= "0"_bi ? other : -other)) {
    *this = BigInteger("0");
    return *this;
  }

  Sign sign_buffer = sign_;

  BigInteger result(0);
  result.num_.resize(0);

  BigInteger copy = other;
  copy.sign_ = Sign::plus;

  BigInteger helper(0);
  helper.num_.resize(0);
  helper.sign_ = Sign::plus;

  sign_ = Sign::plus;

  for (size_t i = num_.size(); i != 0; --i) {
    int less_num = 0;
    int major_num = kBase;
    int digit = 0;

    helper.num_.insert(helper.num_.begin(), num_[i - 1]);
    helper.sign_ = Sign::plus;

    helper.DeleteZeroes();

    while (less_num <= major_num) {
      int averege = (major_num + less_num) / 2;

      if (BigInteger(averege) * copy > helper) {
        major_num = averege - 1;
      } else {
        digit = averege;
        less_num = averege + 1;
      }
    }
    helper -= copy * BigInteger(digit);
    result.num_.insert(result.num_.begin(), digit);
  }

  *this = result;
  sign_ = (sign_buffer == other.sign_ ? Sign::plus : Sign::minus);
  DeleteZeroes();
  return *this;
}
BigInteger &BigInteger::operator%=(const BigInteger &other) {
  Sign buf_sign = sign_;
  BigInteger copy = other;
  copy.sign_ = Sign::plus;
  sign_ = Sign::plus;
  *this -= (*this / copy) * copy;
  sign_ = buf_sign;
  DeleteZeroes();
  return *this;
}

BigInteger &BigInteger::operator++() {
  *this += "1"_bi;
  return *this;
}
BigInteger BigInteger::operator++(int) {
  BigInteger buffer = *this;
  ++*this;
  return buffer;
}
BigInteger &BigInteger::operator--() {
  *this -= "1"_bi;
  return *this;
}
BigInteger BigInteger::operator--(int) {
  BigInteger buffer = *this;
  --*this;
  return buffer;
}

BigInteger BigInteger::operator-() const {
  BigInteger result = *this;
  if (sign_ == Sign::minus) {
    result.sign_ = Sign::plus;
  } else if (sign_ == Sign::plus) {
    result.sign_ = Sign::minus;
  }
  return result;
}

std::weak_ordering BigInteger::operator<=>(const BigInteger &other) const {
  if (sign_ == Sign::minus && other.sign_ == Sign::plus) {
    return std::weak_ordering::less;
  }
  if (sign_ == Sign::plus && other.sign_ == Sign::minus) {
    return std::weak_ordering::greater;
  }
  if (sign_ == Sign::minus) {
    if (num_.size() < other.num_.size()) {
      return std::weak_ordering::greater;
    }
    if (num_.size() > other.num_.size()) {
      return std::weak_ordering::less;
    }
    for (long long i = num_.size() - 1; i >= 0; --i) {
      if (num_[i] > other.num_[i]) {
        return std::weak_ordering::less;
      }
      if (num_[i] < other.num_[i]) {
        return std::weak_ordering::greater;
      }
    }
  }
  if (num_.size() < other.num_.size()) {
    return std::weak_ordering::less;
  }
  if (num_.size() > other.num_.size()) {
    return std::weak_ordering::greater;
  }
  for (long long i = num_.size() - 1; i >= 0; --i) {
    if (num_[i] > other.num_[i]) {
      return std::weak_ordering::greater;
    }
    if (num_[i] < other.num_[i]) {
      return std::weak_ordering::less;
    }
  }
  return std::weak_ordering::equivalent;
}

class Rational {
  static const int default_precision = 15;
 private:
  BigInteger numerator_;
  BigInteger denominator_;

  void swap(BigInteger &first, BigInteger &second) {
    BigInteger buffer = first;
    first.swap(second);
    second.swap(buffer);
  }
  BigInteger gcd(BigInteger &first, BigInteger &second) {
    while (second > "0"_bi) {
      first %= second;
      swap(first, second);
    }
    return first;
  }
  void DoMutuallyPrime() {
    BigInteger clone_denom = denominator_;
    BigInteger clone_numer = (numerator_ > "0"_bi) ? numerator_ : -numerator_;
    BigInteger buf = gcd(clone_numer, clone_denom);
    numerator_ /= buf;
    denominator_ /= buf;
  }
  void swap(Rational &other) {
    std::swap(numerator_, other.numerator_);
    std::swap(denominator_, other.denominator_);
  }

 public:
  Rational();
  Rational(int num);
  Rational(const BigInteger &value);
//  template<size_t N>
//  Rational(Residue<N> r): Rational(static_cast<int>(r)) {}

  std::weak_ordering operator<=>(const Rational &other) const;
  bool operator==(const Rational &other) const;
  bool operator!=(const Rational &other) const;

  Rational operator-() const;

  Rational &operator+=(const Rational &other);
  Rational &operator-=(const Rational &other);
  Rational &operator*=(const Rational &other);
  Rational &operator/=(const Rational &other);

  Rational &operator++();
  Rational operator++(int);
  Rational &operator--();
  Rational operator--(int);

  std::string toString() const;
  std::string asDecimal(size_t precision) const;

  explicit operator double() const;
  explicit operator int() const {
    return std::stoi(numerator_.toString()) / std::stoi(denominator_.toString());
  }
};

std::istream &operator>>(std::istream &input, Rational &num) {
  std::string buf;
  input >> buf;
  num = Rational(BigInteger(buf));
  return input;
}

std::ostream &operator<<(std::ostream &output, Rational &num) {
  output << num.toString();
  return output;
}

Rational::Rational() : Rational(0) {}
Rational::Rational(int value) : Rational(BigInteger(value)) {}
Rational::Rational(const BigInteger &value) : numerator_(value), denominator_(1) {}

std::weak_ordering Rational::operator<=>(const Rational &other) const {
  if (numerator_.sign_ == BigInteger::Sign::plus &&
      other.numerator_.sign_ == BigInteger::Sign::minus) {
    return std::weak_ordering::greater;
  }

  if (numerator_.sign_ == BigInteger::Sign::minus &&
      other.numerator_.sign_ == BigInteger::Sign::plus) {
    return std::weak_ordering::less;
  }

  if (numerator_ * other.denominator_ > other.numerator_ * denominator_) {
    return std::weak_ordering::greater;
  }
  if (numerator_ * other.denominator_ == other.numerator_ * denominator_) {
    return std::weak_ordering::equivalent;
  }
  return std::weak_ordering::less;
}
bool Rational::operator==(const Rational &other) const {
  return (numerator_ == other.numerator_) && (denominator_ == other.denominator_);
}
bool Rational::operator!=(const Rational &other) const { return !(*this == other); }

Rational Rational::operator-() const {
  Rational buffer = *this;
  if (numerator_.sign_ != BigInteger::Sign::zero) {
    if (numerator_.sign_ == BigInteger::Sign::plus) {
      buffer.numerator_.sign_ = BigInteger::Sign::minus;
    } else {
      buffer.numerator_.sign_ = BigInteger::Sign::plus;
    }
  }

  return buffer;
}
Rational &Rational::operator+=(const Rational &other) {
  numerator_ = numerator_ * other.denominator_ + denominator_ * other.numerator_;
  denominator_ *= other.denominator_;
  DoMutuallyPrime();
  return *this;
}
Rational &Rational::operator-=(const Rational &other) {
  numerator_ = numerator_ * other.denominator_ - denominator_ * other.numerator_;
  denominator_ *= other.denominator_;

  DoMutuallyPrime();
  return *this;
}
Rational &Rational::operator*=(const Rational &other) {
  numerator_ *= other.numerator_;
  denominator_ *= other.denominator_;
  DoMutuallyPrime();
  return *this;
}
Rational &Rational::operator/=(const Rational &other) {
  numerator_ *= other.denominator_;

  if (numerator_.sign_ != BigInteger::Sign::zero) {
    if ((numerator_ >= "0"_bi) == (other.numerator_ >= "0"_bi)) {
      numerator_.sign_ = BigInteger::Sign::plus;
    } else {
      numerator_.sign_ = BigInteger::Sign::minus;
    }
  }

  denominator_ *= other.numerator_;
  denominator_ *= (denominator_ >= "0"_bi ? "1"_bi : "-1"_bi);
  DoMutuallyPrime();
  return *this;
}

Rational &Rational::operator++() {
  *this += 1;
  return *this;
}
Rational Rational::operator++(int) {
  Rational ret = *this;
  *this += 1;
  return ret;
}
Rational &Rational::operator--() {
  *this -= 1;
  return *this;
}
Rational Rational::operator--(int) {
  Rational ret = *this;
  *this -= 1;
  return ret;
}

std::string Rational::toString() const {
  std::string result = numerator_.toString();
  if (denominator_ != "1"_bi) {
    result += "/" + denominator_.toString();
  }
  return result;
}
std::string Rational::asDecimal(size_t precision) const {
  BigInteger numer_buffer = numerator_;
  for (size_t i = 0; i < precision; i++) {
    numer_buffer *= "10"_bi;
  }
  numer_buffer /= denominator_;

  std::string extra = numer_buffer.toString();

  if (extra.size() < precision) {
    if (extra[0] != '-') {
      extra =
          std::string(precision - extra.size(), '0') + extra.substr(0, extra.size());
    } else {
      std::string tmp;
      tmp = "-";
      tmp += std::string(precision - (extra.size() - 1), '0');
      tmp += extra.substr(1, extra.size() - 1);
      extra = tmp;
    }
  }

  std::string result = extra.substr(0, extra.size() - precision);
  if (result == "-" || result.size() == 0) {
    result += "0";
  }
  result += "." + extra.substr(extra.size() - precision, precision);
  return result;
}

Rational::operator double() const { return std::stod(asDecimal(default_precision)); }

Rational operator+(const Rational &first, const Rational &second) {
  Rational result = first;
  result += second;
  return result;
}
Rational operator-(const Rational &first, const Rational &second) {
  Rational result = first;
  result -= second;
  return result;
}
Rational operator*(const Rational &first, const Rational &second) {
  Rational result = first;
  result *= second;
  return result;
}
Rational operator/(const Rational &first, const Rational &second) {
  Rational result = first;
  result /= second;
  return result;
}


template <size_t M, size_t N, typename Field=Rational>
class Matrix {
  static constexpr bool is_square_ = M == N;

 private:
  std::array<std::array<Field, N>, M> matrix_;

  void DevideArr(size_t row, const Field& div);
  bool isZeroCol(size_t ind) const;
  Field Adjugate(size_t i, size_t j) const;

 public:
  Matrix();
  Matrix(const std::initializer_list<std::initializer_list<Field>>& list);

  static Matrix<M, N, Field> unityMatrix();

  Matrix<M, N, Field>& operator+=(const Matrix<M, N, Field>& other);
  Matrix<M, N, Field>& operator-=(const Matrix<M, N, Field>& other);
  Matrix<M, N, Field>& operator*=(const Field& other);

  Field det() const;
  Matrix<N, M, Field> transposed() const;
  size_t rank() const;
  Matrix<M, N, Field> inverted() const; // todo
  void invert();
  Field trace() const;
  std::array<Field, N> getRow(size_t ind) const;
  std::array<Field, M> getColumn(size_t ind) const;

  const std::array<Field, N>& operator[](size_t ind) const;
  std::array<Field, N>& operator[](size_t ind);

  bool operator==(Matrix<M, N, Field> other) const {
    for (size_t i = 0; i < M; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (matrix_[i][j] != other.matrix_[i][j]) {
          return false;
        }
      }
    }
    return true;
  }

  Matrix<M, N, Field>& operator*=(Matrix<M, N, Field>& other) {
    *this = *this * other;
    return *this;
  }
};

template <size_t M, size_t N, typename Field=Rational>
std::ostream& operator<<(std::ostream& output, Matrix<M, N, Field>& matrix) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      output << matrix[i][j] << ' ';
    }
    output << std::endl;
  }

  return output;
}

template <size_t M, size_t N, typename Field=Rational>
std::istream& operator>>(std::istream& input, Matrix<M, N, Field>& matrix) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      input >> matrix[i][j];
    }
  }

  return input;
}

template <size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::DevideArr(size_t row, const Field& div) {
  for (size_t col = 0; col < N; ++col) {
    matrix_[row][col] /= div;
  }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix() {
  Log("Matrix()");
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      matrix_[i][j] = Field(0);
    }
  }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix(const std::initializer_list<std::initializer_list<Field>>& list) :
                     Matrix() {
  Log("Matrix({})");
  size_t row =  0;
  for (const auto& rowList : list) {
    size_t col =  0;
    for (const auto& val : rowList) {
      matrix_[row][col] = val;
      col++;
    }
    row++;
  }
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::unityMatrix() {
  Log("unityMatrix()");
  static_assert(is_square_, "the matrix is not square");
  Matrix<M, N, Field> ret;
  for (size_t i = 0; i < N; ++i) {
    ret[i][i] = Field(1);
  }

  return ret;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator+=(const Matrix<M, N, Field>& other) {
  Log("+=");
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      matrix_[i][j] += other.matrix_[i][j];
    }
  }
  return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator-=(const Matrix<M, N, Field>& other) {
  Log("-=");
  *this += Field(-1) * other;
  return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Field& other) {
  Log("*=F");
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      matrix_[i][j] *= other;
    }
  }
  return *this;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator+(const Matrix<M, N, Field>& matrix_f,
                              const Matrix<M, N, Field>& matrix_s) {
  auto ret = matrix_f;
  ret += matrix_s;
  return ret;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator-(const Matrix<M, N, Field>& matrix_f,
                              const Matrix<M, N, Field>& matrix_s) {
  auto ret = matrix_f;
  ret -= matrix_s;
  return ret;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> matrix, const Field& multiplier) {
  auto ret = matrix;
  ret *= multiplier;
  return ret;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(const Field& multiplier, Matrix<M, N, Field> matrix) {
  auto ret = matrix;
  ret *= multiplier;
  return ret;
}

template <size_t M, size_t N, size_t K, typename Field>
Matrix<M, K, Field> operator*(const Matrix<M, N, Field>& matrix_f,
                              const Matrix<N, K, Field>& matrix_s) {
  Log("*MM");
  Matrix<M, K, Field> ret;
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < K; ++j) {
      for (size_t t = 0; t < N; ++t) {
        ret[i][j] += matrix_f[i][t] * matrix_s[t][j];
      }
    }
  }

  return ret;
}

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::det() const {
  Log("det");
  static_assert(is_square_, "the matrix is not square");
  /*
  Field determinant(1);
  Matrix<M, N, Field> buf_matr = *this;

  for (size_t i = 0; i < M; ++i) {
    size_t max_row = i;
    for (size_t j = i + 1; j < M; ++j) {
      if (std::abs(static_cast<int>(buf_matr[j][i])) >
          std::abs(static_cast<int>(buf_matr[max_row][i]))) {
        max_row = j;
      }
    }

    if (buf_matr[max_row][i] == Field(0)) {
      return Field(0);
    }

    if (max_row != i) {
      determinant *= Field(-1);
      std::swap(buf_matr[max_row], buf_matr[i]);
    }

    Field el = buf_matr[i][i];
    determinant *= el;
    for (size_t under_row = i + 1; under_row < M; ++under_row) {
      Field num = buf_matr[under_row][i] / el;
      for (size_t k = i; k < N; ++k) {
        buf_matr[under_row][k] -= num * buf_matr[i][k];
      }
    }
  }

  Log("end");

  return determinant;
   */
  if (rank() != M) {
    return Field(0);
  }

  Matrix<M, N, Field> matrix = *this;

  Field ret(1);
  for (size_t row = 0; row < M; ++row) {
    link:
    if (matrix[row][row] != Field(0)) {
      for (size_t i = row + 1; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
          matrix[i][j] -= matrix[row][i] / matrix[row][row] * matrix[i][row];
        }
      }
      continue;
    }

    for (size_t i = row + 1; i < M; ++i) {
      if (matrix[i][row] != Field(0)) {
        ret *= Field(-1);
        std::swap(matrix[i], matrix[row]);
        goto link;
      }
    }
  }

  for (size_t i = 0; i < M; ++i) {
    ret *= matrix[i][i];
  }

  return ret;
}

template <size_t M, size_t N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
  Log("transp");
  Matrix<N, M, Field> ret;
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      ret[j][i] = matrix_[i][j];
    }
  }

  return ret;
}

template <size_t M, size_t N, typename Field>
size_t Matrix<M, N, Field>::rank() const {
  Log("rank");
  size_t ret = 0;
  Matrix<M, N, Field> buffer = *this;

  Field buf;
  size_t row;

  for (size_t col = 0; col < N; ++col) {
    if (buffer.isZeroCol(col)) {
      continue;
    }

    for (size_t i = 0; i < M; ++i) {
      if (buffer[i][col] == Field(0)) {
        continue;
      }

      buf = buffer[i][col];
      row = i;
      break;
    }

    buffer.DevideArr(row, buf);

    for (size_t i = 0; i < M; ++i) {
      if (i == row) {
        continue;
      }
      for (size_t j = 0; j < N; ++j) {
        buffer[i][j] -= buffer[row][j] * buffer[i][col];
      }
    }
    ++ret;
  }

  return ret;
}

template <size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
  Log("inverted");
  static_assert(is_square_, "the matrix is not square");
  Field determ = det();
  if (determ == Field(0)) {
//    throw std::runtime_error("Matrix is singular (determinant is zero), cannot be inverted.");
    exit(-1);
  }

  Matrix<M, N, Field> inverseMatrix;
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      inverseMatrix[i][j] = Adjugate(i, j) / determ * Field((i + j) % 2 ? -1 : 1);
    }
  }

  return inverseMatrix.transposed();
}

template <size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::invert() { *this = inverted(); }

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::trace() const {
  Log("trace");
  static_assert(is_square_, "the matrix is not square");
  Field ans = Field(0);
  for (size_t i = 0; i < N; ++i) {
    ans += matrix_[i][i];
  }

  return ans;
}

template <size_t M, size_t N, typename Field>
std::array<Field, N> Matrix<M, N, Field>::getRow(size_t ind) const {
  Log("getrow");
  return matrix_[ind];
}

template <size_t M, size_t N, typename Field>
std::array<Field, M> Matrix<M, N, Field>::getColumn(size_t ind) const {
  Log("getcol");
  std::array<Field, M> ret;
  for (size_t row = 0; row < M; ++row) {
    ret[row] = matrix_[row][ind];
  }
  return ret;
}

template <size_t M, size_t N, typename Field>
const std::array<Field, N>& Matrix<M, N, Field>::operator[](size_t ind) const {
//  Log("[] c");
  return matrix_[ind];
}

template <size_t M, size_t N, typename Field>
std::array<Field, N>& Matrix<M, N, Field>::operator[](size_t ind) {
//  Log("[]");
  return matrix_[ind]; }

template <size_t M, size_t N, typename Field>
bool Matrix<M, N, Field>::isZeroCol(size_t ind) const {
  auto buf = getColumn(ind);
  for (auto el: buf) {
    if (el != Field(0)) {
      return false;
    }
  }

  return true;
}

template <size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::Adjugate(size_t i, size_t j) const {
  Matrix<M - 1, N - 1, Field> buf;
  for (size_t row = 0; row < i; ++row) {
    for (size_t col = 0; col < j; ++col) {
      buf[row][col] = matrix_[row][col];
    }

    for (size_t col = j + 1; col < N; ++col) {
      buf[row][col - 1] = matrix_[row][col];
    }
  }
  for (size_t row = i + 1; row < M; ++row) {
    for (size_t col = 0; col < j; ++col) {
      buf[row - 1][col] = matrix_[row][col];
    }

    for (size_t col = j + 1; col < N; ++col) {
      buf[row - 1][col - 1] = matrix_[row][col];
    }
  }
  return buf.det();
}

template <size_t N, typename Field=Rational>
using SquareMatrix = Matrix<N, N, Field>;

// ce ne srabotal
