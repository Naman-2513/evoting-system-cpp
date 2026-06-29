#include "utils.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void loadingDots(string msg) {
    cout << msg;
    for (int i = 0; i < 5; i++) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(300));
    }
    cout << endl;
}

void spinner(string msg) {
    char spin[] = {'|', '/', '-', '\\'};
    cout << msg << " ";
    for (int i = 0; i < 12; i++) {
        cout << "\b" << spin[i % 4];
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << "\b done" << endl;
}

void loading(string message) {
    cout << message;
    for (int i = 0; i < 3; i++) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(400));
    }
    cout << endl;
}

void winnerAnimation(string name) {
    cout << "\n";
    string line = "*** WINNER: " + name + " ***";
    for (int i = 0; i < 3; i++) {
        cout << YELLOW << BOLD << line << RESET << "\n";
        this_thread::sleep_for(chrono::milliseconds(400));
    }
}

void printBanner() {
    cout << CYAN << BOLD;
    cout << "\n";
    cout << "  ╔══════════════════════════════════════╗\n";
    cout << "  ║      ELECTION VOTING SYSTEM v2.0     ║\n";
    cout << "  ║         Secure Digital Ballot        ║\n";
    cout << "  ╚══════════════════════════════════════╝\n";
    cout << RESET << "\n";
}

void printDivider(string title = "") {
    if (title.empty()) {
        cout << CYAN << "  ----------------------------------------\n" << RESET;
    } else {
        cout << CYAN << "\n  --- " << title << " ---\n" << RESET;
    }
}
