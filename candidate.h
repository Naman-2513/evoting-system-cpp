#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <string>
#include <vector>
using namespace std;

class Candidate {
public:
    int id;
    string name;
    string party;
};

extern vector<Candidate> candidates;

void saveCandidates();
void loadCandidates();
void showCandidates();
void addCandidate();
void removeCandidate();
void viewCandidatesMenu();

#endif