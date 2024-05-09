#include <array>
#include <iostream>
#include <memory>

template <size_t N>
class StackStorage {
 private:
  size_t top_ = 0;
  std::array<char, N> memory_;

 public:
  StackStorage() = default;
  ~StackStorage() = default;
  StackStorage(const StackStorage&) = delete;
  template <typename T>
  T* allocate(size_t count) {
    size_t space = N - top_;
    void* ptr = memory_.data() + top_;

    if (std::align(alignof(T), sizeof(T), ptr, space)) {
      T* ret_ptr = reinterpret_cast<T*>(ptr);
      top_ += (N - top_) - space;
      top_ += sizeof(T) * count;
      return ret_ptr;
    }

    return nullptr;
  }
};

template <typename T, size_t N>
class StackAllocator {
 private:
  StackStorage<N>* storage_;

 public:
  using value_type = T;

  StackAllocator() = default;
  StackAllocator(StackStorage<N>& stack_storage) : storage_(&stack_storage) {}
  inline ~StackAllocator() {}
  template <typename U>
  StackAllocator(const StackAllocator<U, N>& other) : storage_(other.get_storage()) {}
  inline StackAllocator& operator=(const StackAllocator& other) {
    storage_ = other.storage_;
    return *this;
  }

  T* allocate(size_t count) { return (*storage_).template allocate<T>(count); }

  void deallocate(T* ptr, size_t count) {
    std::ignore = ptr;
    std::ignore = count;
  }

