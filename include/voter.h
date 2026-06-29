#ifndef VOTER_H
#define VOTER_H

#include <string>
using namespace std;

class Voter {
public:
    string voterID;
    string name;
    string password;
    bool hasVoted;

    void registerVoter();
};

bool login(string id, string pass, bool &voted, string &voterName);
void updateVoteStatus(string id);
void searchVoter();
void deleteVoter();

#endif