#include "linux_parser.h"

#include <dirent.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// Searches for a particular value in a file by a key and returns it
template <typename T>
T _FindValueByKey(const string& key_filter, const string& filename) {
    string line, key;
    T value;

    std::ifstream stream(LinuxParser::kProcDirectory + filename);

    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            std::istringstream linestream(line);

            while (linestream >> key >> value) {
                if (key == key_filter) {
                    return value;
                }
            }
        }
    }

    stream.close();

    return value;
};

// Returns a value from a file
template <typename T>
T _GetValueFromFile(const string& filename) {
    string line;
    T value;

    std::ifstream stream(LinuxParser::kProcDirectory + filename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> value;
    }

    stream.close();

    return value;
};

// Wrapping std::stof or std::stol because it may throw an exception
template <typename T, typename F>
T _StringToNumber(const std::string& str, F f) {
    try {
        return f(str);
    } catch (...) {
        return 0.;
    }
}

float _stof(std::string str="0") {
    return stof(str);
}

long _stol(std::string str="0") {
    return stol(str);
}

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

    filestream.close();

    return value;
}

string LinuxParser::Kernel() {
    string os, kernel, version;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }

    stream.close();

    return kernel;
}

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

float LinuxParser::MemoryUtilization() {
    string key_1 = "MemTotal:";
    string key_2 = "MemFree:";

    float total = _FindValueByKey<float>(key_1, kMeminfoFilename);
    float free = _FindValueByKey<float>(key_2, kMeminfoFilename);

    return (total - free) / total;
}

long LinuxParser::UpTime() {
    string uptime, idletime;
    string line;

    std::ifstream stream(kProcDirectory + kUptimeFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime >> idletime;
    }

    stream.close();

    return _StringToNumber<long>(uptime, _stol);
}

// long LinuxParser::Jiffies() {
//     return 0;
// }

// long LinuxParser::ActiveJiffies(int pid) {
//     return 0;
// }

// long LinuxParser::ActiveJiffies() {
//     return 0;
// }

// long LinuxParser::IdleJiffies() {
//     return 0;
// }

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

    stream.close();

    float user = _StringToNumber<float>(cpu_values[0], _stof);
    float nice = _StringToNumber<float>(cpu_values[1], _stof);
    float system = _StringToNumber<float>(cpu_values[2], _stof);
    float idle = _StringToNumber<float>(cpu_values[3], _stof);
    float iowait = _StringToNumber<float>(cpu_values[4], _stof);
    float irq = _StringToNumber<float>(cpu_values[5], _stof);
    float softirq = _StringToNumber<float>(cpu_values[6], _stof);
    float steal = _StringToNumber<float>(cpu_values[7], _stof);
    float PrevIdle = 0.0;
    float Idle = idle + iowait;
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

int LinuxParser::TotalProcesses() {
    string key = "processes";
    return _FindValueByKey<int>(key, kStatFilename);
}

int LinuxParser::RunningProcesses() {
    string key = "procs_running";
    return _FindValueByKey<int>(key, kStatFilename);
}

string LinuxParser::Command(int pid) {
    string filename = to_string(pid) + kCmdlineFilename;
    return string(_GetValueFromFile<string>(filename));
}

float LinuxParser::Cpu(int pid) {
    auto get_attribute = [pid](int index) {
        std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
        string line;
        string target;

        if (stream.is_open()) {
            std::getline(stream, line);
            std::istringstream linestream(line);

            for (int i = 1; i <= index; i++) {
                linestream >> target;
            }
        }

        stream.close();

        return target;
    };

    string u_time = get_attribute(14);
    string s_time = get_attribute(15);
    string cu_time = get_attribute(16);
    string cs_time = get_attribute(17);
    string start_time = get_attribute(22);

    float Hertz = sysconf(_SC_CLK_TCK);

    float total_time = _StringToNumber<float>(u_time, _stof) + _StringToNumber<float>(s_time, _stof);
    total_time += _StringToNumber<float>(cu_time, _stof) + _StringToNumber<float>(cs_time, _stof);

    float seconds = LinuxParser::UpTime() - (_StringToNumber<float>(start_time, _stof) / Hertz);
    float cpu_usage = (total_time / Hertz) / seconds;

    return cpu_usage;
}

string LinuxParser::Ram(int pid) {
    string key = "VmRSS:";
    string value = string(_FindValueByKey<string>(key, to_string(pid) + kStatusFilename));

    int memory_MB = stoi(value) / 1024;  // Converting to MB

    return to_string(memory_MB);
}

string LinuxParser::Uid(int pid) {
    string key = "Uid:";
    return _FindValueByKey<string>(key, to_string(pid) + kStatusFilename);
}

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

    return string();
}

long LinuxParser::UpTime(int pid) {
    string uptime;
    string line;

    std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);

        for (int i = 1; i <= 22; i++) {
            linestream >> uptime;
        }
    }

    stream.close();

    return _StringToNumber<long>(uptime, _stol) / sysconf(_SC_CLK_TCK);
}