  StackStorage<N>* get_storage() const { return storage_; }

  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* next;
    BaseNode* prev;
  };

  struct Node : public BaseNode {
    T value;
    explicit Node(const T& val) : value(val) {}
  };

  using AllocTraits = std::allocator_traits<Alloc>;
  using NodeAlloc = typename AllocTraits::template rebind_alloc<Node>;
  using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

  size_t size_;
  BaseNode fakeNode_;
  NodeAlloc alloc_;

  void add_node(BaseNode* node, BaseNode* previous) {
    node->prev = previous->prev;
    node->next = previous;

    previous->prev->next = node;
    previous->prev = node;
  }

 public:
  template <bool is_const = false>
  class ListIter {
   private:
    BaseNode* node_;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = typename std::conditional<is_const, const T&, T&>::type;
    using pointer = typename std::conditional<is_const, const T*, T*>::type;
    using difference_type = std::ptrdiff_t;

    ListIter(BaseNode* node) : node_(node) {}
    ListIter(const ListIter<false>& other) : node_(other.get_node()) {}
    ~ListIter() = default;

    ListIter& operator++() {
      node_ = node_->next;
      return *this;
    }
    ListIter operator++(int) {
      auto buff = *this;
      ++*this;
      return buff;
    }
    ListIter& operator--() {
      node_ = node_->prev;
      return *this;
    }
    ListIter operator--(int) {
      auto buff = *this;
      --*this;
      return buff;
    }

    template <bool is_other_const>
    bool operator==(const ListIter<is_other_const> other) {
      return node_ == other.node_;
    }
    template <bool is_other_const>
    bool operator!=(const ListIter<is_other_const> other) {
      return !(*this == other);
    }

    reference operator*() { return reinterpret_cast<Node*>(node_)->value; }
    pointer operator->() { return &reinterpret_cast<Node*>(node_)->value; }

    BaseNode* get_node() const { return node_; }
    ListIter<false> remove_const() { return ListIter<false>(node_); }
  };

  List() : size_(0), fakeNode_{&fakeNode_, &fakeNode_}, alloc_() {}
  explicit List(size_t size) : size_(size), fakeNode_{&fakeNode_, &fakeNode_}, alloc_() {
    Node* new_node;
    try {
      for (size_t i = 0; i < size_; ++i) {
        new_node = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, &new_node->value);
        add_node(new_node, &fakeNode_);
      }
    } catch (...) {
      NodeAllocTraits::deallocate(alloc_, new_node, 1);
      while (begin() != end()) {
        erase(begin());
      }
      throw;
    }
  }
  List(size_t size, const T& value) : size_(size), fakeNode_{&fakeNode_, &fakeNode_}, alloc_() {
    Node* new_node;
    try {
      for (size_t i = 0; i < size_; ++i) {
        new_node = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, &new_node->value, value);
        add_node(new_node, &fakeNode_);
      }
    } catch (...) {
      NodeAllocTraits::deallocate(alloc_, new_node, 1);
      while (begin() != end()) {
        erase(begin());
      }
      throw;
    }
  }
  explicit List(Alloc& allocator) : size_(0), fakeNode_{&fakeNode_, &fakeNode_}, alloc_(allocator) {}
  List(size_t size, Alloc& allocator) : size_(size), fakeNode_{&fakeNode_, &fakeNode_}, alloc_(allocator) {
    Node* new_node;
    try {
      for (size_t i = 0; i < size_; ++i) {
        new_node = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, &new_node->value);
        add_node(new_node, &fakeNode_);
      }
    } catch (...) {
      NodeAllocTraits::deallocate(alloc_, new_node, 1);
      while (begin() != end()) {
        erase(begin());
      }
      throw;
    }
  }
  List(size_t size, const T& value, Alloc& allocator)
      : size_(size), fakeNode_{&fakeNode_, &fakeNode_}, alloc_(allocator) {
    Node* new_node;
    try {
      for (size_t i = 0; i < size_; ++i) {
        new_node = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, &new_node->value, value);
        add_node(new_node, &fakeNode_);
      }
    } catch (...) {
      NodeAllocTraits::deallocate(alloc_, new_node, 1);
      while (begin() != end()) {
        erase(begin());
      }
      throw;
    }
  }
  List(const List& other)
      : size_(0),
        fakeNode_{&fakeNode_, &fakeNode_},
        alloc_(NodeAllocTraits::select_on_container_copy_construction(other.alloc_)) {
    const_iterator start = other.cbegin();
    const_iterator stop = other.cend();
    size_t other_size = other.size_;
    size_t created  = 0;
    Node* new_node;
    try {
      for (auto it = start; it != stop; ++it) {
        new_node = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, &new_node->value, *it);
        add_node(new_node, this->end().get_node());
        ++size_;
        ++created;
      }

      while (size_ > other_size) {
        erase(this->begin());
      }

      size_ = other_size;
    } catch (...) {
      NodeAllocTraits::deallocate(alloc_, new_node, 1);
      while (created > 0) {
        erase(--this->end());
        --created;
      }
      throw;
    }
  }

  List& operator=(const List& other) {
    if (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    } else {
      alloc_ = NodeAllocTraits::select_on_container_copy_construction(other.alloc_);
    }
    const_iterator start = other.cbegin();
    const_iterator stop = other.cend();
    size_t other_size = other.size_;
    size_t created = 0;
    Node* new_node;
    try {
      for (auto it = start; it != stop; ++it) {
        new_node = NodeAllocTraits::allocate(alloc_, 1);
        NodeAllocTraits::construct(alloc_, &new_node->value, *it);
        add_node(new_node, this->end().get_node());
        ++size_;
        ++created;
      }

      while (size_ > other_size) {
        erase(this->begin());
      }

      size_ = other_size;
    } catch (...) {
      NodeAllocTraits::deallocate(alloc_, new_node, 1);
      while (created > 0) {
        erase(--this->end());
        --created;
      }
      throw;
    }
    return *this;
  }
  ~List() {
    while (begin() != end()) {
      erase(begin());
    }
  }

  NodeAlloc get_allocator() const { return alloc_; }

  size_t size() const { return size_; }

  void push_back(const T& value) {
    insert(cend(), value);
  }
  void push_front(const T& value) {
    insert(cbegin(), value);
  }
  void pop_back() {
    erase(--cend());
  }
  void pop_front() {
    erase(cbegin());
  }

  using iterator = ListIter<false>;
  using const_iterator = ListIter<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() { return iterator(fakeNode_.next); }
  iterator end() { return iterator(&fakeNode_); }
  const_iterator begin() const { return const_iterator(fakeNode_.next); }
  const_iterator end() const { return const_iterator(const_cast<BaseNode*>(&fakeNode_)); }

  const_iterator cbegin() const { return const_iterator(fakeNode_.next); }
  const_iterator cend() const { return const_iterator(const_cast<BaseNode*>(&fakeNode_)); }

  reverse_iterator rbegin() { return std::reverse_iterator(end()); }
  reverse_iterator rend() { return std::reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const { return std::reverse_iterator(cend()); }
  const_reverse_iterator rend() const { return std::reverse_iterator(cbegin()); }

  const_reverse_iterator crbegin() const { return std::reverse_iterator(cend()); }
  const_reverse_iterator crend() const { return std::reverse_iterator(cbegin()); }

  void insert(const_iterator iter, const T& value) {
    Node* new_node = NodeAllocTraits::allocate(alloc_, 1);
    try {
      NodeAllocTraits::construct(alloc_, &new_node->value, value);
    } catch (...) {
      NodeAllocTraits::deallocate(alloc_, new_node, 1);
      throw;
    }
    add_node(new_node, iter.get_node());
    ++size_;
  }
  void erase(const_iterator iter) {
    iter.get_node()->prev->next = iter.get_node()->next;
    iter.get_node()->next->prev = iter.get_node()->prev;
    NodeAllocTraits::destroy(alloc_, reinterpret_cast<Node*>(iter.get_node()));
    NodeAllocTraits::deallocate(alloc_, reinterpret_cast<Node*>(iter.get_node()), 1);
    --size_;
  }
};
