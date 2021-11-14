#include "process.h"

#include <cctype>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
    this->cpu_usage_ = LinuxParser::Cpu(pid_);
}

int Process::Pid() {
    return this->pid_;
}

float Process::CpuUtilization() {
    return LinuxParser::Cpu(this->pid_);
}

string Process::Command() {
    return LinuxParser::Command(this->pid_);
}

string Process::Ram() {
    return LinuxParser::Ram(this->pid_);
}

string Process::User() {
    return LinuxParser::User(this->pid_);
}

long int Process::UpTime() {
    return LinuxParser::UpTime() - LinuxParser::UpTime(this->pid_);
}

bool Process::operator<(const Process& other) const {
    return this->cpu_usage_ > other.cpu_usage_;
}

