#include <iostream>

template <typename T> class ControlBlock {
  T *object;
  int shared_ptrs{};
  int weak_ptrs{};
};

template <typename T> class sharedPtr {
private:
  T *m_ptr;

public:
  sharedPtr(T *ptr = nullptr) {
    m_ptr = ptr;
    std::cout << "Initialized new shared pointer with value: " << *m_ptr
              << std::endl;
  }

  int use_count() { return }
};

int main() {
  sharedPtr<int> ptr1(new int(151));

  return 0;
}
