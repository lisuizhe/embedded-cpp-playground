#include <iostream>
#include <stdint.h>

class Complex {
  char *ptr;
public:
  Complex(size_t size) noexcept {
    try {
      ptr = new(std::nothrow) char[size];
      if (ptr) {
        std::cout << "Successfully allocated " << size << " bytes" << std::endl;
      } else {
        std::cout << "Failed allocated " << size << " bytes" << std::endl;
      }
    } catch (...) {
      // do nothing
    }
  }

  ~Complex() {
    try {
      if (ptr) {
        delete[] ptr;
        std::cout << "Successfully deallocated " << std::endl;
      } else {
        std::cout << "Memory was not allocated" << std::endl;
      }
    } catch (...) {
      // do nothing
    }
  }

  bool IsValid() { return nullptr != ptr; }
};

Complex small(100);
Complex large(SIZE_MAX);
int main() {
  std::cout << "Small object is " 
            << (small.IsValid()? "valid" : "invalid")
            << std::endl;
  std::cout << "Large object is " 
            << (large.IsValid()? "valid" : "invalid")
            << std::endl;

  return 0;
}
