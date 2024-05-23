#ifndef BA_GRAPH_SNARKS_COLOURING_NAGY_HPP
#define BA_GRAPH_SNARKS_COLOURING_NAGY_HPP

#include "../invariants/girth.hpp"
#include "../operations/copies.hpp"
#include "../operations/add_graph.hpp"
#include "../operations/basic.hpp"
#include "reductions.hpp"

// ******************************** start of legacy code ********************************
//
// Created by Jan Karabas on 21/08/15.
//
// Modified: Michal Povinsky on 22/06/16.
//

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

namespace ba_graph::legacy::nagy
{

struct vars {
    char *const mmat;
    short (*const sus)[3];
    short *const jednohran;
    char *const poc_nez_hran;

    int poc_n_hran;
    int pocet_jn;
    int graph_size;
};

static struct vars graph_alloc(int size, char *mat)
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct vars graph = {
        .mmat = mat,
        .sus = (short (*)[3])malloc(sizeof(short) * 3 * size),
        .jednohran = (short *)malloc(sizeof(short) * size),
        .poc_nez_hran = (char *)malloc(sizeof(char) * size),
        .graph_size = size
    };
    #pragma GCC diagnostic pop

    return graph;
}

static void graph_free(struct vars *vars)
{
    free(vars->poc_nez_hran);
    free(vars->jednohran);
    free(vars->sus);
}
/* ! GLOBALS */

static inline char ttt(char a, char b, char c) {
    if ((a == b) && (a > 1)) return (0);
    if ((a == c) && (a > 1)) return (0);
    if ((c == b) && (c > 1)) return (0);
    return (1);
}

//static inline int idx(int size, int a, int b)
//{
//    return a*size+b;
//}

static inline char mmat_g(const struct vars *vars, int a, int b)
{
    return vars->mmat[a*vars->graph_size + b];
}

static inline void mmat_s2(const struct vars *vars, int a, int b, int v)
{
    vars->mmat[a*vars->graph_size+b] = v;
    vars->mmat[b*vars->graph_size+a] = v;
}

static inline int ttt2(const struct vars *vars, int kde, short *s)
{
    return ttt(mmat_g(vars, kde, s[0]), mmat_g(vars, kde, s[1]), mmat_g(vars, kde, s[2]));
}

static inline int cond1(const struct vars *vars, int kde1, int vrchol, int j)
{
    short *s = vars->sus[kde1];
    mmat_s2(vars, vrchol, kde1, j);
    return ttt2(vars, kde1, s);
}

static inline int cond2(const struct vars *vars, int kde1, int kde2)
{
    short *s1 = vars->sus[kde1], *s2 = vars->sus[kde2];
    return ttt2(vars, kde1, s1) && ttt2(vars, kde2, s2);
}

