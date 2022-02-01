#include <chrono>
#include <iostream>
#include <system_error>
#include <thread>

#include <unistd.h>
#include <poll.h>
#include <signal.h>

using namespace std::chrono_literals;

enum class Health : uint8_t {
  Ok,
  Unhealthy,
  Shutdown,
};

class HeartBeat {
  int channel[2];
  std::chrono::milliseconds delay;
public:
  HeartBeat(std::chrono::milliseconds delay) : delay(delay) {
    int rv = pipe(channel);
    if (rv < 0) {
      throw std::system_error(errno, std::system_category(), "Failed to open pipe");
    }
  }

  void Report(Health status) {
    int rv = write(channel[1], &status, sizeof(status));
    if (rv < 0) {
      throw std::system_error(errno, std::system_category(), "Failed to report health status");
    }
  }

  bool Monitor() {
    struct pollfd fds[1];
    fds[0].fd = channel[0];
    fds[0].events = POLLIN;
    bool takeover = true;
    bool polling = true;
    while (polling) {
      fds[0].revents = 0;
      int rv = poll(fds, 1, delay.count());
      if (rv) {
        if (fds[0].revents & (POLLERR | POLLHUP)) {
          std::cout << "Polling error occured" << std::endl;
          takeover = false;
          polling = false;
          break;
        }

        Health status;
        int count = read(fds[0].fd, &status, sizeof(status));
        if (count < sizeof(status)) {
          std::cout << "Failed to read heartbeat data" << std::endl;
          break;
        }
        switch (status) {
        case Health::Ok:
          std::cout << "Active process is healthy" << std::endl;
          break;
        case Health::Shutdown:
          std::cout << "Shut down signaled" << std::endl;
          takeover = false;
          polling = false;
          break;
        default:
          std::cout << "Unhealthy status reported" << std::endl;
          polling = false;
          break;
        }
      } else if (!rv) {
        std::cout << "Timeout" << std::endl;
        polling = false;
      } else {
        if (errno != EINTR) {
          std::cout << "Error reading heartbeat data, retrying" << std::endl;
        }
      }
    }
    return takeover;
  }
};

void Worker(HeartBeat& hb) {
  for (int i = 0; i < 5; i++) {
    hb.Report(Health::Ok);
    std::cout << "Processing" << std::endl;
    std::this_thread::sleep_for(100ms);
  }
  hb.Report(Health::Unhealthy);
}

int main() {
  HeartBeat hb(200ms);
  if (fork()) {
    if (hb.Monitor()) {
      std::cout << "Taking over" << std::endl;
      Worker(hb);
    }
  } else {
    Worker(hb);
  }
}