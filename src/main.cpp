#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <iomanip>
#include <thread>
#include <chrono>
using namespace std;

#define GREEN  "\033[32m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define BLUE   "\033[34m"
#define MAGENTA "\033[35m"
#define BOLD   "\033[1m"
#define RESET  "\033[0m"

const int KEY = 7;

/* -------------------- Animations -------------------- */

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

/* -------------------- Candidate -------------------- */

class Candidate {
public:
    int id;
    string name;
    string party;
};

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

/* -------------------- Encryption -------------------- */

int encryptVote(int vote)  { return vote ^ KEY; }
int decryptVote(int vote)  { return vote ^ KEY; }

/* -------------------- Voter -------------------- */

class Voter {
public:
    string voterID;
    string name;
    string password;
    bool hasVoted;

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
};

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
        cout << RED << "  [X] Voter ID not found.\n" << RESET;
}

/* -------------------- Cast Vote -------------------- */

void castVote(string voterID, string voterName) {
    if (candidates.empty()) {
        cout << RED << "  [X] No candidates available!\n" << RESET;
        return;
    }

    printDivider("Official Ballot");
    cout << "\n  Voter : " << BOLD << voterName << RESET
         << "  (" << voterID << ")\n\n";
    cout << "  Select one candidate and enter their number.\n";
    cout << "  This action cannot be undone.\n\n";

    showCandidates();

    int choice;
    cout << "  Enter candidate number: ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << RED << "  [X] Invalid input!\n" << RESET;
        return;
    }

    bool valid = false;
    string chosenName, chosenParty;
    for (auto &c : candidates) {
        if (c.id == choice) {
            valid = true;
            chosenName  = c.name;
            chosenParty = c.party;
            break;
        }
    }

    if (!valid) {
        cout << RED << "  [X] Invalid candidate choice!\n" << RESET;
        return;
    }

    /* Confirm before casting */
    cout << "\n  You selected: " << BOLD << GREEN << chosenName
         << RESET << "  (" << chosenParty << ")\n";
    cout << "  Confirm vote? (y/n): ";
    char confirm;
    cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') {
        cout << YELLOW << "  Vote cancelled.\n" << RESET;
        return;
    }

    spinner("  Casting Vote");

    int encrypted = encryptVote(choice);

    ofstream file("votes.txt", ios::app);
    if (!file) {
        cout << RED << "  [X] Error opening votes file!\n" << RESET;
        return;
    }
    file << encrypted << "\n";
    file.close();

    updateVoteStatus(voterID);

    /* Vote receipt */
    printDivider("Vote Receipt");
    cout << "\n";
    cout << "  " << GREEN << BOLD << "[OK] Vote Cast Successfully!\n" << RESET;
    cout << "\n";
    cout << "  Voter            : " << BOLD << voterName << RESET << " (" << voterID << ")\n";
    cout << "  Voted for        : " << BOLD << GREEN << chosenName << RESET
         << " (" << chosenParty << ")\n";
    cout << "  Encrypted token  : 0x" << hex << uppercase << encrypted
         << dec << "  (XOR-" << KEY << ")\n";
    cout << "\n";
    cout << CYAN << "  Thank you for participating in the election!\n" << RESET;
}

/* -------------------- Voter Login -------------------- */

void voterLogin() {
    string id, pass;
    bool voted = false;
    string voterName;

    printDivider("Voter Login");
    cout << "\n  Enter Voter ID  : ";
    cin >> id;
    cout << "  Enter Password  : ";
    cin >> pass;

    loading("\n  Logging in");

    if (!login(id, pass, voted, voterName)) {
        cout << RED << "  [X] Invalid Credentials!\n" << RESET;
        return;
    }

    if (voted) {
        cout << YELLOW << "  [!] You have already cast your vote.\n" << RESET;
        return;
    }

    cout << GREEN << "  [OK] Login Successful! Welcome, "
         << BOLD << voterName << RESET << GREEN << "!\n" << RESET;

    castVote(id, voterName);
}

/* -------------------- Results -------------------- */

