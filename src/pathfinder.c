/**
 * Pathfinder Engine - Graph Algorithms & Shortest Path Solver
 * Course Project: Data Structures & Algorithms (2nd Year)
 * 
 * Features:
 *  - Open-addressing String Hash Table (djb2 hash) for case-insensitive lookup
 *  - Dynamic Adjacency List graph representation
 *  - Binary Min-Heap Priority Queue for O((V + E) log V) Dijkstra's algorithm
 *  - K-Alternative / Next-Best path generation using edge blocking
 *  - Dynamic memory lifecycle management (free_graph)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/pathfinder.h"

#define INF 1e300
#define MAXN 5000
#define HM_INIT 8192
#define MAXBLOCK 20000

typedef struct {
    int to;
    double w;
} Edge;

typedef struct {
    char *name;         // original lowercased name
    Edge *edges;
    int edge_count;
    int edge_cap;
} Node;

static Node nodes[MAXN];
static int node_count = 0;

// Edge blocking for next-best path enumeration
static int block_u[MAXBLOCK];
static int block_v[MAXBLOCK];
static int block_count = 0;

// Dijkstra state
static double *distArr = NULL;
static int *parent = NULL;
static int current_source = -1;
static int current_target = -1;

// ---------------- String Hash Map (Open Addressing) ----------------
static char **hm_keys = NULL;
static int *hm_vals = NULL;
static unsigned char *hm_used = NULL;
static int hm_cap = 0;

static unsigned long long str_hash(const char *s) {
    unsigned long long h = 5381;
    for (; *s; ++s) h = ((h << 5) + h) + (unsigned char)(*s);
    return h;
}

static void hm_init(int cap) {
    hm_cap = 1;
    while (hm_cap < cap) hm_cap <<= 1;
    hm_keys = (char **)calloc(hm_cap, sizeof(char *));
    hm_vals = (int *)malloc(sizeof(int) * hm_cap);
    hm_used = (unsigned char *)calloc(hm_cap, sizeof(unsigned char));
}

static int hm_get(const char *k) {
    if (!hm_used) return -1;
    unsigned long long h = str_hash(k);
    int i = (int)(h & (hm_cap - 1));
    while (hm_used[i]) {
        if (strcmp(hm_keys[i], k) == 0) return hm_vals[i];
        i = (i + 1) & (hm_cap - 1);
    }
    return -1;
}

static void hm_put(const char *k, int val) {
    unsigned long long h = str_hash(k);
    int i = (int)(h & (hm_cap - 1));
    while (hm_used[i]) i = (i + 1) & (hm_cap - 1);
    hm_used[i] = 1;
    hm_keys[i] = strdup(k);
    hm_vals[i] = val;
}

// ---------------- Case Insensitive Copy ----------------
static void to_lower_copy(const char *src, char *dst, int dstsz) {
    int i = 0;
    for (; src[i] && i < dstsz - 1; ++i) dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

// ---------------- Node Helpers ----------------
static int add_node_by_name(const char *name_lower) {
    int idx = hm_get(name_lower);
    if (idx != -1) return idx;
    if (node_count >= MAXN) {
        fprintf(stderr, "Error: Exceeded MAXN nodes capacity (%d).\n", MAXN);
        return -1;
    }
    idx = node_count++;
    nodes[idx].name = strdup(name_lower);
    nodes[idx].edge_cap = 4;
    nodes[idx].edge_count = 0;
    nodes[idx].edges = (Edge *)malloc(sizeof(Edge) * nodes[idx].edge_cap);
    hm_put(name_lower, idx);
    return idx;
}

static void add_edge_idx(int u, int v, double w) {
    if (nodes[u].edge_count >= nodes[u].edge_cap) {
        nodes[u].edge_cap *= 2;
        nodes[u].edges = (Edge *)realloc(nodes[u].edges, sizeof(Edge) * nodes[u].edge_cap);
    }
    nodes[u].edges[nodes[u].edge_count++] = (Edge){v, w};
}

// ---------------- Block Check ----------------
static int is_blocked_idx(int u, int v) {
    for (int i = 0; i < block_count; ++i) {
        if (block_u[i] == u && block_v[i] == v) return 1;
    }
    return 0;
}

// ---------------- Binary Min-Heap Priority Queue ----------------
typedef struct {
    double d;
    int node;
} HItem;

typedef struct {
    HItem *a;
    int n, cap;
} MinHeap;

static void heap_init(MinHeap *h) {
    h->n = 0;
    h->cap = 256;
    h->a = (HItem *)malloc(sizeof(HItem) * h->cap);
}

static void heap_push(MinHeap *h, double d, int node) {
    if (h->n >= h->cap) {
        h->cap *= 2;
        h->a = (HItem *)realloc(h->a, sizeof(HItem) * h->cap);
    }
    int i = h->n++;
    h->a[i].d = d;
    h->a[i].node = node;
    while (i > 0) {
        int p = (i - 1) >> 1;
        if (h->a[p].d <= h->a[i].d) break;
        HItem t = h->a[p]; h->a[p] = h->a[i]; h->a[i] = t;
        i = p;
    }
}

static HItem heap_pop(MinHeap *h) {
    HItem r = h->a[0];
    h->a[0] = h->a[--h->n];
    int i = 0;
    while (1) {
        int l = i * 2 + 1, rgt = l + 1, s = i;
        if (l < h->n && h->a[l].d < h->a[s].d) s = l;
        if (rgt < h->n && h->a[rgt].d < h->a[s].d) s = rgt;
        if (s == i) break;
        HItem t = h->a[s]; h->a[s] = h->a[i]; h->a[i] = t;
        i = s;
    }
    return r;
}

// ---------------- Dijkstra Algorithm ----------------
static int dijkstra_indices(int src_idx, int tgt_idx) {
    if (!distArr) {
        distArr = (double *)malloc(sizeof(double) * MAXN);
        parent = (int *)malloc(sizeof(int) * MAXN);
    }
    for (int i = 0; i < node_count; ++i) {
        distArr[i] = INF;
        parent[i] = -1;
    }

    MinHeap pq;
    heap_init(&pq);
    distArr[src_idx] = 0.0;
    heap_push(&pq, 0.0, src_idx);

    while (pq.n) {
        HItem it = heap_pop(&pq);
        double d = it.d;
        int u = it.node;
        if (d > distArr[u]) continue;
        if (u == tgt_idx) break;

        for (int ei = 0; ei < nodes[u].edge_count; ++ei) {
            int v = nodes[u].edges[ei].to;
            double w = nodes[u].edges[ei].w;
            if (is_blocked_idx(u, v)) continue;
            double nd = d + w;
            if (nd + 1e-12 < distArr[v]) {
                distArr[v] = nd;
                parent[v] = u;
                heap_push(&pq, nd, v);
            }
        }
    }
    free(pq.a);

    return (parent[tgt_idx] != -1 || src_idx == tgt_idx);
}

// ---------------- Block Last Path Edges ----------------
static void block_last_path_indices(int tgt_idx) {
    int tmp[1024];
    int len = 0;
    int cur = tgt_idx;
    while (cur != -1 && len < 1024) {
        tmp[len++] = cur;
        cur = parent[cur];
    }
    for (int i = len - 1; i > 0; --i) {
        if (block_count < MAXBLOCK) {
            block_u[block_count] = tmp[i];
            block_v[block_count] = tmp[i - 1];
            ++block_count;
        }
    }
}

// ---------------- Public APIs ----------------

int load_graph(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return 0;

    free_graph();
    hm_init(HM_INIT);

    char line[8192];
    while (fgets(line, sizeof(line), f)) {
        char *tok = strtok(line, " \t\r\n");
        if (!tok) continue;
        char name_low[256];
        to_lower_copy(tok, name_low, sizeof(name_low));
        int u = add_node_by_name(name_low);
        if (u < 0) continue;

        while (1) {
            char *n = strtok(NULL, " \t\r\n");
            if (!n) break;
            char *w = strtok(NULL, " \t\r\n");
            if (!w) break;
            char neigh_low[256];
            to_lower_copy(n, neigh_low, sizeof(neigh_low));
            int v = add_node_by_name(neigh_low);
            if (v < 0) continue;
            double wd = atof(w);
            add_edge_idx(u, v, wd);
        }
    }

    fclose(f);
    return 1;
}

void free_graph(void) {
    for (int i = 0; i < node_count; ++i) {
        if (nodes[i].name) free(nodes[i].name);
        if (nodes[i].edges) free(nodes[i].edges);
    }
    node_count = 0;

    if (hm_keys) {
        for (int i = 0; i < hm_cap; ++i) {
            if (hm_used && hm_used[i] && hm_keys[i]) free(hm_keys[i]);
        }
        free(hm_keys); hm_keys = NULL;
    }
    if (hm_vals) { free(hm_vals); hm_vals = NULL; }
    if (hm_used) { free(hm_used); hm_used = NULL; }
    hm_cap = 0;

    if (distArr) { free(distArr); distArr = NULL; }
    if (parent) { free(parent); parent = NULL; }

    block_count = 0;
    current_source = -1;
    current_target = -1;
}

int find_shortest_path_by_name(const char *src_name, const char *tgt_name) {
    char s_low[256], t_low[256];
    to_lower_copy(src_name, s_low, sizeof(s_low));
    to_lower_copy(tgt_name, t_low, sizeof(t_low));

    int si = hm_get(s_low);
    int ti = hm_get(t_low);
    if (si < 0 || ti < 0) return 0;

    block_count = 0;
    current_source = si;
    current_target = ti;
    return dijkstra_indices(si, ti);
}

int find_shortest_path_by_index(int src_idx, int tgt_idx) {
    if (src_idx < 0 || tgt_idx < 0 || src_idx >= node_count || tgt_idx >= node_count) return 0;
    block_count = 0;
    current_source = src_idx;
    current_target = tgt_idx;
    return dijkstra_indices(src_idx, tgt_idx);
}

int find_next_best_path_current(void) {
    if (current_target < 0 || current_source < 0) return 0;
    block_last_path_indices(current_target);
    return dijkstra_indices(current_source, current_target);
}

double get_last_path_distance(void) {
    if (current_target < 0 || !distArr) return -1.0;
    return distArr[current_target];
}

void print_last_path(void) {
    if (current_target < 0) {
        printf("No path to display.\n");
        return;
    }

    int tmp[1024];
    int len = 0;
    int cur = current_target;
    while (cur != -1 && len < 1024) {
        tmp[len++] = cur;
        cur = parent[cur];
    }
    if (len == 0) {
        printf("No path to display.\n");
        return;
    }

    printf("\033[1;92mRoute: ");
    for (int i = len - 1; i >= 0; --i) {
        // Capitalize first letter for neat display
        char display_name[256];
        strncpy(display_name, nodes[tmp[i]].name, sizeof(display_name) - 1);
        display_name[sizeof(display_name) - 1] = '\0';
        if (display_name[0] >= 'a' && display_name[0] <= 'z') {
            display_name[0] = (char)(display_name[0] - 'a' + 'A');
        }
        printf("%s", display_name);
        if (i > 0) printf(" -> ");
    }
    printf("\n\033[1;94mTotal Distance: %.2f km\033[0m\n\n", distArr[current_target]);
}

void list_all_nodes(void) {
    printf("\n\033[1;36mAvailable Nodes (%d total):\033[0m\n", node_count);
    for (int i = 0; i < node_count; i++) {
        char display_name[256];
        strncpy(display_name, nodes[i].name, sizeof(display_name) - 1);
        display_name[sizeof(display_name) - 1] = '\0';
        if (display_name[0] >= 'a' && display_name[0] <= 'z') {
            display_name[0] = (char)(display_name[0] - 'a' + 'A');
        }
        printf("  [%2d] %-18s (Edges: %d)\n", i + 1, display_name, nodes[i].edge_count);
    }
    printf("\n");
}

int get_node_count(void) {
    return node_count;
}
