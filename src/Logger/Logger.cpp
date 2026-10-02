#include "Logger.h"
#include <ctime>
#include <iostream>
#include <string>

const std::string green("\033[0;32m");
const std::string red("\033[0;31m");
const std::string reset("\033[0m");

std::string helper(const std::string &message) {
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, 80, "%d %b %G %T", timeinfo);

  return buffer;
}

void Logger::Log(const std::string &message) {
  std::cout << green;
  std::cout << "LOG | ";
  std::cout << helper(message);
  std::cout << " - ";
  std::cout << message << std::endl << reset;
}

void Logger::Err(const std::string &message) {
  std::cerr << red;
  std::cerr << "ERR | ";
  std::cerr << helper(message);
  std::cerr << " - ";
  std::cerr << message << std::endl << reset;
}