void showResults() {
    loadingDots("  Counting Votes");

    int count[1000] = {0};
    ifstream file("votes.txt");
    int vote;
    int totalVotes = 0;

    while (file >> vote) {
        int original = decryptVote(vote);
        if (original >= 0 && original < 1000) {
            count[original]++;
            totalVotes++;
        }
    }
    file.close();

    /* Count registered voters */
    int totalVoters = 0;
    ifstream vf("voters.txt");
    string vid, vname, vpwd;
    bool vvoted;
    while (vf >> vid >> vname >> vpwd >> vvoted) totalVoters++;
    vf.close();

    /* Stats header */
    printDivider("Election Results");
    cout << "\n";
    cout << "  " << BOLD << "Registered Voters : " << RESET << totalVoters << "\n";
    cout << "  " << BOLD << "Votes Cast        : " << RESET << totalVotes  << "\n";
    if (totalVoters > 0) {
        int turnout = (int)((double)totalVotes / totalVoters * 100);
        cout << "  " << BOLD << "Voter Turnout     : " << RESET << turnout << "%\n";
    }
    cout << "\n";

    /* Per-candidate bar chart */
    int maxVotes = 0;
    for (auto &c : candidates)
        if (count[c.id] > maxVotes) maxVotes = count[c.id];

    string winner = "No votes cast";
    int winnerVotes = -1;

    for (auto &c : candidates) {
        int v   = count[c.id];
        int pct = totalVotes > 0 ? (int)((double)v / totalVotes * 100) : 0;
        int bar = maxVotes  > 0 ? (int)((double)v / maxVotes  * 20)   : 0;

        cout << "  " << BOLD << setw(10) << left << c.name << RESET << "  ";
        cout << GREEN;
        for (int i = 0; i < bar;      i++) cout << "#";
        cout << RESET;
        for (int i = bar; i < 20; i++)    cout << "-";
        cout << "  " << BOLD << setw(3) << right << v << RESET
             << " votes  (" << pct << "%)\n";

        this_thread::sleep_for(chrono::milliseconds(250));

        if (v > winnerVotes) {
            winnerVotes = v;
            winner = c.name;
        }
    }

    /* Tie / winner */
    cout << "\n";
    int tieCount = 0;
    for (auto &c : candidates)
        if (count[c.id] == winnerVotes) tieCount++;

    if (totalVotes == 0) {
        cout << YELLOW << "  [!] No votes have been cast yet.\n" << RESET;
    } else if (tieCount > 1) {
        cout << YELLOW << "  [!] It's a TIE! Multiple candidates with "
             << winnerVotes << " vote(s).\n" << RESET;
    } else {
        spinner("  Finalizing Result");
        winnerAnimation(winner);
    }

    /* Encrypted vote log */
    printDivider("Encrypted Vote Log");
    ifstream vlog("votes.txt");
    int idx = 1;
    while (vlog >> vote) {
        int orig = decryptVote(vote);
        string cn = "Unknown";
        for (auto &c : candidates)
            if (c.id == orig) { cn = c.name; break; }
        cout << "  [" << setw(2) << idx++ << "]  enc=0x"
             << hex << uppercase << setw(2) << setfill('0') << vote
             << dec << setfill(' ') << "  ->  dec=" << orig
             << "  (" << cn << ")\n";
    }
    vlog.close();
    cout << "\n";
}

/* -------------------- Delete All Votes -------------------- */

void deleteAllVotes() {
    /* Count current votes */
    int totalVotes = 0;
    ifstream countFile("votes.txt");
    int v;
    while (countFile >> v) totalVotes++;
    countFile.close();

    /* Count registered voters */
    int totalVoters = 0;
    ifstream countVoters("voters.txt");
    string vid, vname, vpwd;
    bool vvoted;
    while (countVoters >> vid >> vname >> vpwd >> vvoted) totalVoters++;
    countVoters.close();

    if (totalVotes == 0 && totalVoters == 0) {
        cout << YELLOW << "  [!] Nothing to delete. Votes and voters are already empty.\n" << RESET;
        return;
    }

    /* Show warning box */
    cout << "\n";
    cout << RED << BOLD;
    cout << "  ╔══════════════════════════════════════╗\n";
    cout << "  ║         !!! WARNING !!!              ║\n";
    cout << "  ║  This will permanently delete ALL    ║\n";
    cout << "  ║  votes AND all registered voters.    ║\n";
    cout << "  ║  This action CANNOT be undone.       ║\n";
    cout << "  ╚══════════════════════════════════════╝\n";
    cout << RESET;
    cout << "\n";
    cout << "  Votes to delete             : " << RED << BOLD << totalVotes  << RESET << "\n";
    cout << "  Registered voters to delete : " << RED << BOLD << totalVoters << RESET << "\n";
    cout << "\n";
    cout << YELLOW << "  Type " << BOLD << "CONFIRM" << RESET
         << YELLOW << " to proceed, or anything else to cancel: " << RESET;

    string input;
    cin >> input;

    if (input != "CONFIRM") {
        cout << YELLOW << "  [!] Cancelled. Nothing was deleted.\n" << RESET;
        return;
    }

    spinner("  Wiping all votes");

    /* Wipe votes.txt */
    ofstream wipeVotes("votes.txt", ios::trunc);
    wipeVotes.close();

    spinner("  Deleting all voters");

    /* Wipe voters.txt */
    ofstream wipeVoters("voters.txt", ios::trunc);
    wipeVoters.close();

    cout << "\n";
    cout << GREEN << BOLD << "  [OK] Full system reset complete!\n" << RESET;
    cout << "  Votes deleted            : " << RED   << totalVotes  << RESET << "\n";
    cout << "  Registered voters deleted: " << RED   << totalVoters << RESET << "\n";
    cout << CYAN   << "  The election has been fully wiped and reset.\n" << RESET;
    cout << YELLOW << "  New voters can now register from the main menu.\n" << RESET;
}

/* -------------------- Admin Panel -------------------- */

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

/* -------------------- MAIN -------------------- */

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