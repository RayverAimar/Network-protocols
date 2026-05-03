# Network Protocols

Multi-client TCP chat server with a custom binary protocol, implemented in C++. Built for the **CS231 Networks & Communications** course.

## What's inside

```
Network-protocols/
├── src/
│   ├── server.cpp         # Multi-threaded TCP server (one thread per client)
│   ├── client.cpp         # Interactive CLI client
│   ├── test.txt           # Sample file for the file-transfer demo
│   └── receiver_folder/   # Where received files are saved
└── include/
    ├── utils.h            # Protocol constants and shared helpers (header-only)
    ├── TicTacToe.h        # TicTacToe game logic (header-only)
    └── client.h           # Client data class used by the server
```

## Protocol

The client and server communicate over TCP on port **45123** using a single-character tag followed by length-prefixed payloads. Type the letter at the `$` prompt:

| Key | Command    | Description                                         |
|-----|------------|-----------------------------------------------------|
| `N` | Nickname   | Change your display name                            |
| `L` | List       | Show all connected users (`*` marks you)            |
| `M` | Message    | Send a direct message to another user               |
| `B` | Broadcast  | Send a message to all connected users               |
| `F` | File       | Send a file to another user                         |
| `G` | Game       | Challenge or play TicTacToe with another user       |
| `R` | Quit       | Close the session                                   |

## Prerequisites

- C++17 compiler: `g++` (GCC ≥ 7) or `clang++` (Xcode Command Line Tools on macOS)
- POSIX sockets — works on **Linux** and **macOS**

## Compilation

From the repo root:

```bash
make          # compile both server and client
make clean    # remove compiled binaries
```

Or manually:

```bash
g++ -std=c++17 -o server src/server.cpp -lpthread
g++ -std=c++17 -o client src/client.cpp -lpthread
```

## Running

Open at least two terminals.

**Terminal 1 — start the server:**

```bash
./server
```

**Terminal 2 (and more) — connect a client:**

```bash
./client
```

The server listens on `127.0.0.1:45123`. Open as many client terminals as you want.

---

## Demo session

The following shows two clients connecting, listing users, exchanging messages, and disconnecting.

### Server terminal

```
NEW CLIENT:          [Alice]
NEW CLIENT:          [Bob]
LIST:                User list requested by [Alice].
MESSAGE:             [Bob] sent message to [Alice].
                       M06005AliceHello
EXIT:                User [Bob] logged off.
                       Sending notification to [Alice].
```

### Client: Alice

```
Enter your nickname (1-99 chars): Alice

$ L

 * Alice
   Bob

$ 
        [Bob] says: Hello

$ R
        You left the chat.
```

### Client: Bob

```
Enter your nickname (1-99 chars): Bob

$ M
Type the receiver: Alice
Type your message: Hello

$
        Alice just left the chat.

$ R
        You left the chat.
```

---

## Listing connected users (`L`)

```
$ L

 * Alice
   Bob
   Charlie

$
```

Users marked with ` * ` are you. Others are shown indented.

---

## Broadcast (`B`)

Sends a message to all connected users except yourself:

```
$ B
Message for all: Server is going down for maintenance

$
```

All other clients see:

```
        [Alice] shouted: Server is going down for maintenance
```

---

## File transfer (`F`)

```
$ F
Enter your friend's nickname: Bob
Type file name: src/test.txt
```

The file is saved in the receiver's `src/receiver_folder/` directory. Create it before running if it doesn't exist:

```bash
mkdir -p src/receiver_folder
```

---

## TicTacToe (`G`)

Challenge another user. You choose the board dimension (e.g. `3` for 3×3).

**Challenger (Alice):**

```
$ G
Type who you want to play with: Bob
Type the dimension of the board: 3

        [Bob] accepted the match :)

$ G
 ------  ------  ------  -
 | 01  | | 02  | | 03  |
 ------  ------  ------  -
 | 04  | | 05  | | 06  |
 ------  ------  ------  -
 | 07  | | 08  | | 09  |
 ------  ------  ------  -
Type your movement: 5
```

**Opponent (Bob):**

```
        [Alice] invited you to play a TicTacToe match! (Press 'G' to accept!)

$ G

        [Alice] made a new movement!

 ----  ----  ----  -
 |    | |    | |    |
 ----  ----  ----  -
 |    | | X  | |    |
 ----  ----  ----  -
 |    | |    | |    |
 ----  ----  ----  -

$ G
Type your movement: 1
```

The server relays each move between both clients. The game ends when a player completes a row, column, or diagonal, or the board is full (draw).

---

## Known limitations

- **No thread safety** — the shared `names` map in the server is accessed from multiple threads without a mutex; concurrent connects/disconnects can race.
- **Background thread spin** — the client's receive thread loops on `recv` after the socket closes instead of exiting cleanly.
- **Fixed buffers** — receive buffers are 500 bytes (client) / 1000 bytes (server); very long messages or large user lists may overflow.
- **`NICKNAME` and `NOTIFICATION` share the tag `'N'`** — they don't conflict at runtime (they flow in opposite directions: client→server vs server→client) but the naming is confusing.
