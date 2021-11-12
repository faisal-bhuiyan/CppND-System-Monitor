#include <dirent.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// Read and return the OS name
string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);

    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);

            while (linestream >> key >> value) {
                if (key == "PRETTY_NAME") {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }

    return value;
}

// Read and return the kernel name
string LinuxParser::Kernel() {
    string os, kernel, version;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }

    return kernel;
}

// Read and return the process Ids
// TODO: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
    vector<int> pids;
    DIR* directory = opendir(kProcDirectory.c_str());
    struct dirent* file;

    while ((file = readdir(directory)) != nullptr) {
        // Is this a directory?
        if (file->d_type == DT_DIR) {
            // Is every character of the name a digit?
            string filename(file->d_name);
            if (std::all_of(filename.begin(), filename.end(), isdigit)) {
                int pid = stoi(filename);
                pids.push_back(pid);
            }
        }
    }

    closedir(directory);

    return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    string line;
    string key;
    string value;
    std::ifstream stream(kProcDirectory + kMeminfoFilename);

    int memtotal, memfree;

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);

            while (linestream >> key >> value) {
                if (key == "MemTotal:") {
                    memtotal = stoi(value);
                } else if (key == "MemFree:") {
                    memfree = stoi(value);
                }
            }
        }
    }

    return memtotal - memfree;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
    string uptime, idletime;
    string line;
    std::ifstream stream(kProcDirectory + kUptimeFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime >> idletime;
    }

    return stol(uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    return 0;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
    return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    return 0;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    return 0;
}

// Read and return CPU utilization
float LinuxParser::CpuUtilization() {
    vector<string> cpu_values;
    string line;
    string value;
    std::ifstream stream(kProcDirectory + kStatFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);

        int count = 0;
        while(linestream >> value) {
            if (count > 0) {
                cpu_values.emplace_back(value);
            }
            count++;
        }
    }

    float user = stof(cpu_values[0]);
    float nice = stof(cpu_values[1]);
    float system = stof(cpu_values[2]);
    float idle = stof(cpu_values[3]);
    float iowait = stof(cpu_values[4]);
    float irq = stof(cpu_values[5]);
    float softirq = stof(cpu_values[6]);
    float steal = stof(cpu_values[7]);
    float PrevIdle = 0.0;
    float Idle = idle + iowait;
    float PrevNonIdle = 0.0;
    float NonIdle = user + nice + irq + softirq + system + steal;
    float PrevTotal = 0.0;
    float Total = Idle + NonIdle;

    // differentiate: actual minus initial
    float totald = Total - PrevTotal;
    float idled = Idle - PrevIdle;

    // calculate CPU Utilization
    float CPU_Percentage = (totald - idled) / totald;

    return CPU_Percentage;    
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    string line;
    string key;
    string value;
    std::ifstream stream(kProcDirectory + kStatFilename);

    int total_proc;

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);

            while (linestream >> key >> value) {
                if (key == "processes") {
                    total_proc = stoi(value);
                }
            }
        }
    }

    return total_proc;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    string line;
    string key;
    string value;
    std::ifstream stream(kProcDirectory + kStatFilename);

    int running_proc;

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);

            while (linestream >> key >> value) {
                if (key == "procs_running") {
                    running_proc = stoi(value);
                }
            }
        }
    }

    return running_proc;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
    string line;
    string command;

    std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            command = line;
        }
    }

    return command;
}

// Read and return the Cpu used by a process
float LinuxParser::Cpu(int pid) {
    auto get_attribute = [pid](int index) {
        std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
        string line;
        string target;

        if (stream.is_open()) {
            std::getline(stream, line);
            std::istringstream linestream(line);

            for (int i = 1; i <= index; i++) {
                linestream >> target;
            }
        }

        return target;
    };

    string u_time = get_attribute(14);
    string s_time = get_attribute(15);
    string cu_time = get_attribute(16);
    string cs_time = get_attribute(17);
    string start_time = get_attribute(22);

    float Hertz = sysconf(_SC_CLK_TCK);

    float total_time = stof(u_time) + stof(s_time);
    total_time += stof(cu_time) + stof(cs_time);

    float seconds = LinuxParser::UpTime() - (stof(start_time) / Hertz);
    float cpu_usage = 100 * ((total_time / Hertz) / seconds);

    return cpu_usage;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
    string line;
    string key;
    string value;
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

    long memory;

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);

            while (linestream >> key >> value) {
                if (key == "VmSize:") {
                    memory = stoi(value) / 1000;
                }
            }
        }
    }

    return std::to_string(memory);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
    string line;
    string key;
    string value;
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

    string uid;

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);

            while (linestream >> key >> value) {
                if (key == "Uid:") {
                    uid = value;
                }
            }
        }
    }

    return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
    string uid = LinuxParser::Uid(pid);

    string line;
    string name;
    string passwd;
    string id;
    std::ifstream filestream(kPasswordPath);

    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);

            while (linestream >> name >> passwd >> id) {
                if (id == uid) {
                    return name;
                }
            }
        }
    }
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
    string uptime;
    string line;
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);

        for (int i = 1; i <= 22; i++) {
            linestream >> uptime;
        }
    }

    return stol(uptime) / sysconf(_SC_CLK_TCK);
}

