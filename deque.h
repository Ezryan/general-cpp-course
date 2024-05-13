#include <iostream>
#include <iterator>

template <typename T>
class Deque {
  static const size_t kInterSize = 16;
 public:
  template <bool isConstant>
  class DequeIterator {
   public:
    friend class Deque;

    using reference = typename std::conditional<isConstant, const T&, T&>::type;
    using pointer = typename std::conditional<isConstant, const T*, T*>::type;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    DequeIterator() : block_(0), index_(0), deque_(nullptr) {}
    DequeIterator(size_t block, size_t index, const Deque<T>* deque) : block_(block), index_(index), deque_(deque) {}
    DequeIterator(const DequeIterator<false>& other)
        : block_(other.block_), index_(other.index_), deque_(other.deque_) {}

    operator DequeIterator<true>() { return DequeIterator<true>(block_, index_, deque_); }

    DequeIterator<false> remove_const() const { return DequeIterator<false>(block_, index_, deque_); }

    DequeIterator& operator+=(ptrdiff_t diff) {
      int64_t pos = block_ * kInterSize + index_;
      pos += diff;
      index_ = pos % kInterSize;
      block_ = pos / kInterSize;
      return *this;
    }

    DequeIterator& operator-=(ptrdiff_t diff) {
      *this += -diff;
      return *this;
    }

    DequeIterator& operator++() {
      *this += 1;
      return *this;
    }

    DequeIterator& operator--() {
      *this -= 1;
      return *this;
    }

    DequeIterator operator++(int) {
      DequeIterator copy = *this;
      *this += 1;
      return copy;
    }

    DequeIterator operator--(int) {
      DequeIterator copy = *this;
      *this -= 1;
      return copy;
    }

    DequeIterator operator+(ptrdiff_t diff) const {
      DequeIterator copy = *this;
      copy += diff;
      return copy;
    }

    DequeIterator operator-(ptrdiff_t diff) const {
      DequeIterator copy = *this;
      copy -= diff;
      return copy;
    }

    ptrdiff_t operator-(const DequeIterator& other) const {
      return (block_ - other.block_) * kInterSize + index_ - other.index_;
    }

    bool operator<(const DequeIterator& other) const {
      return (block_ * kInterSize + index_) < (other.block_ * kInterSize + other.index_);
    }

    bool operator>(const DequeIterator& other) const { return other < *this; }

    bool operator==(const DequeIterator& other) const { return !(*this < other || other < *this); }

    bool operator!=(const DequeIterator& other) const { return !(*this == other); }

    bool operator<=(const DequeIterator& other) const { return *this < other || *this == other; }

    bool operator>=(const DequeIterator& other) const { return *this > other || *this == other; }

    reference operator*() const { return deque_->outer_[block_][index_]; }

    pointer operator->() const { return &(deque_->outer_[block_][index_]); }

   private:
    int64_t block_;
    int64_t index_;
    const Deque<T>* deque_;
  };

  using iterator = DequeIterator<false>;
  using const_iterator = DequeIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque() : start_(0), size_(0), outer_(1) {
    outer_[0] = reinterpret_cast<T*>(new char[kInterSize * sizeof(T)]);
  }

  explicit Deque(size_t size)
      : start_(0),
        size_(size),
        outer_(size / kInterSize + 1) {
    for (size_t i = 0; i < outer_.size(); ++i) {
      outer_[i] = reinterpret_cast<T*>(new char[kInterSize * sizeof(T)]);
    }
    for (size_t i = 0; i < size; ++i) {
      new(outer_[i / kInterSize] + i % kInterSize) T();
    }
  }

  Deque(size_t size, const T& value)
      : start_(0),
        size_(size),
        outer_(size / kInterSize + 1) {
    for (size_t i = 0; i < outer_.size(); ++i) {
      outer_[i] = reinterpret_cast<T*>(new char[kInterSize * sizeof(T)]);
    }
    for (size_t i = 0; i < size; ++i) {
      new(outer_[i / kInterSize] + i % kInterSize) T(value);
    }
  }

  Deque(const Deque& other)
      : start_(0),
        size_(other.size_),
        outer_(other.size_ / kInterSize + 1) {
    for (size_t i = 0; i < outer_.size(); ++i) {
      outer_[i] = reinterpret_cast<T*>(new char[kInterSize * sizeof(T)]);
    }
    for (size_t i = 0; i < size_; ++i) {
      new(outer_[i / kInterSize] + i % kInterSize) T(other[i]);
    }
  }

  ~Deque() {
    for (size_t i = 0; i < size_; ++i) {
      (outer_[(start_ + i) / kInterSize] + (start_ + i) % kInterSize)->~T();
    }
    for (size_t i = 0; i < outer_.size(); ++i) {
      delete[] reinterpret_cast<char*>(outer_[i]);
    }
  }

