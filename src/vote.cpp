#include "vote.h"
#include "encryption.h"   // NEW — for encryptVote/decryptVote
#include "voter.h"
#include "candidate.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;


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