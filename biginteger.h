#include <cmath>
#include <iostream>
#include <string>
#include <vector>

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
  void swap(BigInteger& other) {
    std::swap(num_, other.num_);
    std::swap(sign_, other.sign_);
  }

  BigInteger();
  explicit BigInteger(const std::string& str);
  BigInteger(int value);

  std::string toString() const;

  explicit operator bool() const;

  BigInteger& operator+=(const BigInteger& other);
  BigInteger& operator-=(const BigInteger& other);
  BigInteger& operator*=(const BigInteger& other);
  BigInteger& operator/=(const BigInteger& other);
  BigInteger& operator%=(const BigInteger& other);

  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger& operator--();
  BigInteger operator--(int);
  BigInteger operator-() const;

  std::weak_ordering operator<=>(const BigInteger& other) const;
};

BigInteger operator""_bi(const char* str, size_t) {
  return BigInteger(std::string(str));
}

BigInteger operator""_bi(unsigned long long num) {
  return BigInteger(num);
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  result += second;
  return result;
}
BigInteger operator-(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  result -= second;
  return result;
}
BigInteger operator*(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  result *= second;
  return result;
}
BigInteger operator/(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  result /= second;
  return result;
}
BigInteger operator%(const BigInteger& first, const BigInteger& second) {
  BigInteger result = first;
  result %= second;
  return result;
}

bool operator==(const BigInteger& first, const BigInteger& second) {
  return first.toString() == second.toString();
}
bool operator!=(const BigInteger& first, const BigInteger& second) { return !(first == second); }

std::ostream& operator<<(std::ostream& output, const BigInteger& num) {
  output << num.toString();
  return output;
}
std::istream& operator>>(std::istream& input, BigInteger& num) {
  std::string buffer;
  input >> buffer;
  num = BigInteger(buffer);
  return input;
}

