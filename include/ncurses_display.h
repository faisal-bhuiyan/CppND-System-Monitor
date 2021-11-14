#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <curses.h>

#include "process.h"
#include "system.h"

namespace NCursesDisplay {
// Displays all components of the system monitor
void Display(System& system, int n_processes = 10);

// Displays the info regarding the system
void DisplaySystem(System& system, WINDOW* window);

// Displays the info regarding the processes
void DisplayProcesses(std::vector<Process>& processes, WINDOW* window, int n_processes);

// Shows the progress bar
std::string ProgressBar(float percent);
};  // namespace NCursesDisplay

#endif

