// #include <cstring>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <libjdb/libjdb.hpp>
#include <string_view>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <editline/readline.h>
#include <algorithm>
#include <sstream>
#include <vector>
#include <libjdb/process.hpp>
#include <libjdb/error.hpp>

namespace {
  std::unique_ptr<jdb::process> attach(int argc, const char** argv) {
    // Passing PID
    if(argc == 3 && argv[1] == std::string_view("-p")) {
      pid_t pid = std::atoi(argv[2]);
      return jdb::process::attach(pid);
    }

    // Passing program name
    else {
      const char *program_path = argv[1];
      return jdb::process::launch(program_path);
    }
  }

  void print_stop_reason(const jdb::process& process, jdb::stop_reason reason) {
    std::cout << "Process " << process.pid() << ' ';

    switch(reason.reason) {
      case jdb::process_state::exited:
        std::cout << "exited with status " << static_cast<int>(reason.info);
        break;
      case jdb::process_state::terminated:
        std::cout << "terminated with signal " << sigabbrev_np(reason.info);
        break;
      case jdb::process_state::stopped:
        std::cout << "stopped with signal " << sigabbrev_np(reason.info);
        break;
      case jdb::process_state::running:
        std::cout << "running state is by definition not stopped, this should never happen";
        break;
    }
    std::cout << std::endl;
  }

  std::vector<std::string> split(std::string_view str, char delimiter);
  bool is_prefix(std::string_view str, std::string_view of);

  void handle_command(std::unique_ptr<jdb::process>& process, std::string_view line) {
    auto args = split(line, ' ');
    auto command = args[0];

    if(is_prefix(command, "continue")) {
      process->resume();
      auto reason = process->wait_on_signal();
      print_stop_reason(*process, reason);
    }
    else {
      std::cerr << "Unknown command\n";
    }
  }

  std::vector<std::string> split(std::string_view str, char delimiter) {
    std::vector<std::string> out{};
    std::stringstream ss {std::string{str}};
    std::string item;

    while(std::getline(ss, item, delimiter)) {
      out.push_back(item);
    }
    return out;
  }

  bool is_prefix(std::string_view str, std::string_view of) {
    if(str.size() > of.size()) return false;
    return std::equal(str.begin(), str.end(), of.begin());
  }

  void main_loop(std::unique_ptr<jdb::process>& process) {
    char* line = nullptr;
    while((line = readline("jdb> ")) != nullptr) {
      std::string line_str;

      if(line == std::string_view("")) {
        free(line);
        if(history_length > 0) {
          line_str = history_list()[history_length - 1]->line;
        }
        else {
          line_str = line;
          add_history(line);
          free(line);
        }

        if(!line_str.empty()) {
          try {
            handle_command(process, line_str);
          }
          catch(const jdb::error& err) {
            std::cout << err.what() << '\n';
          }
        }
      }
    }
  }
}

int main(int argc, const char** argv) {
  if(argc == 1) {
    std::cerr << "No arguments given\n";
    return -1;
  }

  try {
    auto process = attach(argc, argv);
    main_loop(process);
  }
  catch (const jdb::error& err) {
    std::cout << err.what() << '\n';
  }
}