BigInteger::BigInteger() : num_(1, 0), sign_(Sign::zero) {}
BigInteger::BigInteger(const std::string& string) : sign_(Sign::zero) {
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

BigInteger& BigInteger::operator+=(const BigInteger& other) {
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
BigInteger& BigInteger::operator-=(const BigInteger& other) {
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
BigInteger& BigInteger::operator*=(const BigInteger& other) {
  if (other == 0) {
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
BigInteger& BigInteger::operator/=(const BigInteger& other) {
  if (*this == 0) {
    return *this;
  }
  if ((*this >= 0 ? *this : -*this) == (other >= 0 ? other : -other)) {
    *this = BigInteger("1");
    sign_ = (sign_ == other.sign_ ? Sign::plus : Sign::minus);
    return *this;
  }
  if ((*this >= 0 ? *this : -*this) < (other >= 0 ? other : -other)) {
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

      if (averege * copy > helper) {
        major_num = averege - 1;
      } else {
        digit = averege;
        less_num = averege + 1;
      }
    }
    helper -= copy * digit;
    result.num_.insert(result.num_.begin(), digit);
  }

  *this = result;
  sign_ = (sign_buffer == other.sign_ ? Sign::plus : Sign::minus);
  DeleteZeroes();
  return *this;
}
BigInteger& BigInteger::operator%=(const BigInteger& other) {
  Sign buf_sign = sign_;
  BigInteger copy = other;
  copy.sign_ = Sign::plus;
  sign_ = Sign::plus;
  *this -= (*this / copy) * copy;
  sign_ = buf_sign;
  DeleteZeroes();
  return *this;
}

BigInteger& BigInteger::operator++() {
  *this += "1"_bi;
  return *this;
}
BigInteger BigInteger::operator++(int) {
  BigInteger buffer = *this;
  ++*this;
  return buffer;
}
BigInteger& BigInteger::operator--() {
  *this -= 1;
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

std::weak_ordering BigInteger::operator<=>(const BigInteger& other) const {
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

  void swap(BigInteger& first, BigInteger& second) {
    BigInteger buffer = first;
    first.swap(second);
    second.swap(buffer);
  }
  BigInteger gcd(BigInteger& first, BigInteger& second) {
    while (second > 0) {
      first %= second;
      swap(first, second);
    }
    return first;
  }
  void DoMutuallyPrime() {
    BigInteger clone_denom = denominator_;
    BigInteger clone_numer = (numerator_ > 0) ? numerator_ : -numerator_;
    BigInteger buf = gcd(clone_numer, clone_denom);
    numerator_ /= buf;
    denominator_ /= buf;
  }
  void swap(Rational& other) {
    std::swap(numerator_, other.numerator_);
    std::swap(denominator_, other.denominator_);
  }

 public:
  Rational();
  Rational(int num);
  Rational(const BigInteger& value);

  std::weak_ordering operator<=>(const Rational &other) const;
  bool operator==(const Rational &other) const;
  bool operator!=(const Rational &other) const;


  Rational operator-() const;

  Rational& operator+=(const Rational &other);
  Rational& operator-=(const Rational &other);
  Rational& operator*=(const Rational &other);
  Rational& operator/=(const Rational &other);

  Rational& operator++();
  Rational operator++(int);
  Rational& operator--();
  Rational operator--(int);

  std::string toString() const;
  std::string asDecimal(size_t precision) const;

  explicit operator double() const;
};

Rational::Rational() : Rational(0) {}
Rational::Rational(int value) : Rational(BigInteger(value)) {}
Rational::Rational(const BigInteger& value) : numerator_(value), denominator_(1) {}

std::weak_ordering Rational::operator<=>(const Rational &other) const {
  if (numerator_.sign_ == BigInteger::Sign::plus && other.numerator_.sign_ == BigInteger::Sign::minus) {
    return std::weak_ordering::greater;
  }

  if (numerator_.sign_ == BigInteger::Sign::minus && other.numerator_.sign_ == BigInteger::Sign::plus) {
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
Rational& Rational::operator+=(const Rational &other) {
  numerator_ = numerator_ * other.denominator_ + denominator_ * other.numerator_;
  denominator_ *= other.denominator_;
  DoMutuallyPrime();
  return *this;
}
Rational& Rational::operator-=(const Rational &other) {
  numerator_ = numerator_ * other.denominator_ - denominator_ * other.numerator_;
  denominator_ *= other.denominator_;

  DoMutuallyPrime();
  return *this;
}
Rational& Rational::operator*=(const Rational &other) {
  numerator_ *= other.numerator_;
  denominator_ *= other.denominator_;
  DoMutuallyPrime();
  return *this;
}
Rational& Rational::operator/=(const Rational &other) {
  numerator_ *= other.denominator_;

  if (numerator_.sign_ != BigInteger::Sign::zero) {
    if ((numerator_ >= 0) == (other.numerator_ >= 0)) {
      numerator_.sign_ = BigInteger::Sign::plus;
    } else {
      numerator_.sign_ = BigInteger::Sign::minus;
    }
  }

  denominator_ *= other.numerator_;
  denominator_ *= (denominator_ >= 0 ? 1 : -1);
  DoMutuallyPrime();
  return *this;
}

Rational& Rational::operator++() {
  *this += 1;
  return *this;
}
Rational Rational::operator++(int) {
  Rational ret = *this;
  *this += 1;
  return ret;
}
Rational& Rational::operator--() {
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
  if (denominator_ != 1) {
    result += "/" + denominator_.toString();
  }
  return result;
}
std::string Rational::asDecimal(size_t precision) const {
  BigInteger numer_buffer = numerator_;
  for (size_t i = 0; i < precision; i++) {
    numer_buffer *= 10;
  }
  numer_buffer /= denominator_;

  std::string extra = numer_buffer.toString();

  if (extra.size() < precision) {
    if (extra[0] != '-') {
      extra = std::string(precision - extra.size(), '0') + extra.substr(0, extra.size());
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

Rational operator+(const Rational& first, const Rational& second) {
  Rational result = first;
  result += second;
  return result;
}
Rational operator-(const Rational& first, const Rational& second) {
  Rational result = first;
  result -= second;
  return result;
}
Rational operator*(const Rational& first, const Rational& second) {
  Rational result = first;
  result *= second;
  return result;
}
Rational operator/(const Rational& first, const Rational& second) {
  Rational result = first;
  result /= second;
  return result;
}
