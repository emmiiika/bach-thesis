#ifndef BA_GRAPH_SNARKS_COLOURING_KOWALIK_HPP
#define BA_GRAPH_SNARKS_COLOURING_KOWALIK_HPP

#include "../invariants/girth.hpp"
#include "../operations/copies.hpp"
#include "../operations/add_graph.hpp"
#include "../operations/basic.hpp"
#include "reductions.hpp"

// ******************************** start of legacy code ********************************
//
// Created by Michal Povinsky on 04/08/16.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace ba_graph::legacy::kowalik
{

struct graph {
    int size;
    int (*nei)[3];
    short *deg;
    int *hid;
    int (*col)[3];
};

struct swtch {
    int x, u, v, y;
};

//static void showgraph(struct graph *g, struct graph *gs);

static struct graph graph_init(int s)
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct graph g = {
        .size = s,
        .nei = (int (*)[3])calloc(s*3, sizeof(int)),
        .deg = (short *)calloc(s, sizeof(short)),
        .hid = (int *)calloc(s, sizeof(int)),
        .col = NULL
    };
    #pragma GCC diagnostic pop

    return g;
}

static void graph_init_col(struct graph *g)
{
    assert(!g->col);
    g->col = (int (*)[3])calloc(g->size * 3, sizeof(int));
}

static void graph_copy(struct graph *d, struct graph *s)
{
    int size = s->size;
    memcpy(d->nei, s->nei, sizeof(int)*size*3);
    memcpy(d->deg, s->deg, sizeof(short)*size);
    memcpy(d->hid, s->hid, sizeof(int)*size);
}

static void graph_free(struct graph *g)
{
    free(g->nei);
    free(g->deg);
    free(g->hid);
    free(g->col);
}


static inline void hide(struct graph *g, int a)
{
    assert(g->deg[a] == 0 && g->hid[a] == 0);
    g->hid[a] = 1;
}

static inline void unhide(struct graph *g, int a)
{
    assert(g->hid[a] == 1);
    g->hid[a] = 0;
}

static inline void hide2(struct graph *g, int a, int b)
{
    hide(g, a);
    hide(g, b);
}

static inline void unhide2(struct graph *g, int a, int b)
{
    unhide(g, a);
    unhide(g, b);
}

static inline void hide3(struct graph *g, int a, int b, int c)
{
    hide(g, a);
    hide(g, b);
    hide(g, c);
}

static inline void unhide3(struct graph *g, int a, int b, int c)
{
    unhide(g, a);
    unhide(g, b);
    unhide(g, c);
}

static inline void nei_add(struct graph *g, int a, int b)
{
    assert(g->deg[a] < 3);
    if (g->col)
        g->col[a][g->deg[a]] = 0;
    g->nei[a][g->deg[a]++] = b;
}

static inline void nei_del(struct graph *g, int a, int b)
{
    assert(g->deg[a] > 0);
    assert(!g->col);
    for (int i = 0; i<g->deg[a]; i++) {
        if (g->nei[a][i] == b) {
            g->nei[a][i] = g->nei[a][--(g->deg[a])];
            return;
        }
    }
    fprintf(stderr, "tried to delete invalid neighbor %d %d\n", a, b);
    abort();
}

static inline void nei_del_c(struct graph *g, int a, int b, int c)
{
    assert(g->deg[a] > 0);
    for (int i = 0; i < g->deg[a]; i++) {
        if (g->nei[a][i] == b && g->col[a][i] == c) {
            g->nei[a][i] = g->nei[a][--(g->deg[a])];
            g->col[a][i] = g->col[a][g->deg[a]];
            return;
        }
    }
    fprintf(stderr, "tried to delete invalid color neighbor %d %d %d\n", a, b, c);
    abort();
}

static inline char mget(struct graph *g, int a, int b)
{
    int c = 0;
    for (int i=0; i<g->deg[a]; i++) {
        if (g->nei[a][i] == b) c++;
    }
    return c;
}

static inline int get_color(struct graph *g, int u, int v)
{
    for (int i = 0; i < g->deg[u]; i++) {
        if (g->nei[u][i] == v) return g->col[u][i];
    }
    fprintf(stderr, "get_color failed %d %d\n", u, v);
    abort();
}

static inline void add_edge(struct graph *g, int a, int b)
{
    assert(a != b && mget(g, a, b) < 3);
    nei_add(g, a, b);
    nei_add(g, b, a);
}

static inline void remove_edge(struct graph *g, int a, int b)
{
    assert(a != b && mget(g, a, b) > 0);
    if (g->col) {
        int col = get_color(g, a, b);
        nei_del_c(g, a, b, col);
        nei_del_c(g, b, a, col);
    } else {
        nei_del(g, a, b);
        nei_del(g, b, a);
    }
}

static inline void toggle_edge(struct graph *g, int a, int b)
{
    if (mget(g, a, b))
        remove_edge(g, a, b);
    else
        add_edge(g, a, b);
}

