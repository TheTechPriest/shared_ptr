#ifndef WEAKPTR_H
#define WEAKPTR_H

#include "sharedPtr.h"

template <typename T> class weakPtr {
private:
  ControlBlock<T> *control;
  sharedPtr<T> *shared;

public:
  weakPtr(sharedPtr<T> *newShared = nullptr) : shared{newShared} {
    if (shared) {
      control = shared->getControl();

      std::cout << "Initialized new weak pointer with value: " << shared->get()
                << std::endl;

      control->print_value();
      control->weak_ptrs++;

      return;
    }

    std::cout << "Initialized empty weak pointer\n";
  }
};

#endif
