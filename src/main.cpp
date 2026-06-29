#include "candidate.h"
#include "voter.h"
#include "vote.h"
#include "utils.h"
#include <iostream>
using namespace std;

int main() {
    loadCandidates();
    printBanner();

    int choice;
    while (true) {
        cout << BOLD << "  ======================================\n" << RESET;
        cout << "  1.  Register Voter\n";
        cout << "  2.  Cast Vote\n";
        cout << "  3.  View Candidates\n";
        cout << "  4.  Admin Panel\n";
        cout << "  5.  Exit\n";
        cout << BOLD << "  ======================================\n" << RESET;
        cout << "  Enter Choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << RED << "  [X] Invalid input!\n" << RESET;
            continue;
        }

        switch (choice) {
            case 1: { Voter v; v.registerVoter(); break; }
            case 2: voterLogin();         break;
            case 3: viewCandidatesMenu(); break;
            case 4: adminPanel();         break;
            case 5:
                cout << GREEN << "\n  Goodbye! Exiting...\n" << RESET;
                return 0;
            default:
                cout << RED << "  [X] Invalid choice.\n" << RESET;
        }
    }
}
