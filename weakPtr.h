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
    control = nullptr;
  }

  weakPtr(weakPtr<T> &wptr) {
    shared = wptr.get();
    control = wptr.getControl();
    control->weak_ptrs++;
  }

  T *get() { return shared->get(); }
  ControlBlock<T> *getControl() { return control; }

  int use_count() { return control->shared_ptrs; }

  bool expired() { return (control->shared_ptrs == 0); }

  weakPtr<T> &operator*() { return *shared; }
  weakPtr<T> *operator->() { return shared; }

  void operator=(weakPtr wptr) {
    if (shared != wptr.get()) {
      std::cout << "Pointing to different location" << std::endl;
      if (shared && control) {
        control->weak_ptrs--;
      }

      shared = wptr.get();
      if (shared) {
        control = wptr.getControl();

        control->weak_ptrs++;
      }
    }
  }

  void operator=(sharedPtr<T> *sptr) {
    if (shared != sptr) {
      std::cout << "Pointing to different location" << std::endl;
      if (shared && control) {
        control->weak_ptrs--;
      }

      shared = sptr;
      if (shared) {
        control = sptr->getControl();

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
