#include <iostream>

template <typename T> class ControlBlock {
public:
  T *object;
  int shared_ptrs{1};
  int weak_ptrs{0};

  ControlBlock(T *newObject) : object{newObject} {};

  void print_value() {
    std::cout << "Control block value: " << *object << std::endl;
  }
};

template <typename T> class sharedPtr {
private:
  T *m_ptr;
  ControlBlock<T> control;

public:
  sharedPtr(T *ptr = nullptr)
      : m_ptr{ptr}, control{ptr} { // Assigning default as nullptr if no element
                                   // is provided
    std::cout << "Initialized new shared pointer with value: " << *m_ptr
              << std::endl;

    control.print_value();
  }

  T *get() { return m_ptr; }

  int use_count() { return control.shared_ptrs; }
};

int main() {
  sharedPtr<int> ptr1(new int(151));

  return 0;
}
