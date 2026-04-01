#ifndef SHAREDPTR_H
#define SHAREDPTR_H

#include <atomic>
#include <concepts>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include "benchmark.cpp"

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
    // Using -- since it's properly overloaded in the atomic class
    shared_ptrs--;
    if (shared_ptrs == 0) {
      delete object;
      if (weak_ptrs == 0) {

        std::cout << "Deleting control block\n";
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

    if (storedPtr) {
      std::cout << "Initialized new shared pointer with value: " << *storedPtr
                << std::endl;

      control->print_value();
      return;
    }

    std::cout << "Initialized new empty shared pointer\n";
  }

  // Copy Constructor
  sharedPtr(sharedPtr<T> &sptr) {
    storedPtr = sptr.get();
    control = sptr.getControl();
    control->shared_ptrs++;
  }

  T *get() { return storedPtr; }
  ControlBlock<T> *getControl() { return control; }
  int use_count() { return control->shared_ptrs; }

  // Operator overloading

  void reset(T *ptr = nullptr) {
    control->deleter();

    storedPtr = ptr;

    if (storedPtr) {
      control = new ControlBlock<T>(storedPtr);
    }
  }

  T &operator*() { return *storedPtr; }
  T *operator->() { return storedPtr; }

  void operator=(sharedPtr sptr) {
    if (storedPtr != sptr.get()) {
      std::cout << "Pointing to different location" << std::endl;
      if (storedPtr && control) {
        control->deleter();
      }

      storedPtr = sptr.get();
      if (storedPtr) {
        control = sptr.getControl();

        control->shared_ptrs++;
      }
    }
  }

  // Destructor
  ~sharedPtr() { control->deleter(); }
};

#endif
