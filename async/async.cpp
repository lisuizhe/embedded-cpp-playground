#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int calculate(int x) {
  auto start = std::chrono::system_clock::now();
  std::cout << "Start calculation" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto delta = std::chrono::system_clock::now() - start;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(delta);
  std::cout << "Done in " << ms.count() << " ms" << std::endl;
  return x*x;
}

void test(int value, int worktime) {
  std::cout << "Request result of calculations for " << value << std::endl;
  std::future<int> fut = std::async(calculate, value);
  std::cout << "Keep working for " << worktime << " ms" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(worktime));
  auto start = std::chrono::system_clock::now();
  std::cout << "Waiting for result" << std::endl;
  int result = fut.get();
  auto delta = std::chrono::system_clock::now() - start;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(delta);

  std::cout << "Result is " << result << ", wait for " << ms.count() << " ms" << std::endl << std::endl;
}

int main() {
  test(5, 400);
  test(8, 1200);
  return 0;
}