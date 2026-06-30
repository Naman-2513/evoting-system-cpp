#include "encryption.h"

int encryptVote(int vote) { return vote ^ KEY; }
int decryptVote(int vote) { return vote ^ KEY; }