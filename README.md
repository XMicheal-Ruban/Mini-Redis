# Mini-Redis in C++

A multi-threaded, in-memory key-value store inspired by Redis, built from scratch using C++ and Winsock.

## Features
- **Multi-threaded:** Handles multiple client connections using `std::thread`.
- **RESP Protocol:** Supports basic REdis Serialization Protocol parsing.
- **Commands:** Implements `SET`, `GET`, and `PING`.
- **In-Memory:** Uses a Hash Map for $O(1)$ average time complexity for data access.

## How to Run (Windows)
1. Compile: `g++ main.cpp -o mini_redis -lws2_32`
2. Run: `./mini_redis`
3. Connect using `redis-cli` or a TCP tester.