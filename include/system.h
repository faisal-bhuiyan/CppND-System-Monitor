#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
public:
    System();
    System(Processor&, std::vector<Process>&);

    // Returns the system's CPU
    Processor& Cpu();

    // Returns a container composed of the system's processes
    std::vector<Process>& Processes();

    // Returns the system's memory utilization
    float MemoryUtilization();

    // Returns the number of seconds since the system started running
    long UpTime();

    // Returns the total number of processes on the system
    int TotalProcesses();

    // Returns the number of processes actively running on the system
    int RunningProcesses();

    // Returns the system's kernel identifier (string)
    std::string Kernel();

    // Returns the operating system name
    std::string OperatingSystem();

private:
    Processor cpu_;
    std::vector<Process> processes_;
};

#endif