static int farbi(struct vars *vars, int vrchol) {
    static const int mozn[3][2] = {{4,5},{3,5},{3,4}};
    int i;
    int kde1 = 0, kde2 = 0, kolko = 0;
    int ulox;
    int dv;
    char zmena = 0;
    char zmen = 0;

    for (i = 0; i < 3; i++) {
        if (mmat_g(vars, vrchol, vars->sus[vrchol][i]) == 1) {
            kde2 = kde1;
            kde1 = vars->sus[vrchol][i];
        }
        else kolko += mmat_g(vars, vrchol, vars->sus[vrchol][i]);
    }

    switch (vars->poc_nez_hran[vrchol]) {
        case 0:
            if (vars->poc_n_hran == 0) return 1;
            else {
                i = 0;
                while ((vars->poc_nez_hran[i] != 1) && (vars->poc_nez_hran[i] != 2)) i++;
                ulox = farbi(vars, i);
                return (ulox);
            }

        case 1:
            vars->poc_n_hran--;
            vars->poc_nez_hran[vrchol]--;
            vars->poc_nez_hran[kde1]--;
            zmena = 0;
            if (vars->poc_nez_hran[kde1] == 1 && vars->pocet_jn != 0) {
                zmena = 1;
                dv = vars->jednohran[(vars->pocet_jn - 1)];
                --vars->pocet_jn;
            } else {
                dv = kde1;
            }

            switch (kolko) {
                case 0:
                    if (cond1(vars, kde1, vrchol, 3) && farbi(vars, dv)) return (1);
                    if (cond1(vars, kde1, vrchol, 4) && farbi(vars, dv)) return (1);
                    if (cond1(vars, kde1, vrchol, 5) && farbi(vars, dv)) return (1);
                    break;

                case 3:
                    if (cond1(vars, kde1, vrchol, 4) && farbi(vars, dv)) return (1);
                    if (cond1(vars, kde1, vrchol, 5) && farbi(vars, dv)) return (1);
                    break;

                case 4:
                    if (cond1(vars, kde1, vrchol, 3) && farbi(vars, dv)) return (1);
                    if (cond1(vars, kde1, vrchol, 5) && farbi(vars, dv)) return (1);
                    break;

                case 5:
                    if (cond1(vars, kde1, vrchol, 3) && farbi(vars, dv)) return (1);
                    if (cond1(vars, kde1, vrchol, 4) && farbi(vars, dv)) return (1);
                    break;

                default:
                    if (cond1(vars, kde1, vrchol, 12-kolko) && farbi(vars, dv)) return (1);
                    break;
            }
            if (zmena) {
                ++vars->pocet_jn;
                vars->jednohran[(vars->pocet_jn - 1)] = dv;
            }
            vars->poc_n_hran++;
            vars->poc_nez_hran[vrchol]++;
            vars->poc_nez_hran[kde1]++;
            mmat_s2(vars, vrchol, kde1, 1);
            return 0;
        case 2:
            vars->poc_n_hran -= 2;
            vars->poc_nez_hran[vrchol] -= 2;
            vars->poc_nez_hran[kde1]--;
            vars->poc_nez_hran[kde2]--;

            zmena = 0;
            if (vars->poc_nez_hran[kde1] == 1) {
                ++zmena;
                vars->jednohran[vars->pocet_jn] = kde1;
                ++vars->pocet_jn;
            }
            if (vars->poc_nez_hran[kde2] == 1) {
                ++zmena;
                vars->jednohran[vars->pocet_jn] = kde2;
                ++vars->pocet_jn;
            }
            if (vars->pocet_jn) {
                zmen = 1;
                dv = vars->jednohran[(vars->pocet_jn - 1)];
                --vars->pocet_jn;
            }
            else dv = kde1;

            switch (kolko) {
                case 0:
                    for (kolko = 3; kolko < 6; kolko++) {
                        mmat_s2(vars, vrchol, kde1, mozn[(kolko - 3)][0]);
                        mmat_s2(vars, vrchol, kde2, mozn[(kolko - 3)][1]);
                        if (cond2(vars, kde1, kde2) && farbi(vars, dv)) return (1);

                        mmat_s2(vars, vrchol, kde1, mozn[(kolko - 3)][1]);
                        mmat_s2(vars, vrchol, kde2, mozn[(kolko - 3)][0]);
                        if (cond2(vars, kde1, kde2) && farbi(vars, dv)) return (1);
                    }
                    break;

                default:
                    mmat_s2(vars, vrchol, kde1, mozn[(kolko - 3)][0]);
                    mmat_s2(vars, vrchol, kde2, mozn[(kolko - 3)][1]);
                    if (cond2(vars, kde1, kde2) && farbi(vars, dv)) return (1);

                    mmat_s2(vars, vrchol, kde1, mozn[(kolko - 3)][1]);
                    mmat_s2(vars, vrchol, kde2, mozn[(kolko - 3)][0]);
                    if (cond2(vars, kde1, kde2) && farbi(vars, dv)) return (1);
                    break;

            }
            if (zmena == 2)
                --vars->pocet_jn;
            if ((zmen == 1) && (zmena == 0)) {
                vars->jednohran[vars->pocet_jn] = dv;
                ++vars->pocet_jn;
            }

            vars->poc_n_hran += 2;
            vars->poc_nez_hran[vrchol] += 2;
            vars->poc_nez_hran[kde1]++;
            vars->poc_nez_hran[kde2]++;
            mmat_s2(vars, vrchol, kde1, 1);
            mmat_s2(vars, vrchol, kde2, 1);

            return (0);

        default: {
        } /* OUTPUT ONLY cprintf(" som tu, kde nemam byt.... \r\n"); */
            return (0);
    }
}

inline char nagy_colourise(char *mmatt, int grph_size) {
    struct vars vars_g = graph_alloc(grph_size, mmatt);
    struct vars *vars = &vars_g;
    static const int pol[] = {3,4,5};
    int i, l, k, kde = -1;
    int ulox;

    l = 0;
    do {
        k = 0;
        for (i = 0; i < grph_size; i++) {
            if (mmat_g(vars, l, i) == 1) {
                mmat_s2(vars, l, i, pol[k]);
                k++;
                kde = i;
            }
        }
        l++;
    } while (!k);

    vars->poc_n_hran = 0;

    for (i = 0; i < grph_size; i++) {
        l = 0;
        vars->poc_nez_hran[i] = 0;
        for (k = 0; k < grph_size; k++) {
            if (mmat_g(vars, i, k) > 0) {
                vars->sus[i][l] = k;
                l++;
                if (mmat_g(vars, i, k) == 1) {
                    ++vars->poc_nez_hran[i];
                    vars->poc_n_hran++;
                }
            }
        }

        if (l < 3) vars->sus[i][l] = i;
        l++;
        if (l < 3) vars->sus[i][l] = i;
    }
    vars->poc_n_hran = vars->poc_n_hran / 2;
    vars->pocet_jn = 0;
    for (i = 0; i < grph_size; i++) {
        if (vars->poc_nez_hran[i] == 1) {
            vars->jednohran[vars->pocet_jn] = i;
            vars->pocet_jn++;
        }
    }
    ulox = farbi(vars, kde);
//   for (int u = 0; u<grph_size; u++) {
//    	for(int v=0; v<grph_size; v++) {
//		fprintf(stderr,"%d ",mmat_g(vars,u,v));
//	}
//	printf("\n");
//    }

    // printf("DEBUG ulox %d\n",ulox);
    graph_free(vars);
    return (ulox);
}

} // namespace ba_graph::legacy::nagy
// ******************************** end of legacy code ********************************

namespace ba_graph
{

class NagyColouriser
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

        int length = H.order() * H.order();
        char *graph = new char[length];
        memset(graph, 0, length);
        for (auto ii : H.list(RP::all(), IP::all()))
            graph[ii->n1().to_int() * H.order() + ii->n2().to_int()] = 1;

        int result = legacy::nagy::nagy_colourise(graph, H.order());
        delete[] graph;
        return result;
    }
};

} // namespace ba_graph

#endif
