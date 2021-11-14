#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <iostream>
#include <regex>
#include <string>

namespace LinuxParser {
/*
* Paths
*/
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

/*
* System
*/
// Reads and returns the system memory utilization
float MemoryUtilization();

// Reads and returns the system uptime
long UpTime();

// Reads and returns the process Ids
// TODO: Update this to use std::filesystem
std::vector<int> Pids();

// Reads and returns the total number of processes
int TotalProcesses();

// Reads and returns the number of running processes
int RunningProcesses();

// Reads and returns the OS name
std::string OperatingSystem();

// Reads and returns the kernel name
std::string Kernel();

/*
* CPU
*/
enum CPUStates {
    kUser_ = 0,
    kNice_,
    kSystem_,
    kIdle_,
    kIOwait_,
    kIRQ_,
    kSoftIRQ_,
    kSteal_,
    kGuest_,
    kGuestNice_
};

// Reads and returns CPU utilization
float CpuUtilization();

// Reads and returns the number of jiffies for the system
long Jiffies();

// Reads and returns the number of active jiffies for a PID
long ActiveJiffies();

// Reads and returns the number of active jiffies for the system
long ActiveJiffies(int pid);

// Reads and returns the number of idle jiffies for the system
long IdleJiffies();

/*
* Processes
*/
// Reads and returns the command associated with a process
std::string Command(int pid);

// Reads and returns the Cpu used by a process
float Cpu(int pid);

// Reads and returns the memory used by a process
std::string Ram(int pid);

// Reads and returns the user ID associated with a process
std::string Uid(int pid);

// Reads and returns the user associated with a process
std::string User(int pid);

// Reads and returns the uptime of a process
long int UpTime(int pid);
};  // namespace LinuxParser

#endif

