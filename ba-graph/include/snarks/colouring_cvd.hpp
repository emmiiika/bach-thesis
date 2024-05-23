#ifndef BA_GRAPH_SNARKS_COLOURING_CVD_HPP
#define BA_GRAPH_SNARKS_COLOURING_CVD_HPP

#include "../invariants/colouring.hpp"
#include "../invariants/girth.hpp"
#include "../operations/copies.hpp"
#include "../operations/add_graph.hpp"
#include "../operations/basic.hpp"
#include "reductions.hpp"

// ******************************** start of legacy code ********************************
//
// Created by Michal Povinsky on 04/10/16.
// Modified: Jan Karabas on 31/03/17
//

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

namespace ba_graph::legacy::cvd
{

#ifdef __POPCNT__
#include <popcntintrin.h>
#endif

#define DEGREE 3
#define ALL_COLORS ((1<<DEGREE)-1)
#define FIRST_COLOR_INDEX 3

typedef uint32_t colorset;

struct set {
    int elems;
    int *elem;
};

struct graph {
    int size;
    int (*nei)[DEGREE];
    short *deg;
    colorset (*col)[DEGREE];
    int *dictpos, *oldcl;
    struct set dict[DEGREE-1];
    int *chain, *chain_used;
};

static struct graph graph_init(int s)
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct graph g = {
        .size = s,
        .nei = (int (*)[3])calloc(s*DEGREE, sizeof(int)),
        .deg = (short *)calloc(s, sizeof(short)),
        .col = (colorset (*)[3])calloc(s*DEGREE, sizeof(colorset)),
        .dictpos = (int *)calloc(s, sizeof(int)),
        .oldcl = (int *)calloc(s, sizeof(int)),
        .chain = (int *)calloc(s+1, sizeof(int)),
        .chain_used = (int *)calloc(s, sizeof(int))
    };
    #pragma GCC diagnostic pop

    for (int i=0; i<DEGREE-1; i++) {
        g.dict[i].elem = (int *)calloc(s, sizeof(int));
    }
    return g;
}

static void graph_free(struct graph *g)
{
    free(g->nei);
    free(g->deg);
    free(g->col);
    free(g->dictpos);
    free(g->oldcl);
    free(g->chain);
    free(g->chain_used);
    for (int i=0; i<DEGREE-1; i++) {
        free(g->dict[i].elem);
    }
}

/*static void showgraph(struct graph *g)
{
    static char *colors[] = { "black", "red", "green", NULL, "blue" };
    FILE *f = fopen("_g.dot", "w");
    fprintf(f, "graph {\n");
    fprintf(f, "overlap = false; \n splines = true; \n");
    for (int u = 0; u < g->size; u++) {
        fprintf(f, "%d\n", u);
        for (int vi = 0; vi < g->deg[u]; vi++) {
            int v = g->nei[u][vi];
            if (v > u) continue;
            fprintf(f, "%d -- %d [color=%s]\n", u, v, colors[g->col[u][vi]]);
        }
    }
    fprintf(f, "}\n");
    fclose(f);
    system("sfdp -Tpng -o _g.png _g.dot && eom _g.png");
}*/

static void add_edge(struct graph *g, int u, int v)
{
    //fprintf(stderr, "adding edge %d,%d\n", u, v);
    assert(g->deg[u] < DEGREE && g->deg[v] < DEGREE);
    g->nei[u][g->deg[u]++] = v;
    g->nei[v][g->deg[v]++] = u;
}

static int set_random(struct set *s)
{
    assert(s->elems > 0);
    return s->elem[rand() % s->elems];
}

