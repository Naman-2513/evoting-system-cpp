# Architecture

## Overview

This is a console-based Election Voting System written in C++. It lets voters register, log in, and cast a single vote for a candidate, while an admin panel provides election management (viewing results, managing candidates and voters, and resetting the election). There is no database — all persistence is done through flat text files in the working directory.

The codebase is split into modules by responsibility (voter, candidate, vote, admin, encryption, utils), each with a `.cpp`/`.h` pair, and wired together from a single `main.cpp` entry point with a menu-driven CLI.

## System Diagram

```
                ┌────────────┐
                │  main.cpp  │   <- entry point, top-level menu
                └─────┬──────┘
                       │
   ┌───────────┬───────┼────────────┬─────────────┐
   │           │       │            │             │
┌──▼───┐   ┌───▼──┐ ┌──▼───┐   ┌────▼─────┐  ┌────▼────┐
│voter │   │vote  │ │candi-│   │  admin   │  │  utils  │
│.cpp  │   │.cpp  │ │date  │   │  .cpp    │  │  .cpp   │
└──┬───┘   └──┬───┘ └──┬───┘   └────┬─────┘  └─────────┘
   │          │        │            │
   │          ▼        │      (calls into vote/candidate/voter)
   │   ┌─────────────┐ │
   │   │encryption   │ │
   │   │.cpp (XOR)   │ │
   │   └─────────────┘ │
   │                    │
   ▼                    ▼
voters.txt          candidates.txt
votes.txt
```

## Components

### `main.cpp`
- Application entry point.
- Loads candidates on startup (`loadCandidates()`), prints the banner, and runs the top-level menu loop (register voter, cast vote, view candidates, admin panel, exit).
- Delegates all real work to the other modules; contains no business logic itself.

### `voter.cpp` / `voter.h`
- Defines the `Voter` class and free functions for voter management.
- `Voter::registerVoter()` — registers a new voter, checks for duplicate IDs, enforces a minimum password length, and appends the record to `voters.txt`.
- `login()` — validates voter ID/password against `voters.txt` and reports whether the voter has already voted.
- `updateVoteStatus()` — flips a voter's `hasVoted` flag to true after a vote is cast (rewrites `voters.txt` via a temp file).
- `searchVoter()` / `deleteVoter()` — admin-facing lookup and deletion of voter records.

### `candidate.cpp` / `candidate.h`
- Maintains the in-memory `candidates` vector (global) and syncs it with `candidates.txt`.
- `loadCandidates()` — loads from file, or seeds 3 default candidates if the file is empty/missing.
- `addCandidate()` / `removeCandidate()` — admin operations on the candidate list, persisted immediately via `saveCandidates()`.
- `showCandidates()` / `viewCandidatesMenu()` — display formatted candidate listings to the user.

### `vote.cpp` / `vote.h`
- Core voting workflow:
  - `voterLogin()` — prompts for credentials, calls `login()`, blocks already-voted users, then hands off to `castVote()`.
  - `castVote()` — displays the ballot, validates the chosen candidate, asks for confirmation, encrypts the vote (`encryptVote()`), and appends it to `votes.txt`. Also triggers `updateVoteStatus()`.
- Results and admin reporting:
  - `showResults()` — tallies decrypted votes from `votes.txt`, computes turnout, renders a bar chart per candidate, determines the winner (or a tie), and prints a full encrypted/decrypted vote log.
  - `deleteAllVotes()` — admin-only full reset; requires typing `CONFIRM`, then truncates both `votes.txt` and `voters.txt`.

### `admin.cpp` / `admin.h`
- `adminPanel()` — password-gated menu (hardcoded password) exposing: show results, add/remove candidate, search/delete voter, delete all votes, and return to main menu.
- Acts purely as a router into the other modules; holds no state of its own.

### `encryption.cpp` / `encryption.h`
- Minimal vote "encryption" using a single XOR operation against a constant `KEY`.
- `encryptVote()` and `decryptVote()` are the same operation (XOR is self-inverse), so this is symmetric obfuscation rather than real encryption.

### `utils.cpp` / `utils.h`
- Shared CLI presentation helpers: `printBanner()`, `printDivider()`, `loading()`, `loadingDots()`, `spinner()`, `winnerAnimation()`.
- Also expected to define color constants (`RED`, `GREEN`, `YELLOW`, `CYAN`, `BOLD`, `RESET`) used throughout the other modules.

## Data Flow

1. **Startup**: `main()` calls `loadCandidates()`, populating the in-memory `candidates` vector from `candidates.txt` (or seeding defaults).
2. **Registration**: User input → `Voter::registerVoter()` → duplicate/password checks → append to `voters.txt`.
3. **Voting**: User input → `voterLogin()` → `login()` reads `voters.txt` → if valid and not yet voted → `castVote()` → candidate validated against in-memory `candidates` → vote encrypted → appended to `votes.txt` → `updateVoteStatus()` rewrites `voters.txt` with `hasVoted = true`.
4. **Results**: `showResults()` streams `votes.txt`, decrypts each entry, tallies per-candidate counts in memory, cross-references `voters.txt` for turnout, then renders the report.
5. **Admin actions**: All admin operations read/write the same two files (`voters.txt`, `votes.txt`) or `candidates.txt`, generally by rewriting through a temporary file and renaming it (e.g. `updateVoteStatus()`, `deleteVoter()`).

## Tech Stack

- **Language**: C++ (STL: `<iostream>`, `<fstream>`, `<vector>`, `<string>`, `<thread>`, `<chrono>`, `<iomanip>`)
- **Build**: single executable compiled from all `.cpp` files (no external dependencies)
- **Persistence**: plain text files (`voters.txt`, `candidates.txt`, `votes.txt`) — no database
- **UI**: text-based CLI with ANSI color codes and simple loading/spinner animations

## File/Data Formats

- `voters.txt`: one line per voter — `voterID name password hasVoted`
- `candidates.txt`: one line per candidate — `id name party`
- `votes.txt`: one line per vote — XOR-encrypted candidate ID (integer)

## Design Decisions

- **Flat files over a database**: keeps the project dependency-free and easy to run/grade for a course project, at the cost of no concurrency safety and weak query capability.
- **Global `candidates` vector**: simplifies passing candidate data between `vote.cpp`, `candidate.cpp`, and `admin.cpp` without a shared context object, at the cost of tighter coupling.
- **XOR "encryption"**: a deliberately simple, reversible transform to demonstrate the concept of protecting vote values at rest, not intended as cryptographically secure.
- **Rewrite-via-temp-file pattern**: used in `updateVoteStatus()` and `deleteVoter()` to "edit" a line in a flat file, since text files can't be edited in place.
- **Hardcoded admin password**: simplifies the demo; a real system would hash/store this separately or use proper auth.

## Known Limitations / Future Considerations

- No concurrency control — simultaneous runs/writes to the text files could corrupt data.
- Admin password is hardcoded in source and stored in plaintext comparison.
- XOR-based vote "encryption" provides obfuscation, not real security.
- `showResults()` uses a fixed-size `count[1000]` array, assuming candidate IDs stay below 1000.
- No input sanitization beyond basic numeric validation (e.g., names/passwords with spaces would break the space-delimited file format).
- Possible future improvements: move to a real database, add password hashing, support multi-word names safely (e.g., JSON/CSV with proper escaping), add unit tests, and replace XOR with real encryption (e.g., AES) if genuine confidentiality is required.
