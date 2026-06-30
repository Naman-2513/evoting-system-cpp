#include "admin.h"
#include "vote.h"        // showResults(), deleteAllVotes()
#include "candidate.h"   // addCandidate(), removeCandidate()
#include "voter.h"       // searchVoter(), deleteVoter()
#include "utils.h"       // printDivider(), spinner()
#include <iostream>
using namespace std;

void adminPanel() {
    string pass;
    printDivider("Admin Access");
    cout << "\n  Enter Admin Password: ";
    cin >> pass;

    spinner("  Verifying Admin");

    if (pass != "admin123") {
        cout << RED << "  [X] Access Denied!\n" << RESET;
        return;
    }
    cout << GREEN << "  [OK] Admin Access Granted!\n" << RESET;

    int choice;
    while (true) {
        printDivider("Admin Panel");
        cout << "\n";
        cout << "  1. Show Results\n";
        cout << "  2. Add Candidate\n";
        cout << "  3. Remove Candidate\n";
        cout << "  4. Search Voter\n";
        cout << "  5. Delete Voter\n";
        cout << RED << "  6. Delete ALL Votes (Reset Election)\n" << RESET;
        cout << "  7. Back to Main Menu\n";
        cout << "\n  Enter Choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << RED << "  [X] Invalid input!\n" << RESET;
            continue;
        }

        switch (choice) {
            case 1: showResults();     break;
            case 2: addCandidate();    break;
            case 3: removeCandidate(); break;
            case 4: searchVoter();     break;
            case 5: deleteVoter();     break;
            case 6: deleteAllVotes();  break;
            case 7: return;
            default: cout << RED << "  [X] Invalid choice.\n" << RESET;
        }
    }
}
