#include "voter.h"
#include "utils.h"
#include <iostream>
#include <fstream>
using namespace std;

  void registerVoter() {
        cout << "\n  Enter Voter ID   : ";
        cin >> voterID;

        ifstream check("voters.txt");
        string vid, vname, vpwd;
        bool vvoted;
        while (check >> vid >> vname >> vpwd >> vvoted) {
            if (vid == voterID) {
                cout << RED << "  [X] Voter ID already exists!\n" << RESET;
                check.close();
                return;
            }
        }
        check.close();

        cout << "  Enter Name       : ";
        cin >> name;
        cout << "  Create Password  : ";
        cin >> password;

        if (password.length() < 4) {
            cout << RED << "  [X] Weak password! Must be at least 4 characters.\n" << RESET;
            return;
        }

        hasVoted = false;

        ofstream file("voters.txt", ios::app);
        file << voterID << " " << name << " " << password << " " << hasVoted << "\n";
        file.close();

        cout << GREEN << "  [OK] Registration Successful! Welcome, " << name << "!\n" << RESET;
    }


bool login(string id, string pass, bool &voted, string &voterName) {
    ifstream file("voters.txt");
    string vid, name, password;
    bool hasVoted;
    while (file >> vid >> name >> password >> hasVoted) {
        if (vid == id) {
            if (password == pass) {
                voted = hasVoted;
                voterName = name;
                file.close();
                return true;
            } else {
                file.close();
                return false;
            }
        }
    }
    file.close();
    return false;
}

void updateVoteStatus(string id) {
    ifstream file("voters.txt");
    ofstream temp("temp.txt");
    string vid, name, password;
    bool voted;
    while (file >> vid >> name >> password >> voted) {
        if (vid == id) voted = true;
        temp << vid << " " << name << " " << password << " " << voted << "\n";
    }
    file.close();
    temp.close();
    remove("voters.txt");
    rename("temp.txt", "voters.txt");
}

void searchVoter() {
    string id;
    cout << "\n  Enter Voter ID: ";
    cin >> id;

    ifstream file("voters.txt");
    string vid, name, password;
    bool voted;
    while (file >> vid >> name >> password >> voted) {
        if (vid == id) {
            cout << GREEN << "\n  Found Voter:\n" << RESET;
            cout << "  Name   : " << BOLD << name << RESET << "\n";
            cout << "  ID     : " << vid << "\n";
            cout << "  Voted  : " << (voted ? GREEN "Yes" RESET : YELLOW "No" RESET) << "\n";
            return;
        }
    }
    cout << RED << "  [X] Voter not found.\n" << RESET;
}

void deleteVoter() {
    string id;
    cout << "\n  Enter Voter ID to delete: ";
    cin >> id;

    ifstream file("voters.txt");
    ofstream temp("temp.txt");
    string vid, name, password;
    bool voted;
    bool found = false;

    while (file >> vid >> name >> password >> voted) {
        if (vid != id)
            temp << vid << " " << name << " " << password << " " << voted << "\n";
        else
            found = true;
    }
    file.close();
    temp.close();
    remove("voters.txt");
    rename("temp.txt", "voters.txt");

    if (found)
        cout << GREEN << "  [-] Voter deleted.\n" << RESET;
    else
        cout << RED << "  [X] Voter ID not found.\n" << RESET;}