static inline void add_edge_double(struct graph *g, int a, int b)
{
    add_edge(g, a, b);
    add_edge(g, a, b);
}

static inline void remove_edge_double(struct graph *g, int a, int b)
{
    remove_edge(g, a, b);
    remove_edge(g, a, b);
}

static inline void add_edge_triple(struct graph *g, int a, int b)
{
    add_edge(g, a, b);
    add_edge(g, a, b);
    add_edge(g, a, b);
}

static inline void remove_edge_triple(struct graph *g, int a, int b)
{
    remove_edge(g, a, b);
    remove_edge(g, a, b);
    remove_edge(g, a, b);
}

//static inline void add_edge3(struct graph *g, int a, int b, int c)
//{
//    add_edge(g, a, b);
//    add_edge(g, b, c);
//}

static inline void add_edge4(struct graph *g, int a, int b, int c, int d)
{
    add_edge(g, a, b);
    add_edge(g, b, c);
    add_edge(g, c, d);
}

static inline void remove_edge4(struct graph *g, int a, int b, int c, int d)
{
    remove_edge(g, a, b);
    remove_edge(g, b, c);
    remove_edge(g, c, d);
}

static inline void add_edge5(struct graph *g, int a, int b, int c, int d, int e)
{
    add_edge(g, a, b);
    add_edge(g, b, c);
    add_edge(g, c, d);
    add_edge(g, d, e);
}

static inline void remove_edge5(struct graph *g, int a, int b, int c, int d, int e)
{
    remove_edge(g, a, b);
    remove_edge(g, b, c);
    remove_edge(g, c, d);
    remove_edge(g, d, e);
}

static inline int other_neigh(struct graph *g, int a, int b)
{
    for (int i = 0; i<g->deg[a]; i++) {
        int n = g->nei[a][i];
        if (n != b) return n;
    }
    assert(0);
    return 0;
}

static inline int other_neigh2(struct graph *g, int a, int b, int c)
{
    for (int i = 0; i<g->deg[a]; i++) {
        int n = g->nei[a][i];
        if (n != b && n != c) return n;
    }
    assert(0);
    return 0;
}

static inline int remove_neigh(struct graph *g, int a)
{
    if (g->deg[a] == 0) return -1;
    int b = g->nei[a][0];
    remove_edge(g, a, b);
    return b;
}

static inline void add_edge_maybe(struct graph *g, int a, int b)
{
    if (b < 0) return;
    add_edge(g, a, b);
}

static inline void color_neigh(struct graph *g, int u, int v, int color)
{
    for (int i = 0; i < 3; i++) {
        if (g->col[u][i] == 0 && g->nei[u][i] == v) {
            g->col[u][i] = color;
            return;
        }
    }
    fprintf(stderr, "color_neigh failed %d %d %d\n", u, v, color);
    abort();
}

static inline void color_edge(struct graph *g, int u, int v, int color)
{
    assert(color == 1 || color == 2 || color == 4);
    assert(u >= 0 && v >= 0);
    color_neigh(g, u, v, color);
    color_neigh(g, v, u, color);
}

static inline void uncolor_edge(struct graph *g, int u, int v, int color)
{
   for (int i = 0; i < g->deg[u]; i++)
       if (g->col[u][i] == color) g->col[u][i] = 0;
   for (int i = 0; i < g->deg[v]; i++)
       if (g->col[v][i] == color) g->col[v][i] = 0;
}

// Return all used colors for a vertex
static inline int vertex_colors(struct graph *g, int u)
{
    int c = 0;
    for (int i = 0; i < g->deg[u]; i++)
        c |= g->col[u][i];
    return c;
}

// Colors an edge with first unused color
static inline void color_edge_auto(struct graph *g, int u, int v)
{
    int c = 1, uc = vertex_colors(g, u) | vertex_colors(g, v);
    for (int i = 0; i < g->deg[u]; i++) {
        if (g->nei[u][i] != v || g->col[u][i]) continue;
        //if (uc == 7) { fprintf(stderr, "failed to color %d %d\n", u, v); showgraph(g, g); }
        assert(uc < 7);
        for (c = 1; c & uc; c <<= 1) {};
        color_edge(g, u, v, c);
        uc |= c;
    }
}

static int color_uncolored_(struct graph *g, int start)
{
   for (int u = start; u < g->size; u++) {
       if (g->hid[u]) continue;
       for (int vi = 0; vi < g->deg[u]; vi++) {
           int v = g->nei[u][vi];
           if (g->col[u][vi]) continue;
           int vc = vertex_colors(g, u) | vertex_colors(g, v);
           for (int c = 1; c < 8; c <<= 1) {
               if (c & vc) continue;
               color_edge(g, u, v, c);
               int ret = color_uncolored_(g, u);
               if (ret) return 1;
               uncolor_edge(g, u, v, c);
           }
           return 0;
       }
   }
   return 1;
}

