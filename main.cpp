#include <atomic>
#include <iostream>
#include <memory>

// Custom Shared pointer implementation for learning purposes

template <typename T> class ControlBlock {
public:
  T *object;
  // Using atomic for safety during multithreading operations
  std::atomic<int> shared_ptrs{1};
  std::atomic<int> weak_ptrs{0};

  ControlBlock(T *newObject = nullptr) : object{newObject} {};

  void print_value() {
    std::cout << "Control block value: " << *object << std::endl;
  }

  void deleter() {
    // Using -- since it's properly overloaded in the= atomic class
    shared_ptrs--;
    if (shared_ptrs == 0) {
      delete object;
      if (weak_ptrs == 0) {
        delete this;
        return;
      }
    }
  }
};

template <typename T> class sharedPtr {
private:
  T *storedPtr;
  ControlBlock<T> *control;

public:
  // Constructor
  // Assigning default as nullptr if
  // no element is provided
  sharedPtr(T *ptr = nullptr)
      : storedPtr{ptr}, control{new ControlBlock<T>(ptr)} {
    std::cout << "Initialized new shared pointer with value: " << *storedPtr
              << std::endl;

    control->print_value();
  }

  // Destructor
  ~sharedPtr() { control->deleter(); }

  T *get() { return storedPtr; }

  int use_count() { return control.shared_ptrs; }
};

int main() {
  sharedPtr<int> ptr1(new int(151));

  return 0;
}
