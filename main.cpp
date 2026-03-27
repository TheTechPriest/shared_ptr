#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

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
    std::cout << "Initialized new shared pointer with value: " << *storedPtr
              << std::endl;

    control->print_value();
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
      if (storedPtr && *control) {
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

void printSharedPointerTest() {

  std::cout << "===REFERENCE COUNT TEST===\n";

  sharedPtr<int> ptr1(new int(151));

  std::cout << "Use count for ptr1: " << ptr1.use_count() << std::endl;

  sharedPtr<int> ptr2 = ptr1;

  // Use count increments
  std::cout << "Use count for ptr1: " << ptr1.use_count() << std::endl;
  std::cout << "Use count for ptr2: " << ptr2.use_count() << std::endl;

  {
    sharedPtr<int> ptr3(ptr2);

    std::cout << "Use count for ptr1: " << ptr1.use_count() << std::endl;
    std::cout << "Use count for ptr2: " << ptr2.use_count() << std::endl;
    std::cout << "Use count for ptr3: " << ptr3.use_count() << std::endl;
  }

  // ptr3 goes out of scope and is deallocated

  std::cout << "Use count for ptr1: " << ptr1.use_count() << std::endl;
  std::cout << "Use count for ptr2: " << ptr2.use_count() << std::endl;
}

// Mostly copied from the cppref shared_pointer article
void thr(sharedPtr<int> p) {
  std::this_thread::sleep_for(987ms);
  sharedPtr<int> lp = p;
  {
    static std::mutex io_mutex;
    std::lock_guard<std::mutex> lk(io_mutex);
    std::cout << "Local pointer in a thread: " << lp.get()
              << ", Use count: " << lp.use_count() << std::endl;
  }
}

void printConcurrencyTest() {

  std::cout << "===CONCURRENCY TEST===\n";

  sharedPtr<int> ptr(new int(100));

  std::cout << "Use count in main thread: " << ptr.use_count() << std::endl;
  std::thread t1{thr, std::ref(ptr)}, t2{thr, std::ref(ptr)},
      t3{thr, std::ref(ptr)};

  // Can use reset() to release ownership from main
  // ptr.reset();

  t1.join();
  t2.join();
  t3.join();

  std::cout << "Use count in main thread: " << ptr.use_count() << std::endl;
}

int main() {

  printSharedPointerTest();

  printConcurrencyTest();

  return 0;
}
