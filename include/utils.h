#ifndef UTILS_H
#define UTILS_H

#include <string>
using namespace std;

// Color codes
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

// Function declarations
void loadingDots(string msg);
void spinner(string msg);
void loading(string message);
void winnerAnimation(string name);
void printBanner();
void printDivider(string title = "");

#endif