  Deque& operator=(const Deque& other) {
    Deque copy(other);
    for (size_t i = 0; i < size_; ++i) {
      (outer_[(start_ + i) / kInterSize] + (start_ + i) % kInterSize)->~T();
    }
    for (size_t i = 0; i < outer_.size(); ++i) {
      delete[] reinterpret_cast<char*>(outer_[i]);
    }
    start_ = 0;
    size_ = copy.size_;
    outer_.resize(copy.size_ / kInterSize + 1);
    for (size_t i = 0; i < outer_.size(); ++i) {
      outer_[i] = reinterpret_cast<T*>(new char[kInterSize * sizeof(T)]);
    }
    for (size_t i = 0; i < size_; ++i) {
      new(outer_[i / kInterSize] + i % kInterSize) T(copy[i]);
    }
    return *this;
  }

  T& operator[](size_t index) {
    index += start_;
    return outer_[index / kInterSize][index % kInterSize];
  }

  const T& operator[](size_t index) const {
    index += start_;
    return outer_[index / kInterSize][index % kInterSize];
  }

  T& at(size_t index) {
    if (index >= size_) {
      throw std::out_of_range("Index is out of range");
    }
    return (*this)[index];
  }

  const T& at(size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("Index is out of range");
    }
    return (*this)[index];
  }

  [[nodiscard]] size_t size() const { return size_; }

  void push_back(const T& value) {
    if (start_ + size_ == outer_.size() * kInterSize) {
      reallocate();
    }
    new(outer_[(start_ + size_) / kInterSize] + (start_ + size_) % kInterSize) T(value);
    ++size_;
  }

  void pop_back() {
    (outer_[(start_ + size_ - 1) / kInterSize] + (size_ - 1) % kInterSize)->~T();
    --size_;
  }

  void push_front(const T& value) {
    if (start_ == 0) {
      reallocate();
    }
    --start_;
    ++size_;
    new(outer_[start_ / kInterSize] + start_ % kInterSize) T(value);
  }

  void pop_front() {
    (outer_[start_ / kInterSize] + start_ % kInterSize)->~T();
    ++start_;
    --size_;
  }

  iterator begin() {
    return iterator(start_ / kInterSize, start_ % kInterSize, this);
  }

  const_iterator begin() const {
    return const_iterator(start_ / kInterSize, start_ % kInterSize, this);
  }

  const_iterator cbegin() const {
    return const_iterator(start_ / kInterSize, start_ % kInterSize, this);
  }

  reverse_iterator rbegin() {
    return std::reverse_iterator(end());
  }

  const_reverse_iterator rbegin() const {
    return std::reverse_iterator(cend());
  }

  const_reverse_iterator crbegin() const {
    return std::reverse_iterator(cend());
  }

  iterator end() {
    return iterator((start_ + size_) / kInterSize, (start_ + size_) % kInterSize, this);
  }

  const_iterator end() const {
    return const_iterator((start_ + size_) / kInterSize, (start_ + size_) % kInterSize, this);
  }

  const_iterator cend() const {
    return const_iterator((start_ + size_) / kInterSize, (start_ + size_) % kInterSize, this);
  }

  reverse_iterator rend() {
    return std::reverse_iterator(begin());
  }

  const_reverse_iterator rend() const {
    return std::reverse_iterator(cbegin());
  }

  const_reverse_iterator crend() const {
    return std::reverse_iterator(cbegin());
  }

  void insert(iterator iter, const T& value) {
    if (iter == begin()) {
      push_front(value);
      return;
    }
    if (iter == end()) {
      push_back(value);
      return;
    }
    push_back(value);
    reverse_iterator rit = std::reverse_iterator(iter);
    for (reverse_iterator from = rbegin() + 1, to = rbegin(); from <= rit;
         ++from, ++to) {
      new(&(*to)) T(*from);
    }
    new(&(*iter)) T(value);
  }

  void insert(const_iterator iter, const T& value) {
    insert(iter.remove_const(), value);
  }

  void erase(const iterator& iter) {
    if (iter == begin()) {
      pop_front();
      return;
    }
    if (iter == end() - 1) {
      pop_back();
      return;
    }
    iter->~T();
    for (iterator from = iter + 1, to = iter; from != end(); ++from, ++to) {
      new(&(*(to))) T(*from);
    }
    (end() - 1)->~T();
    --size_;
  }

  void erase(const_iterator citer) {
    erase(citer.remove_const());
  }

 private:
  void reallocate() {
    std::vector<T*> new_out(3 * outer_.size());
    for (size_t i = 0; i < new_out.size(); ++i) {
      if (i < outer_.size() || i >= 2 * outer_.size()) {
        new_out[i] = reinterpret_cast<T*>(new char[kInterSize * sizeof(T)]);
      } else {
        new_out[i] = outer_[i - outer_.size()];
      }
    }
    start_ += kInterSize * outer_.size();
    outer_ = new_out;
  }

  size_t start_;
  size_t size_;
  std::vector<T*> outer_;
};
