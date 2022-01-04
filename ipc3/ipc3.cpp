#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

std::string kQueueName = "/test";

class MessageQueue {
private:
  mqd_t handle;
public:
  MessageQueue(const std::string& name, int flags) {
    handle = mq_open(name.c_str(), flags);
    if (handle < 0) {
      throw std::runtime_error("Failed to open a queue for writing");
    }
  }

  MessageQueue(const std::string& name, int flags, int max_count, int max_size) {
    struct mq_attr attrs = { 0, max_count, max_size, 0 };
    handle = mq_open(name.c_str(), flags | O_CREAT, 0666, &attrs);
    if (handle < 0) {
      throw std::runtime_error("Failed to open a queue for writing");
    }
  }

  ~MessageQueue() {
    mq_close(handle);
  }

  void Send(const char* data, size_t len) {
    if (mq_send(handle, data, len, 0) < 0) {
      throw std::runtime_error("Failed to send a message");
    }
  }

  void Receive(char* data, size_t len) {
    if (mq_receive(handle, data, len, 0) < 0) {
      throw std::runtime_error("Failed to receive a message");
    }
  }
};

template<class T>
class Writer {
  private:
    MessageQueue queue;
  public:
    Writer(std::string& name):
      queue(name, O_WRONLY) {}

    void Write(const T& data) {
      queue.Send(reinterpret_cast<const char*>(&data), sizeof(data));
    }
};

template<class T>
class Reader {
private:
  MessageQueue queue;
public:
  Reader(std::string& name):
    queue(name, O_RDONLY) {}

  void Run() {
    T data;
    while(true) {
      queue.Receive(reinterpret_cast<char*>(&data), sizeof(data));
      Callback(data);
    }
  }
protected:
  virtual void Callback(const T& data) = 0;
};

struct Message {
  int x, y;
};

std::ostream& operator<<(std::ostream& o, const Message& m) {
  o << "(x=" << m.x << ", y=" << m.y << ")";
}

class CoordLogger : public Reader<Message> {
using Reader<Message>::Reader;

protected:
  void Callback(const Message& data) override {
    std::cout << "Received coordinate " << data << std::endl;
  }
};

void DoWrites() {
  std::vector<Message> messages {{1, 0}, {0, 1}, {1, 1}, {0, 0}};
  Writer<Message> writer(kQueueName);
  for (const auto& m : messages) {
    std::cout << "Write " << m << std::endl;
    writer.Write(m);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void DoReads() {
  CoordLogger logger(kQueueName);
  logger.Run();
}

int main(int argc, char** argv) {
  MessageQueue q(kQueueName, O_WRONLY, 10, sizeof(Message));
  pid_t pid = fork();
  if (pid) {
    DoWrites();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    kill(pid, SIGTERM);
  } else {
    DoReads();
  }
}