#include "candidate.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>
using namespace std;

vector<Candidate> candidates;

void saveCandidates() {
    ofstream file("candidates.txt");
    for (auto &c : candidates)
        file << c.id << " " << c.name << " " << c.party << "\n";
    file.close();
}

void loadCandidates() {
    candidates.clear();
    ifstream file("candidates.txt");
    Candidate c;
    while (file >> c.id >> c.name >> c.party)
        candidates.push_back(c);
    file.close();

    if (candidates.empty()) {
        candidates.push_back({1, "Alice",   "PartyA"});
        candidates.push_back({2, "Bob",     "PartyB"});
        candidates.push_back({3, "Charlie", "PartyC"});
        saveCandidates();
    }
}

void showCandidates() {
    printDivider("Candidate List");
    cout << "\n";
    for (auto &c : candidates) {
        cout << "  " << BOLD << "[" << c.id << "]" << RESET
             << "  " << GREEN << setw(12) << left << c.name << RESET
             << "  " << YELLOW << "(" << c.party << ")" << RESET << "\n";
        this_thread::sleep_for(chrono::milliseconds(150));
    }
    cout << "\n";
}

void addCandidate() {
    Candidate c;
    cout << "\n  Enter Candidate ID   : ";
    cin >> c.id;

    for (auto &existing : candidates) {
        if (existing.id == c.id) {
            cout << RED << "  [X] Candidate ID already exists!\n" << RESET;
            return;
        }
    }

    cout << "  Enter Name           : ";
    cin >> c.name;
    cout << "  Enter Party          : ";
    cin >> c.party;

    candidates.push_back(c);
    saveCandidates();
    cout << GREEN << "  [+] Candidate Added Successfully!\n" << RESET;
}

void removeCandidate() {
    int id;
    cout << "\n  Enter Candidate ID to remove: ";
    cin >> id;

    for (auto it = candidates.begin(); it != candidates.end(); ++it) {
        if (it->id == id) {
            candidates.erase(it);
            saveCandidates();
            cout << GREEN << "  [-] Candidate Removed!\n" << RESET;
            return;
        }
    }
    cout << RED << "  [X] Candidate not found.\n" << RESET;
}

void viewCandidatesMenu() {
    if (candidates.empty()) {
        cout << RED << "  [X] No candidates available!\n" << RESET;
        return;
    }
    cout << CYAN << "\n  Fetching Candidate List...\n" << RESET;
    this_thread::sleep_for(chrono::milliseconds(500));
    showCandidates();
    cout << GREEN << "  [OK] End of List\n" << RESET;
}