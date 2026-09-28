#ifndef PATHFINDER_H
#define PATHFINDER_H

// Core graph operations
int load_graph(const char *fname);
void free_graph(void);

// Path querying APIs (case-insensitive string names and 0-based indices)
int find_shortest_path_by_name(const char *src_name, const char *tgt_name);
int find_shortest_path_by_index(int src_idx, int tgt_idx);

// Next-best (k-alternative) path enumeration
int find_next_best_path_current(void);

// Route inspection and output
void print_last_path(void);
double get_last_path_distance(void);
void list_all_nodes(void);
int get_node_count(void);

#endif // PATHFINDER_H
