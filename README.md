# Chronicle

A lightweight version control system written in C.

Chronicle lets you track file changes, commit snapshots, view history, compare versions, and roll back to any past state — all from the terminal.

---

## Features

| Command | Description |
|---|---|
| `init` | Initialize a new Chronicle repository |
| `add <file>` | Stage a file for the next commit |
| `commit -m <msg>` | Save a snapshot with a message |
| `log` | View full commit history |
| `diff [file]` | Show line-by-line changes (LCS algorithm) |
| `status` | Show staged and unstaged changes |
| `rollback <id>` | Restore working tree to any past commit |
| `guard <file>` | 🛡️ Mark a file as protected (Chronicle Guard) |

### 🛡️ Chronicle Guard — Unique Feature

Mark any file as "guarded". Before every commit, Chronicle checks that the guarded file still exists in your working directory. If it's missing, you get a loud warning — stopping you from accidentally committing without a critical file.

---

## Prerequisites

- GCC (or any C99-compatible compiler)
- Make (GNU Make recommended)

---

## Build Instructions

```bash
# Clone the repository
git clone https://github.com/AmritpaL-Singh0945/Chronicle.git
cd Chronicle

# Compile
make

## Usage

```bash
# 1. Initialize a repository in your project folder
./chronicle init

# 2. Stage files you want to track
./chronicle add main.c
./chronicle add include/utils.h

# 3. Commit your staged files
./chronicle commit -m "Initial commit"

# 4. Make changes, then check what changed
./chronicle status
./chronicle diff main.c

# 5. Commit again
./chronicle add main.c
./chronicle commit -m "Fix memory leak in parser"

# 6. View history
./chronicle log

# 7. Roll back to an earlier commit
./chronicle rollback 1

# 8. Guard a critical file
./chronicle guard config.h
```

---

## Project Structure

```
Chronicle/
├── src/           # C source files
├── include/       # Header files
├── Makefile       # Build system
├── README.md      # This file
└── LICENSE        # MIT License
```

### Internal Storage (`.chronicle/`)

```
.chronicle/
├── HEAD                    # Current commit number
├── staging_index.txt       # Staged files and their hashes
├── guards.cfg              # List of guarded files
├── staging/                # Copies of staged files
└── commits/
    ├── 0001/
    │   ├── meta.txt        # id, message, timestamp, files
    │   └── files/          # Full snapshot of committed files
    └── 0002/
        └── ...
```

---

## How Diff Works

Chronicle uses the **LCS (Longest Common Subsequence)** algorithm — the same approach used by the Unix `diff` utility:

1. Read both file versions line by line
2. Build an LCS table using dynamic programming
3. Backtrack to identify added (`+`) and removed (`-`) lines

---

## License

MIT — see [LICENSE](LICENSE)
