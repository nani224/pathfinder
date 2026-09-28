# PathFinder: Graph Shortest Path & K-Alternative Routes

High-performance Graph Theory & Shortest Path Engine built for the **2nd Year Data Structures & Algorithms Course Project**.

## 🚀 Key Features

- **Dijkstra's Algorithm with Min-Heap Priority Queue**: Achieves optimal $O((V + E) \log V)$ time complexity for single-source shortest paths on non-negative weighted graphs.
- **K-Alternative Route Enumeration**: Implements edge penalty/blocking to calculate next-best detour paths without duplicating previously explored edges.
- **String Hash Table (djb2)**: Open-addressing hash table with case-insensitive hashing, enabling lightning-fast string node lookups (e.g. `France`, `India`, `spain`).
- **Dynamic Adjacency List**: Reallocating dynamic edge arrays allowing scalable vertices and arbitrary edge density.
- **Interactive Terminal UI**: ANSI color-coded route rendering, complete node inventory (`list`), and numeric ID or name-based navigation.
- **Memory Safety**: Clean deallocation lifecycle (`free_graph`) ensuring zero memory leaks.

## 📁 Project Structure

```text
pathfinder/
├── bin/                 # Compiled executable (pathfinder.exe)
├── data/                # Graph topology & visualization
│   ├── graph.txt        # 22-node international road/transit network
│   └── graph.png        # Graphical map representation
├── include/             # C header interface
│   └── pathfinder.h
├── src/                 # Implementation
│   ├── main.c           # Interactive CLI
│   └── pathfinder.c     # Hashmap, Min-heap, Dijkstra & K-paths engine
├── build.bat            # One-click compiler script (GCC)
├── run.bat              # Interactive launcher
├── test_pathfinder.py   # Automated algorithm verification suite
├── dsreport.pdf         # Course project formal report
└── README.md
```

## 🛠️ Build and Run

### 1. Build
```cmd
build.bat
```
*(Uses MinGW `gcc -O2 -Wall src\main.c src\pathfinder.c -o bin\pathfinder.exe`)*

### 2. Run Interactively
```cmd
run.bat
```
Commands inside the app:
- `list` : View all 22 available nodes with edge counts.
- `<Source> <Target>` : Find the optimal shortest path.
- `next` : Compute the next-best alternative detour.
- `new` : Start a new route search.
- `exit` : Cleanly exit the engine.

### 3. Run Automated Algorithm Tests
```cmd
python test_pathfinder.py
```
Validates graph loading, node counting, optimal Dijkstra path calculation, detour alternatives, and invalid input resilience.