static int popcount(colorset a)
{
#if defined (__POPCNT__)
    return _mm_popcnt_u32(a);
#else
    a = a - ((a >> 1) & 0x55555555);
    a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
    return (((a + (a >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
#endif
}

static int conflict_level(struct graph *g, int v)
{
    colorset colors = 0;
    for (int i = 0; i < g->deg[v]; i++) {
        colors |= g->col[v][i];
    }
    return g->deg[v] - popcount(colors);
}

/*static void printdict(struct graph *g)
{
    for (int i=1; i<DEGREE; i++) {
        fprintf(stderr, "CL[%d]={", i);
        for (int j=0; j<g->dict[i-1].elems; j++) {
            fprintf(stderr, "%d,", g->dict[i-1].elem[j]);
        }
        fprintf(stderr, "}\n");
    }
}*/

static void update_conflict_level(struct graph *g, int v)
{
    int newcl = conflict_level(g, v);
    int oldcl = g->oldcl[v];
    if (oldcl == newcl) return;
    //fprintf(stderr, "Updating conflict level for vertex %d from %d to %d\n", v, oldcl, newcl);
    if (oldcl > 0) {
        struct set *odict = &g->dict[oldcl-1];
        int odictpos = g->dictpos[v];
        //fprintf(stderr, "Removing %d from position %d\n", v, odictpos);
        assert(odictpos < odict->elems);
        assert(odict->elem[odictpos] == v);
        odict->elems--;
        if (odict->elems >= odictpos + 1) {
            //fprintf(stderr, "Had to update odictpos on %d to %d\n", odict->elem[odict->elems], odictpos);
            odict->elem[odictpos] = odict->elem[odict->elems];
            g->dictpos[odict->elem[odictpos]] = odictpos;
        }
    }
    if (newcl > 0) {
        struct set *ndict = &g->dict[newcl-1];
        ndict->elem[ndict->elems] = v;
        g->dictpos[v] = ndict->elems;
        ndict->elems++;
    }
    g->oldcl[v] = newcl;
    //printdict(g);
}

static colorset used_colors(struct graph *g, int u)
{
    colorset used = 0;
    for (int vi = 0; vi < g->deg[u]; vi++) {
        used |= g->col[u][vi];
    }
    return used;
}

static colorset conflicting_colors(struct graph *g, int u)
{
    colorset used = 0, confl = 0;
    for (int i = 0; i < g->deg[u]; i++) {
        colorset c = g->col[u][i];
        if (used & c) confl |= c;
        else used |= c;
    }
    return confl;
}

static colorset random_color(colorset avail)
{
    if (avail == 0 || avail == ALL_COLORS) return 1 << (rand() % DEGREE);
    if (popcount(avail) == 1) return avail;
    while (1) {
        int col = rand() % DEGREE;
        if (col & avail) return col;
    }
}

static colorset edge_color(struct graph *g, int u, int v)
{
    for (int i = 0; i < g->deg[u]; i++) {
        if (g->nei[u][i] == v) return g->col[u][i];
    }
    fprintf(stderr, "edge_color: edge doesn't exist\n");
    abort();
}

static void color_single(struct graph *g, int u, int v, colorset c)
{
    for (int i = 0; i < g->deg[u]; i++) {
        if (g->nei[u][i] == v) {
            g->col[u][i] = c;
            return;
        }
    }
}

static void color(struct graph *g, int u, int v, colorset c)
{
    color_single(g, u, v, c);
    color_single(g, v, u, c);
}

static int highest_conflict_level(struct graph *g)
{
    for (int i = DEGREE - 1; i > 0; i--) {
        if ((g->dict[i-1].elems)) return i;
    }
    return 0;
}

static int total_number_of_conflicts(struct graph *g)
{
    int sum = 0;
    for (int i = 1; i < DEGREE; i++)
        sum += i * g->dict[i-1].elems;
    return sum;
}

static void precolor(struct graph *g)
{
    for (int u = 0; u < g->size; u++) {
        for (int vi = 0; vi < g->deg[u]; vi++) {
            int v = g->nei[u][vi];
            colorset avail = ALL_COLORS ^ (used_colors(g, u) | used_colors(g, v));
            color(g, u, v, random_color(avail));
        }
    }
}

static int random_neigh_of_color(struct graph *g, int v, colorset col)
{
    assert(used_colors(g, v) & col);
    while (1) {
        int i = rand() % g->deg[v];
        if (col & g->col[v][i]) return g->nei[v][i];
    }
}


static void kempe(struct graph *g, int start)
{
    colorset cused = used_colors(g, start);
    colorset cconfl = conflicting_colors(g, start);
    colorset cfree = ALL_COLORS ^ cused;
    int *chain = g->chain, *chain_used = g->chain_used;
    int chain_length;
    colorset colors[2];
    chain[0] = start;
    //fprintf(stderr, "cused=%d cconfl=%d cfree=%d\n", cused, cconfl, cfree);
    chain[1] = random_neigh_of_color(g, start, cconfl);
    colors[0] = edge_color(g, start, chain[1]);
    colors[1] = random_color(cfree);
    chain_used[start] = chain_used[chain[1]] = 1;
    //fprintf(stderr, "chain start %d,%d colors %d,%d\n", chain[0], chain[1], colors[0], colors[1]);
    for (int i = 1; ; i++) {
        colorset next_used = used_colors(g, chain[i]);
        colorset next_color = colors[i%2];
        //fprintf(stderr, "chain %d next_used=%d next_color=%d\n", chain[i], next_used, next_color);
        if (!(next_used & next_color)) {
            chain_length = i;
            break;
        }
        chain[i+1] = random_neigh_of_color(g, chain[i], next_color);
        //fprintf(stderr, "next chain step %d -> %d\n", chain[i], chain[i+1]);
        if (chain_used[chain[i+1]]) {
            chain_length = i+1;
            break;
        }
        chain_used[chain[i+1]] = 1;
    }
/*    fprintf(stderr, "Chain [");
    for (int i = 0; i <= chain_length; i++) {
        fprintf(stderr, "%d%c", chain[i], i==chain_length ? ']' : ' ');
    }
    fprintf(stderr, " with colors %d %d\n", colors[0], colors[1]);
    showgraph(g);*/
    for (int i = 0; i < chain_length; i++) {
        color(g, chain[i], chain[i+1], colors[(i+1)%2]);
    }
    for (int i = 0; i <= chain_length; i++) {
        update_conflict_level(g, chain[i]);
    }
    for (int i = 0; i <= chain_length; i++) {
        chain_used[chain[i]] = 0;
    }
}

static int heuristic(struct graph *g, int replimit)
{
    //fprintf(stderr, "****************************** new iteration\n");
    for (int i = 0; i < DEGREE - 1; i++) {
        g->dict[i].elems = 0;
    }
    for (int i = 0; i < g->size; i++) {
        g->oldcl[i] = 0;
        for (int j = 0; j < g->deg[i]; j++) {
            g->col[i][j] = 0;
        }
    }
    precolor(g);
    for (int i = 0; i < g->size; i++) {
        update_conflict_level(g, i);
    }
    //printdict(g);
    int step = 0, repcount = 0, previous, current;
    current = total_number_of_conflicts(g);
    while (1) {
        /*fprintf(stderr, "step=%d rep=%d conflicts=%d [", step, repcount, current);
        for (int i = 0; i < DEGREE - 1; i++) fprintf(stderr, "%d ", g->dict[i].elems);
        fprintf(stderr, "]\n");*/
        if (current == 0) return 1;

        previous = current;
        int clevel = highest_conflict_level(g);
        struct set *dict = &(g->dict[clevel-1]);
        int chain_start = set_random(dict);
        //printdict(g);
        //fprintf(stderr, "highest_conflict_level = %d, %d conflicts, picked vertex %d with cl %d\n", clevel, dict->elems, chain_start, conflict_level(g, chain_start));
        kempe(g, chain_start);
        current = total_number_of_conflicts(g);
        if (current < previous) {
            repcount = 0;
        } else {
            repcount++;
            if (repcount > replimit) return 0;
        }
        step++;
    }
}

static int do_heuristic(struct graph *g, int iterlimit=-1, int replimit=-1)
{
    int ret = 0;
    // TODO fine-tune these limits maybe?
    if (replimit == -1)
        replimit = (int)(sqrt(g->size))+1;
    if (iterlimit == -1)
        iterlimit = 64;

    //printf("Graph size: %d\n",g->size);
    for (int i = 0; i < iterlimit; i++) {
        ret = heuristic(g, replimit);
        if (ret) {
            ret = i+1;
            break;
        }
    }
    return ret;
}

static int core_heuristic(struct graph *g, int iterlimit, int replimit)
{
    int ret = 0;//, replimit = (int)(sqrt(g->size))+1, iterlimit = 64;;

    //printf("Graph size: %d\n",g->size);
    for (int i = 0; i < iterlimit; i++) {
        ret = heuristic(g, replimit);
        if (ret) {
            ret = i+1;
            break;
        }
    }
    return ret;
}


static int color_to_idx(colorset c)
{
    int ci = 0;
    while (c && !(c & 1)) {
        ci++;
        c >>= 1;
    }
    assert(c == 1);
    return ci+1;
}


inline int color_cvd_param(char *mat, int size, int iterlimit, int replimit)
{
    int ret;
    struct graph g = graph_init(size);
    for (int u = 0; u < size; u++) {
        for (int v = 0; v < u; v++) {
            if (mat[u*size+v]) {
                add_edge(&g, u, v);
            }
        }
    }
    ret = core_heuristic(&g, iterlimit, replimit);
    if (ret) {
        for (int u = 0; u < size; u++) {
            for (int vi = 0; vi < g.deg[u]; vi++) {
                int v = g.nei[u][vi];
                mat[u*size+v] = mat[v*size+u] = color_to_idx(edge_color(&g, u, v));
            }
        }
    }
    graph_free(&g);
    return ret;
}

inline int color_cvd_adjlist(int *mat, int *colormat, int size)
{
    int ret;
    struct graph g = graph_init(size);
    for (int u = 0; u < size; u++) {
        for (int vi = 0; vi < DEGREE; vi++) {
            int v = mat[2*DEGREE*u+vi];
            if (v >= 0) add_edge(&g, u, v);
        }
    }
    ret = do_heuristic(&g);
    if (ret) {
        for (int u = 0; u < size; u++) {
            for (int vi = 0; vi < DEGREE; vi++) {
                int v = mat[DEGREE*u+vi];
                colormat[DEGREE*u+vi] = (v < 0) ? -1 : color_to_idx(edge_color(&g, u, v));
            }
        }
    }
    graph_free(&g);
    return ret;
}

} // namespace ba_graph::legacy::cvd
// ******************************** end of legacy code ********************************

namespace ba_graph {

inline bool is3EdgeColourable_cvd(const Graph &G, int iterationLimit=-1, int repetitionLimit=-1)
{
    Factory f;
    Graph H(copy_other_factory(G, f));
    reduce_digons(H, f);
    if (has_loop(H)) return false;
    if (H.order() == 2) return true; // cubic graph of order 2 without a loop is a triple edge
    renumber_consecutive(H, f);

    struct legacy::cvd::graph g = legacy::cvd::graph_init(H.order());
    for (auto &ii : H.list(RP::all(), IP::primary()))
        legacy::cvd::add_edge(&g, ii->n1().to_int(), ii->n2().to_int());

    int ret = legacy::cvd::do_heuristic(&g, iterationLimit, repetitionLimit);
    // if (ret) {
    //     for (int u = 0; u < size; u++) {
    //         for (int vi = 0; vi < g.deg[u]; vi++) {
    //             int v = g.nei[u][vi];
    //             mat[u*size+v] = mat[v*size+u] = color_to_idx(edge_color(&g, u, v));
    //         }
    //     }
    // }
    legacy::cvd::graph_free(&g);
    return ret;
}


class HeuristicsColouriser
{
    int iterationLimit;
    int repetitionLimit;
  public:
     // TODO choose appropriate default iterationLimit
    HeuristicsColouriser(int iterationLimit=5, int repetitionLimit=-1)
            : iterationLimit(iterationLimit), repetitionLimit(repetitionLimit) {}

    bool isColourable(const Graph &G) const
    {
        bool r = is3EdgeColourable_cvd(G, iterationLimit, repetitionLimit);
        if (r) return true;
        BasicColouriser<void> c;
        return c.isColourable(G);
    }
};


} // namespace ba_graph

#endif
