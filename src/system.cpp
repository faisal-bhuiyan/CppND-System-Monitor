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
        auto proc = Process(pid);

        if (proc.Pid() > 0 && proc.User() != string() && proc.Command() != string() &&
            proc.CpuUtilization() > 0. && proc.Ram() != string() && proc.UpTime() > 0) {
            procs.emplace_back(proc);
        }
    }

    std::sort(procs.begin(), procs.end());

    this->processes_ = procs;
}

System::System(Processor& cpu, std::vector<Process>& processes) :
    cpu_(cpu), processes_(processes) {
}

Processor& System::Cpu() {
    return cpu_;
}

vector<Process>& System::Processes() {
    return processes_;
}

std::string System::Kernel() {
    return LinuxParser::Kernel();
}

float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
}

int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

long System::UpTime() {
    return LinuxParser::UpTime();
}

