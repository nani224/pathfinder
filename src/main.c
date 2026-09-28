/**
 * Interactive CLI for PathFinder Graph Solver
 * Course Project: Data Structures & Algorithms (2nd Year)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "../include/pathfinder.h"

static void enable_ansi_escape() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
}

int main(int argc, char *argv[]) {
    enable_ansi_escape();

    const char *graph_file = "data/graph.txt";
    if (argc > 1) {
        graph_file = argv[1];
    } else {
        // Check if data/graph.txt exists, else fallback to graph.txt
        FILE *test = fopen(graph_file, "r");
        if (!test) {
            graph_file = "graph.txt";
        } else {
            fclose(test);
        }
    }

    printf("\033[1;36m====================================================\n");
    printf("   PathFinder: Dijkstra Shortest Path & K-Paths   \n");
    printf("   2nd Year Data Structures & Algorithms Project   \n");
    printf("====================================================\033[0m\n\n");

    if (!load_graph(graph_file)) {
        printf("\033[1;31m[ERROR] Failed to open '%s'. Please verify file exists.\033[0m\n", graph_file);
        return 1;
    }

    printf("\033[1;32m[+] Graph loaded successfully with %d nodes from '%s'.\033[0m\n", get_node_count(), graph_file);
    printf("Type '\033[1;33mlist\033[0m' to inspect available nodes, or '\033[1;33mexit\033[0m' to quit.\n");

    while (1) {
        char s1[128], s2[128];
        printf("\n\033[1;37mEnter source node name/ID (or 'list'/'exit'): \033[0m");
        if (scanf("%127s", s1) != 1) break;

        if (strcmp(s1, "exit") == 0 || strcmp(s1, "quit") == 0) break;
        if (strcmp(s1, "list") == 0) {
            list_all_nodes();
            continue;
        }

        printf("\033[1;37mEnter target node name/ID: \033[0m");
        if (scanf("%127s", s2) != 1) break;

        // Check if numeric (1-based index)
        int use_num1 = 1, use_num2 = 1;
        for (char *p = s1; *p; ++p) if (*p < '0' || *p > '9') { use_num1 = 0; break; }
        for (char *p = s2; *p; ++p) if (*p < '0' || *p > '9') { use_num2 = 0; break; }

        int ok = 0;
        if (use_num1 && use_num2) {
            int src_idx = atoi(s1) - 1;
            int tgt_idx = atoi(s2) - 1;
            ok = find_shortest_path_by_index(src_idx, tgt_idx);
        } else {
            ok = find_shortest_path_by_name(s1, s2);
        }

        if (!ok) {
            printf("\033[1;31mNo path exists between '%s' and '%s'. (Type 'list' to verify node names)\033[0m\n", s1, s2);
            continue;
        }

        printf("\n\033[1;33m[Optimal Shortest Route]\033[0m\n");
        print_last_path();

        // K-alternative paths loop
        int alt_count = 1;
        while (1) {
            char cmd[32];
            printf("Type '\033[1;35mnext\033[0m' for alternative route or '\033[1;36mnew\033[0m' for new search: ");
            if (scanf("%31s", cmd) != 1) break;

            if (strcmp(cmd, "new") == 0) break;
            if (strcmp(cmd, "next") == 0) {
                if (!find_next_best_path_current()) {
                    printf("\033[1;91mNo further alternative routes exist between these nodes.\033[0m\n");
                    break;
                }
                alt_count++;
                printf("\n\033[1;33m[Alternative Route #%d]\033[0m\n", alt_count);
                print_last_path();
            } else {
                printf("Unknown command '%s'. Please enter 'next' or 'new'.\n", cmd);
            }
        }
    }

    free_graph();
    printf("\n\033[1;36mPathFinder closed. Thank you!\033[0m\n");
    return 0;
}