// Color uncolored edges in a graph using brute force
static void color_uncolored(struct graph *g)
{
    //showgraph(g, g);
//    int ret = color_uncolored_(g, 0);
    assert(color_uncolored_(g, 0));
    //showgraph(g, g);
}

/*static void graphstat(struct graph *g)
{
    int vtx = 0, edg = 0;
    int degs[4] = { 0 };
    for (int i = 0; i < g->size; i++) {
        if (g->hid[i]) continue;
        vtx++;
        edg += g->deg[i];
        degs[g->deg[i]]++;
    }
    fprintf(stderr, "G: %dv %de [%d %d %d %d] ", vtx, edg, degs[0], degs[1], degs[2], degs[3]);
}
*/

//static void showgraph(struct graph *g, struct graph *gs)
//{
//    static char *colors[] = { "black", "red", "green", NULL, "blue" };
//    int nodes = 0;
//    for (int i = 0; i < g->size; i++) if (g->hid[i] == 0) nodes++;
//    if (nodes < 1) return;
//    //graphstat(g); graphstat(gs); fprintf(stderr, "\n");
//    //g = gs;
//    FILE *f = fopen("_g.dot", "w");
//    fprintf(f, "graph {\n");
//    fprintf(f, "overlap = false; \n splines = true; \n");
//    for (int u = 0; u < g->size; u++) {
//        if (g->hid[u]) continue;
//        if (gs->hid[u]) fprintf(f, "%d [style=dotted]\n", u);
//        else fprintf(f, "%d\n", u);
//        for (int vi = 0; vi < g->deg[u]; vi++) {
//            int v = g->nei[u][vi];
//            if (v > u) continue;
//            fprintf(f, "%d -- %d", u, v);
//            if (mget(gs, u, v) == 0) fprintf(f, "[style=dotted]");
//            if (g->col) fprintf(f, " [color=%s]", colors[g->col[u][vi]]);
//            fprintf(f, "\n");
//        }
//        for (int vi = 0; vi < gs->deg[u]; vi++) {
//            int v = gs->nei[u][vi];
//            if (u < v && mget(g, u, v) == 0) fprintf(f, "%d -- %d [color=red]\n", u, v);
//        }
//    }
//    fprintf(f, "}\n");
//    fclose(f);
//    system("sfdp -Tpng -o _g.png _g.dot && eom _g.png");
//}


static void graph_sub(struct graph *a, struct graph *b, struct graph *c)
{
    memset(a->deg, 0, a->size * sizeof(a->deg[0]));
    memcpy(a->hid, b->hid, a->size * sizeof(a->hid[0]));
    for (int u = 0; u < a->size; u++) {
        for (int vi = 0; vi < b->deg[u]; vi++) {
            int v = b->nei[u][vi];
            if (v > u) continue;
            if (mget(c, u, v) == 0)
                add_edge(a, u, v);
        }
    }
}

static int find_3cycle(struct graph *g, int *a, int *b, int *c)
{
    for (int x = 0; x<g->size; x++) {
        if (g->hid[x]) continue;
        if (g->deg[x] != 3) continue;
        for (int yi = 0; yi < 3; yi++) {
            int y = g->nei[x][yi];
            if (g->deg[y] != 3) continue;
            for (int zi = 0; zi < 3; zi++) {
                int z = g->nei[y][zi];
                if (z == x) continue;
                if (mget(g, z, x)) {
                    *a = x; *b = y; *c = z;
                    return 1;
                }
            }
        }
    }
    return 0;
}

static int find_xuzvy(struct graph *g, int *a, int *b, int *c, int *d, int *e)
{
    for (int x = 0; x < g->size; x++) {
        if (g->hid[x]) continue;
        if (g->deg[x] != 3) continue;
        for (int ui = 0; ui < 3; ui++) {
            int u = g->nei[x][ui];
            if (g->deg[u] != 2) continue;
            int z = other_neigh(g, u, x);
            if (g->deg[z] != 3) continue;
            for (int vi = 0; vi < 3; vi++) {
                int v = g->nei[z][vi];
                if (v==u) continue;
                if (g->deg[v] != 2) continue;
                int y = other_neigh(g, v, z);
                *a = x; *b = u; *c = z; *d = v; *e = y;
                return 1;
            }
        }
    }
    return 0;
}

