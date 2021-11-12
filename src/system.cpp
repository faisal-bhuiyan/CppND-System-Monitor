#include "system.h"

#include <cstddef>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
    this->cpu_ = Processor();

    vector<Process> procs;
    for (const auto& pid : LinuxParser::Pids()) {
        procs.emplace_back(Process(pid));
    }

    this->processes_ = procs;
}

System::System(Processor& cpu, std::vector<Process>& processes) :
    cpu_(cpu), processes_(processes) {
}

// Return the system's CPU
Processor& System::Cpu() {
    return cpu_;
}

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
    return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() {
    //std::cout << "LinuxParser::Kernel()" << LinuxParser::Kernel() << std::endl;
    return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// Return the operating system name
std::string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
}

// Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long System::UpTime() {
    return LinuxParser::UpTime();
}

