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

// Return this process's ID
int Process::Pid() {
    return this->pid_;
}

// Return this process's CPU utilization
float Process::CpuUtilization() {
    return LinuxParser::Cpu(this->pid_);
}

// Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(this->pid_);
}

// Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(this->pid_);
}

// Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(this->pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(this->pid_);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(const Process& other) const {
    if (this->cpu_usage_ < other.cpu_usage_) {
        return true;
    }

    return false;
}

