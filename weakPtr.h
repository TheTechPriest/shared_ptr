#ifndef WEAKPTR_H
#define WEAKPTR_H

#include "sharedPtr.h"

template <typename T> class weakPtr {
private:
  ControlBlock<T> *control;
  T *storedPtr;

public:
  weakPtr(sharedPtr<T> *newShared = nullptr) {
    if (newShared) {
      storedPtr = newShared->get();
    }
    if (storedPtr) {
      control = newShared->getControl();

      std::cout << "Initialized new weak pointer with value: " << storedPtr
                << std::endl;

      control->print_value();
      control->weak_ptrs++;

      return;
    }

    std::cout << "Initialized empty weak pointer\n";
    control = nullptr;
  }

  weakPtr(weakPtr<T> &wptr) {
    storedPtr = wptr.get();
    control = wptr.getControl();
    control->weak_ptrs++;
  }

  T *get() { return storedPtr; }
  ControlBlock<T> *getControl() { return control; }

  int use_count() { return control->shared_ptrs; }

  bool expired() { return (control->shared_ptrs == 0); }

  weakPtr<T> &operator*() { return *storedPtr; }
  weakPtr<T> *operator->() { return storedPtr; }

  void operator=(weakPtr wptr) {
    if (storedPtr != wptr.get()) {
      std::cout << "Pointing to different location" << std::endl;
      if (storedPtr && control) {
        control->weak_ptrs--;
      }

      storedPtr = wptr.get();
      if (storedPtr) {
        control = wptr.getControl();

        control->weak_ptrs++;
      }
    }
  }

  void operator=(sharedPtr<T> &sptr) {
    if (storedPtr != sptr.get()) {
      std::cout << "Pointing to different location" << std::endl;
      if (storedPtr && control) {
        control->weak_ptrs--;
      }

      storedPtr = sptr.get();

      // std::cout << "Shared value = " << shared << std::endl;
      if (storedPtr) {
        control = sptr.getControl();

        control->weak_ptrs++;
      }
    }
  }

  // Destructor
  ~weakPtr() {
    if (control)
      control->weak_ptrs--;
  }
};

#endif
