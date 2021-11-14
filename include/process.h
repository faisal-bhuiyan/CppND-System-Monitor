#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/*
* Basic class for Process representation
* It contains relevant attributes as shown below
*/
class Process {
public:
    Process(int pid);

    // Returns this process's ID
    int Pid();

    // Returns the user (name) that generated this process
    std::string User();

    // Returns the command that generated this process
    std::string Command();

    // Returns this process's CPU utilization
    float CpuUtilization();

    // Returns this process's memory utilization
    std::string Ram();

    // Returns the age of this process (in seconds)
    long int UpTime();

    // Overloads the "less than" comparison operator for Process objects
    bool operator<(const Process& other) const;

private:
    int pid_;
    float cpu_usage_;
};

#endif