static int find_4cycle(struct graph *g, int *a, int *b, int *c, int *d)
{
    for (int x = 0; x < g->size; x++) {
        if (g->hid[x]) continue;
        if (g->deg[x] != 3) continue;
        for (int yi = 0; yi < 3; yi++) {
            int y = g->nei[x][yi];
            if (g->deg[y] != 3) continue;
            for (int zi = 0; zi < 3; zi++) {
                int z = g->nei[y][zi];
                if (z == x || g->deg[z] != 3) continue;
                for (int ui = 0; ui < 3; ui++) {
                    int u = g->nei[z][ui];
                    if (u == y) continue;
                    if (mget(g, u, x)) {
                        *a = x; *b = y; *c = z; *d = u;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}


static int find_switch(struct graph *g0, struct graph *g, int start, struct swtch *sw)
{
    int x, u, v, y;
    if (g->deg[start] == 1) {
        x = start;
    } else if (g->deg[start] == 2) {
        int na = g->nei[start][0];
        int nb = g->nei[start][1];
        if (g->deg[na] == 1) x = na;
        else if (g->deg[nb] == 1) x = nb;
        else return 0;
    } else {
        return 0;
    }
    assert(g->deg[x] == 1);
    if (g0->deg[x] == 3) return 0;
    u = g->nei[x][0];
    if (g->deg[u] != 2 || g0->deg[u] < 3) return 0;
    v = other_neigh(g, u, x);
    if (g->deg[v] != 2 || g0->deg[v] < 3) return 0;
    y = other_neigh(g, v, u);
    if (g->deg[y] != 1 || g0->deg[y] == 3) return 0;
    if (sw) {
        sw->x = x; sw->u = u; sw->v = v; sw->y = y;
    }
    return 1;
}

static int is_switch(struct graph *g0, struct graph *g, int start)
{
    return find_switch(g0, g, start, NULL);
}

static int all_switches(struct graph *g0, struct graph *g)
{
    for (int i = 0; i < g->size; i++) {
        if (g->hid[i] == 0 && is_switch(g0, g, i) == 0) return 0;
    }
    return 1;
}

static inline int weight(struct graph *g, int a, int b)
{
    return g->deg[a] + g->deg[b];
}

static inline int allowed(struct graph *g0, struct graph *g, int u, int v)
{
    return (g0->deg[u] == 3 && g0->deg[v] == 3 && !is_switch(g0, g, u));
}

static inline void flip_switch(struct graph *g, struct swtch *sw)
{
    toggle_edge(g, sw->x, sw->u);
    toggle_edge(g, sw->u, sw->v);
    toggle_edge(g, sw->v, sw->y);
}

static struct swtch *find_switches(struct graph *g0, struct graph *g)
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Walloc-size-larger-than="
    struct swtch *sw = (struct swtch *)calloc((g->size + 4)/4, sizeof(sw[0]));
    int *marked = (int *)calloc(g->size, sizeof(int));
    #pragma GCC diagnostic pop

    int found = 0;
    for (int i = 0; i < g->size; i++) {
        if (g->hid[i] == 0 && marked[i] == 0 && find_switch(g0, g, i, &sw[found])) {
            if (i == sw[found].x && sw[found].y > i) found++;
        }
    }
    /*if (found > 0) {
        fprintf(stderr, "Found %d switches\n", found);
        showgraph(g0, g);
    }*/
    sw[found].x = -1; sw[found].u = -1; sw[found].v = -1; sw[found].y = -1;
    free(marked);
    return sw;
}

static int crossing(struct graph *g0m, struct swtch *a, struct swtch *b)
{
    //fprintf(stderr, "Crosing called with switches %d-%d-%d-%d %d-%d-%d-%d!\n", a->x, a->u, a->v, a->y, b->x, b->u, b->v, b->y);
    if (mget(g0m, a->u, a->v)) { return 0; }
    if (mget(g0m, b->u, b->v)) { return 0; }

    int cur = a->x, prev = -1, count = 0, found_y = 0, found_xb = 0, found_yb = 0;
    while (1) {
        if (g0m->deg[cur] != 2) return 0;
        int next = other_neigh(g0m, cur, prev);
        prev = cur; cur = next;
        if (cur == a->y) found_y = 1;
        if (cur == b->x) found_xb = 1;
        if (cur == b->y) found_yb = 1;
        if (!found_y && (cur == b->x || cur == b->y)) count++;
        if (next == a->x) {
            if (count == 0 || count == 2) return 0;
            if (found_y == 0 || found_xb == 0 || found_yb == 0) return 0;
            break;
        }
    }

    /*struct graph temp = graph_init(g0m->size);
    flip_switch(&temp, a);
    flip_switch(&temp, b);
    showgraph(g0m, &temp);
    graph_free(&temp);*/
    return 1;
}

static void count_cycles(struct graph *g, int *count, int *length)
{
    *count = 0;
    *length = 0;
    int *visit = (int *)malloc(g->size * sizeof(int));
    for (int i = 0; i < g->size; i++)
        visit[i] = 0;
    for (int i = 0; i < g->size; i++) {
        if (visit[i] || g->hid[i] || g->deg[i] == 0) continue;
        int clen = 0, cur = i, next = g->nei[i][0];
        while (cur != i || !clen) {
            visit[cur] = 1;
            int prev = cur;
            cur = next;
            if (g->deg[cur] == 1) {
                clen = -1;
                break;
            }
            assert(g->deg[cur] == 2);
            next = other_neigh(g, cur, prev);
            clen++;
        }
        if (clen % 2 && clen >= 0) {
            (*count)++;
            *length += clen;
        }
    }
    free(visit);
}

static int color_path(struct graph *g0, struct graph *g0m, int u, int v)
{
    int last = u, cur = v, ccol = 0;
    while (1) {
        if (ccol || get_color(g0, last, cur) == 0) color_edge(g0, last, cur, (ccol%2) ? 4 : 2);
        if (cur == u) return 1;
        if (g0m->deg[cur] == 1) return 0;
        int next = other_neigh(g0m, cur, last);
        last = cur; cur = next; ccol++;
    }
}

static void generate_col(struct graph *g0, struct graph *g0m)
{
    graph_init_col(g0);
    for (int u = 0; u < g0->size; u++) {
        if (g0->hid[u]) continue;
        for (int vi = 0; vi < g0->deg[u]; vi++) {
            int v = g0->nei[u][vi];
            if (mget(g0m, u, v) == 0) g0->col[u][vi] = 1;
            else if (g0->col[u][vi] == 0) {
                if (!color_path(g0, g0m, u, v))
                    color_path(g0, g0m, v, u);
            }
        }
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
// Which one of these implementations is correct?
static inline int set_switches_(struct graph *g0, struct graph *m, struct graph *g0m, struct swtch *sw)
{
    int cycnum, cyclen, cycnum2, cyclen2;
    count_cycles(g0m, &cycnum, &cyclen);

    while (1) {
        if (!cycnum) return 1;
        int brk = 0;
        for (int s = 0; sw[s].x >= 0; s++) {
            flip_switch(g0m, &sw[s]);
            count_cycles(g0m, &cycnum2, &cyclen2);
            if (cycnum2 < cycnum || (cyclen2 > cyclen && cycnum2 == cycnum)) { // is cycnum2 == cycnum correct?
                //fprintf(stderr, "Flipping %d-%d-%d-%d (%d %d) -> (%d %d)\n", sw[s].x, sw[s].u, sw[s].v, sw[s].y, cycnum, cyclen, cycnum2, cyclen2);
                cycnum = cycnum2; cyclen = cyclen2;
                brk = 1;
                break;
            }
            flip_switch(g0m, &sw[s]);
        }
        if (brk) continue;
        for (int s1 = 0; sw[s1].x >= 0; s1++) {
            for (int s2 = 0; s2 < s1; s2++) {
                if (!crossing(g0m, &sw[s1], &sw[s2])) continue;
                flip_switch(g0m, &sw[s1]);
                flip_switch(g0m, &sw[s2]);
                count_cycles(g0m, &cycnum2, &cyclen2);
                if (cycnum2 < cycnum || (cyclen2 > cyclen && cycnum2 == cycnum)) { // also here
                    //fprintf(stderr, "Flipping %d-%d-%d-%d %d-%d-%d-%d (%d %d) -> (%d %d)\n", sw[s1].x, sw[s1].u, sw[s1].v, sw[s1].y, sw[s2].x, sw[s2].u, sw[s2].v, sw[s2].y, cycnum, cyclen, cycnum2, cyclen2);
                    cycnum = cycnum2; cyclen = cyclen2;
                    brk = 1;
                    break;
                }
                flip_switch(g0m, &sw[s1]);
                flip_switch(g0m, &sw[s2]);
            }
            if (brk) break;
        }
        if (!brk) return 0;
    }
}

//static inline int set_switches_branching(struct graph *g0, struct graph *m, struct graph *g0m, struct swtch *sw)
//{
//    int cycnum, cyclen, cycnum2, cyclen2;
//    count_cycles(g0m, &cycnum, &cyclen);
//    if (!cycnum) return 1;
//    int ret = 0;
//    for (int s = 0; sw[s].x >= 0; s++) {
//        flip_switch(g0m, &sw[s]);
//        count_cycles(g0m, &cycnum2, &cyclen2);
//        if (cycnum2 < cycnum || cyclen2 < cyclen) {
//            ret = set_switches_branching(g0, m, g0m, sw);
//            if (ret) return ret;
//        }
//        flip_switch(g0m, &sw[s]);
//    }
//    for (int s1 = 0; sw[s1].x >= 0; s1++) {
//        for (int s2 = 0; s2 < s1; s2++) {
//            if (!crossing(g0m, &sw[s1], &sw[s2])) continue;
//            flip_switch(g0m, &sw[s1]);
//            flip_switch(g0m, &sw[s2]);
//            count_cycles(g0m, &cycnum2, &cyclen2);
//            if (cycnum2 < cycnum || cyclen2 < cyclen) {
//                ret = set_switches_branching(g0, m, g0m, sw);
//                if (ret) return ret;
//            }
//            flip_switch(g0m, &sw[s1]);
//            flip_switch(g0m, &sw[s2]);
//        }
//    }
//    return 0;
//}
#pragma GCC diagnostic pop

static int set_switches(struct graph *g0, struct graph *g, struct graph *m)
{
    struct swtch *switches = find_switches(g0, g);
    struct graph g0m = graph_init(g->size);
    graph_sub(&g0m, g0, m);
    for (int s = 0; switches[s].x >= 0; s++) {
        remove_edge(&g0m, switches[s].x, switches[s].u);
        remove_edge(&g0m, switches[s].v, switches[s].y);
    }
    int ret = set_switches_(g0, m, &g0m, switches);
    if (!ret) { // Test alternative set_switches if the first one fails
/*        graph_free(&g0m);
        g0m = graph_init(g->size);
        graph_sub(&g0m, g0, m);
        for (int s = 0; switches[s].x >= 0; s++) {
            remove_edge(&g0m, switches[s].x, switches[s].u);
            remove_edge(&g0m, switches[s].v, switches[s].y);
        }
        ret = set_switches_branching(g0, m, &g0m, switches);
        if (ret) {
            fprintf(stderr, "alternative set_switches returned a different result\n");
            showgraph(g0, g0);
        }*/
    }
    if (ret) {
        generate_col(g0, &g0m);
    }
    graph_free(&g0m);
    free(switches);
    return ret;
}

static int fitting_match(struct graph *g0, struct graph *g, struct graph *m)
{
    if (all_switches(g0, g)) {
        //fprintf(stderr, "FM All connected components are switches, calling set_switches\n"); showgraph(g0, g);
        return set_switches(g0, g, m);
    }
    for (int v = 0; v < g->size; v++)
        if (g->hid[v] == 0 && g->deg[v] == 0 && g0->deg[v] == 3) {
            //fprintf(stderr, "FM Found a degree 0 forced vertex %d\n", v);// showgraph(g0, g);
            return 0;
        }
    for (int v = 0; v < g->size; v++) {
        if (g->hid[v] == 0 && g->deg[v] == 0 && g0->deg[v] < 3) {
            //fprintf(stderr, "FM Found a degree 0 non-forced vertex %d\n", v); showgraph(g0, g);
            hide(g, v);
            int ret = fitting_match(g0, g, m);
            unhide(g, v);
            return ret;
        }
    }
    for (int v = 0; v < g->size; v++) {
        if (g->hid[v] == 0 && g->deg[v] == 1 && g0->deg[v] == 3) {
            //fprintf(stderr, "FM Found a degree 1 forced vertex %d\n", v); showgraph(g0, g);
            int u = g->nei[v][0];
            add_edge(m, u, v);
            remove_edge(g, u, v);
            int n1 = remove_neigh(g, u);
            int n2 = remove_neigh(g, u);
            hide2(g, u, v);
            int ret = fitting_match(g0, g, m);
            unhide2(g, u, v);
            add_edge_maybe(g, u, n1);
            add_edge_maybe(g, u, n2);
            add_edge(g, u, v);
            remove_edge(m, u, v);
            return ret;
        }
    }
    int max_weight = -1, uu = -1, vv = -1;
    for (int u = 0; u < g->size; u++) {
        if (g->hid[u]) continue;
        for (int vi = 0; vi < g->deg[u]; vi++) {
            int v = g->nei[u][vi];
            if (u != v && g->hid[v] == 0 && allowed(g0, g, u, v)) {
                int w = weight(g, u, v);
                if (w > max_weight) {
                    max_weight = w;
                    uu = u; vv = v;
                }
            }
        }
    }
    assert(max_weight >= 0);
    remove_edge(g, uu, vv);
    int nu1 = remove_neigh(g, uu);
    int nu2 = remove_neigh(g, uu);
    int nv1 = remove_neigh(g, vv);
    int nv2 = remove_neigh(g, vv);
    hide2(g, uu, vv);
    add_edge(m, uu, vv);
    int ret = fitting_match(g0, g, m);
    remove_edge(m, uu, vv);
    unhide2(g, uu, vv);
    add_edge_maybe(g, uu, nu1);
    add_edge_maybe(g, uu, nu2);
    add_edge_maybe(g, vv, nv1);
    add_edge_maybe(g, vv, nv2);
    if (!ret) ret = fitting_match(g0, g, m);
    add_edge(g, uu, vv);
    return ret;
}

static int edgecolor(struct graph *g)
{
    //showgraph(g, g);
    // remove vertices with degree 0 and 1
    for (int i=0; i<g->size; i++) {
        if (g->hid[i]) continue;
        if (g->deg[i] == 0) {
            //fprintf(stderr, "EC Removing vertex %d with degree 0\n", i); showgraph(g, g);
            hide(g, i);
            int ret = edgecolor(g);
            unhide(g, i);
            return ret;
        } else if (g->deg[i] == 1) {
            //fprintf(stderr, "EC Removing vertex %d with degree 1\n", i); showgraph(g, g);
            int j = g->nei[i][0];
            remove_edge(g, i, j);
            hide(g, i);
            int ret = edgecolor(g);
            unhide(g, i);
            add_edge(g, i, j);
            if (ret) color_edge_auto(g, i, j);
            return ret;
        }
    }
    // remove edge uv when deg(u) = deg(v) = 2
    for (int u=0; u<g->size; u++) {
        if (g->hid[u] || g->deg[u] != 2) continue;
        for (int vi = 0; vi < g->deg[u]; vi++) {
            int v = g->nei[u][vi];
            if (g->deg[v] != 2) continue;
            //fprintf(stderr, "EC removing 22 edge %d-%d\n", u, v); showgraph(g, g);
            remove_edge(g, u, v);
            int ret = edgecolor(g);
            add_edge(g, u, v);
            if (ret) color_edge_auto(g, u, v);
            return ret;
        }
    }
    // remove triple edge
    for (int u=0; u<g->size; u++) {
        if (g->hid[u]) continue;
        for (int vi = 0; vi < g->deg[u]; vi++) {
            int v = g->nei[u][vi];
            if (mget(g, u, v) != 3) continue;
            //fprintf(stderr, "EC removing triple edge %d-%d\n", u, v); showgraph(g, g);
            remove_edge_triple(g, u, v);
            hide2(g, u, v);
            int ret = edgecolor(g);
            unhide2(g, u, v);
            add_edge_triple(g, u, v);
            if (ret) color_edge_auto(g, u, v);
            return ret;
        }
    }
    // handle double edge
    for (int u=0; u<g->size; u++) {
        if (g->hid[u]) continue;
        for (int vi = 0; vi < g->deg[u]; vi++) {
            int v = g->nei[u][vi];
            if (mget(g, u, v) != 2) continue;
            //fprintf(stderr, "EC handling double edge %d-%d\n", u, v); showgraph(g, g);
            if (g->deg[u] == 2 || g->deg[v] == 2) {
                remove_edge_double(g, u, v);
                int u1 = remove_neigh(g, u);
                int v1 = remove_neigh(g, v);
                hide2(g, u, v);
                int ret = edgecolor(g);
                unhide2(g, u, v);
                add_edge_maybe(g, u, u1);
                add_edge_maybe(g, v, v1);
                add_edge_double(g, u, v);
                if (ret) {
                    color_uncolored(g);
                    //fprintf(stderr, "colored double edge %d %d\n", u, v); showgraph(g, g);
                }
                return ret;
            } else {
                int u1 = other_neigh(g, u, v);
                int v1 = other_neigh(g, v, u);
                if (u1 == v1) {
                    return 0;
                } else {
                    remove_edge_double(g, u, v);
                    remove_edge(g, u, u1);
                    remove_edge(g, v, v1);
                    add_edge(g, u1, v1);
                    hide2(g, u, v);
                    int ret = edgecolor(g);
                    unhide2(g, u, v);
                    remove_edge(g, u1, v1);
                    add_edge(g, u, u1);
                    add_edge(g, v, v1);
                    add_edge_double(g, u, v);
                    if (ret) {
                        color_uncolored(g);
                        //fprintf(stderr, "colored double edge %d %d\n", u, v); showgraph(g, g);
                    }
                    return ret;
                }
            }
        }
    }
    // 3-cycle
    {
        int u,v,w;
        if (find_3cycle(g, &w, &v, &u)) {
            //fprintf(stderr, "EC contracting 3-cycle %d-%d-%d\n", u, v, w); showgraph(g, g);
            int v1 = other_neigh2(g, v, u, w);
            int w1 = other_neigh2(g, w, u, v);
            remove_edge(g, v, v1);
            remove_edge(g, w, w1);
            remove_edge4(g, u, v, w, u);
            add_edge(g, u, v1);
            add_edge(g, u, w1);
            hide2(g, v, w);
            int ret = edgecolor(g);
            unhide2(g, v, w);
/*            if (ret) {
                fprintf(stderr, "3-cycle u=%d v1=%d w1=%d\n", u, v1, w1);
                showgraph(g, g);
            }*/
            remove_edge(g, u, v1);
            remove_edge(g, u, w1);
            add_edge(g, v, v1);
            add_edge(g, w, w1);
            add_edge4(g, u, v, w, u);
            if (ret) {
                color_uncolored(g);
                //fprintf(stderr, "colored 3-cycle %d %d %d\n", u, v, w); showgraph(g, g);
            }
            return ret;
        }
    }
    // xuzvy path
    {
        int x,u,z,v,y;
        if (find_xuzvy(g, &x, &u, &z, &v, &y)) {
            //fprintf(stderr, "EC found xuzvy %d-%d-%d-%d-%d\n", x, u, z, v, y); showgraph(g, g);
            int z1 = other_neigh2(g, z, u, v);
            remove_edge4(g, u, z, v, y);
            remove_edge(g, z, z1);
            hide2(g, z, v);
            add_edge(g, u, y);
            int  ret = edgecolor(g);
            remove_edge(g, u, y);
            if (!ret) {
                remove_edge(g, x, u);
                hide(g, u);
                add_edge(g, x, z1);
                ret = edgecolor(g);
                remove_edge(g, x, z1);
                unhide(g, u);
                add_edge(g, x, u);
            }
            unhide2(g, z, v);
            add_edge(g, z, z1);
            add_edge4(g, u, z, v, y);
            if (ret) {
                color_uncolored(g);
                //fprintf(stderr, "colored xuzvy %d %d %d %d %d\n", x, u, z, v, y); showgraph(g, g);
            }
            return ret;
        }
    }
    // xyzu cycle
    {
        int x,y,z,u, ret;
        if (find_4cycle(g, &x, &y, &z, &u)) {
            //fprintf(stderr, "EC found 4-cycle %d-%d-%d-%d\n", x, y, z, u); showgraph(g, g);
            int x1 = other_neigh2(g, x, y, u);
            int y1 = other_neigh2(g, y, x, z);
            int z1 = other_neigh2(g, z, y, u);
            remove_edge5(g, x, y, z, u, x);
            remove_edge(g, x, x1);
            remove_edge(g, y, y1);
            remove_edge(g, z, z1);
            hide3(g, x, y, z);
            if (g->deg[u] == 0) {
                hide(g, u);
                add_edge(g, x1, y1);
                ret = edgecolor(g);
                remove_edge(g, x1, y1);
                if (!ret) {
                    add_edge(g, y1, z1);
                    ret = edgecolor(g);
                    remove_edge(g, y1, z1);
                }
                unhide(g, u);
            } else {
                int u1 = other_neigh2(g, u, z, x);
                remove_edge(g, u, u1);
                hide(g, u);
                add_edge(g, x1, y1);
                add_edge(g, u1, z1);
                ret = edgecolor(g);
                remove_edge(g, u1, z1);
                remove_edge(g, x1, y1);
                if (!ret) {
                    add_edge(g, x1, u1);
                    add_edge(g, y1, z1);
                    ret = edgecolor(g);
                    remove_edge(g, y1, z1);
                    remove_edge(g, x1, u1);
                }
                unhide(g, u);
                add_edge(g, u, u1);
            }
            unhide3(g, x, y, z);
            add_edge5(g, x, y, z, u, x);
            add_edge(g, x, x1);
            add_edge(g, y, y1);
            add_edge(g, z, z1);
            if (ret) {
                color_uncolored(g);
                //fprintf(stderr, "colored 4-cycle %d %d %d %d\n", x, y, z, u); showgraph(g, g);
            }
            return ret;
        }
    }
    //fprintf(stderr, "Nothing to do, calling fitting_match\n"); showgraph(g, g);
    struct graph h = graph_init(g->size);
    struct graph m = graph_init(g->size);
    graph_copy(&h, g);
    int ret = fitting_match(g, &h, &m);
    graph_free(&h);
    graph_free(&m);
    return ret;
}

/*static void test_crossing()
{
    struct graph g = graph_init(15);
    struct swtch s1, s2;
    s1.x = 2; s1.u = 3; s1.v = 8; s1.y = 9;
    s2.x = 5; s2.u = 6; s2.v = 11; s2.y = 12;
    for (int i = 0; i < g.size; i++) {
        add_edge(&g, i, (i+1) % g.size);
    }
    showgraph(&g, &g);
    crossing(&g, &s1, &s2);
    graph_free(&g);
    exit(1);
}
*/

inline int verify(struct graph *g)
{
    for (int u = 0; u < g->size; u++) {
        int ucol = 0;
        for (int vi = 0; vi < g->deg[u]; vi++) {
            int v = g->nei[u][vi], c = g->col[u][vi];
            if (!c || (ucol & c)) return 0;
            ucol |= c;
            int found = 0;
            for (int ui = 0; ui < g->deg[v]; ui++) {
                if (g->nei[v][ui] == u && g->col[v][ui] == c) found = 1;
            }
            if (!found) return 0;
        }
    }
    return 1;
}

} // namespace ba_graph::legacy::kowalik
// ******************************** end of legacy code ********************************

namespace ba_graph {

class KowalikColouriser
{
  public:
    bool isColourable(const Graph &G) const
    {
        Factory f;
        Graph H(copy_other_factory(G, f));
        reduce_digons(H, f);
        if (has_loop(H)) return false;
        if (H.order() == 2) return true; // cubic graph of order 2 without a loop is a triple edge
        renumber_consecutive(H, f);

        struct legacy::kowalik::graph g = legacy::kowalik::graph_init(H.order());
        for (auto ii : H.list(RP::all(), IP::primary()))
            legacy::kowalik::add_edge(&g, ii->n1().to_int(), ii->n2().to_int());

        bool ret = legacy::kowalik::edgecolor(&g);
    /*
        if (ret) {
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < g.deg[i]; j++) {
                    static const int color[] = {-1, 3, 4, -1, 5};
                    mat[size*i + g.nei[i][j]] = color[g.col[i][j]];
                }
            }
        }
    */
        legacy::kowalik::graph_free(&g);
        return ret;
    }
};

} // namespace ba_graph

#endif
