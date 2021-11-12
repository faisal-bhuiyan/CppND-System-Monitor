#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    static constexpr int ONE_HOUR = 60 * 60;
    static constexpr int ONE_MINUTE = 60;

    int hour;
    int min;
    int sec;

    hour = seconds / ONE_HOUR;
    seconds -= hour * ONE_HOUR;

    min = seconds / ONE_MINUTE;
    seconds -= min * ONE_MINUTE;

    sec = seconds;

    string elapsed_time = std::to_string(hour) + ":" + std::to_string(min) + ":" + std::to_string(sec);

    return elapsed_time;
}

