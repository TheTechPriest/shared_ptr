#ifndef TESTS_CPP
#define TESTS_CPP

#include "weakPtr.h"

using namespace std::chrono_literals;

void printWeakPointerTest() {

  weakPtr<int> gw;

  auto observe = [&gw]() {
    std::cout << "gw.use_count() == " << gw.use_count() << "; ";
    // we have to make a copy of shared pointer before usage:
    if (!gw.expired()) {
      std::cout << "*shared == " << *gw.get() << '\n';
    } else
      std::cout << "gw is expired\n";
  };

  auto execute_test = [&gw, observe]() {
    {
      sharedPtr<int> sp{new int(100)};
      gw = sp;

      observe();
    }

    observe();
  };

  execute_test();
}

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
template <typename T>
concept SmartPointer = requires(T ptr) {
  { ptr.get() } -> std::convertible_to<bool>;
  { ptr.use_count() } -> std::convertible_to<long>;
};

void thr(SmartPointer auto p) {
  std::this_thread::sleep_for(987ms);
  auto lp = p;
  {
    static std::mutex io_mutex;
    std::lock_guard<std::mutex> lk(io_mutex);
    std::cout << "Local pointer in a thread: " << lp.get()
              << ", Use count: " << lp.use_count() << std::endl;
  }
}

void printConcurrencyTest() {

  std::cout << "===CONCURRENCY TEST===\n";
  std::cout << "Custom implementation: \n";

  {
    Timer timer;

    sharedPtr<int> ptr{new int(100)};

    std::cout << "Use count in main thread: " << ptr.use_count() << std::endl;

    // Here I'm forced to give it the explicit pointer type so it understands
    // which template to use at compile time
    std::thread t1{[&ptr]() { thr(ptr); }}, t2{[&ptr]() { thr(ptr); }},
        t3{[&ptr]() { thr(ptr); }};

    // Can use reset() to release ownership from main
    // ptr.reset();

    t1.join();
    t2.join();
    t3.join();
  }

  // Comparing with standard implementation
  std::cout << "Official implementation: \n";

  {
    Timer timer;

    std::shared_ptr<int> offPtr(new int(100));

    std::cout << "Use count in main thread: " << offPtr.use_count()
              << std::endl;

    std::thread offt1{[&offPtr]() { thr(offPtr); }},
        offt2{[&offPtr]() { thr(offPtr); }},
        offt3{[&offPtr]() { thr(offPtr); }};

    offt1.join();
    offt2.join();
    offt3.join();
  }

  // std::cout << "Use count in main thread: " << ptr.use_count() << std::endl;
}

#endif
