#include <chrono>
#include <iostream>
#include <thread>

#include <unistd.h>

using namespace std::chrono_literals;

class WatchDog {
  std::chrono::seconds seconds;

public:
  WatchDog(std::chrono::seconds seconds) : seconds(seconds) {
    feed();
  }

  ~WatchDog() {
    alarm(0);
  }

  void feed() {
    alarm(seconds.count());
  }
};

int main() {
  WatchDog watchdog(2s);
  std::chrono::milliseconds delay = 700ms;
  for (int i = 0; i < 10; i++) {
    watchdog.feed();
    std::cout << delay.count() << "ms delay" << std::endl;
    std::this_thread::sleep_for(delay);
    delay += 300ms;
  }
}