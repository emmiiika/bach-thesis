#ifndef BA_GRAPH_ALGORITHMS_ISOMORPHISM_ISOMORPHISM_NAUTY_HPP
#define BA_GRAPH_ALGORITHMS_ISOMORPHISM_ISOMORPHISM_NAUTY_HPP

#include <map>
#include <stdlib.h>
#include <gtools.h>
#include <nausparse.h>

#include "../../invariants/girth.hpp"
#include "../../io/graph6.hpp"

using namespace ba_graph;

/**
 * Returns a sparse6 string of G in canonical form computed by nauty.
 *
 * DOES NOT WORK FOR GRAPHS WITH PARALLEL EDGES AND LOOPS
 */
inline std::string canonical_sparse6(const Graph &G)
{
#ifdef BA_GRAPH_DEBUG
	assert(!has_loop(G));
	assert(!has_parallel_edge(G));
#endif
	std::string s = write_sparse6(G, false);
	char *Gs6 = new char[s.length() + 1];
	std::copy(s.begin(), s.end(), Gs6);
	Gs6[s.length()] = '\0';

	sparsegraph* g = (sparsegraph*)malloc(sizeof(sparsegraph));
    SG_INIT((*g));
    int num_loops;
    stringtosparsegraph(Gs6, g, &num_loops);
    delete[] Gs6;

	int n = g->nv; // G.order()

    int m = (n + WORDSIZE - 1) / WORDSIZE;
    nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

    DYNALLSTAT(int, lab, lab_n);
    DYNALLSTAT(int, ptn, ptn_n);
    DYNALLSTAT(int, orbits, orbits_n);
    DYNALLOC1(int, lab, lab_n, n, "malloc");
    DYNALLOC1(int, ptn, ptn_n, n, "malloc");
    DYNALLOC1(int, orbits, orbits_n, n, "malloc");

	static DEFAULTOPTIONS_SPARSEGRAPH( options);
    options.defaultptn = TRUE;
    options.getcanon = TRUE;
	options.digraph = FALSE;

    DYNALLSTAT(setword, workspace, worksize);
    DYNALLOC1(setword, workspace, worksize, 50 * m, "malloc");

    sparsegraph canonical_g;
    SG_INIT(canonical_g);

    statsblk stats;
    nauty((graph*) g, lab, ptn, NULL, orbits, &options, &stats, workspace, 50 * m, m, n, (graph*) &canonical_g);
    sortlists_sg(&canonical_g);

	std::string result(sgtos6(&canonical_g));

    DYNFREE(workspace, worksize);
    DYNFREE(lab,lab_n);
    DYNFREE(ptn,ptn_n);
    DYNFREE(orbits,orbits_n);
    SG_FREE(canonical_g);
    SG_FREE((*g));
    free(g);

	result.pop_back();
	return result;
}

// TODO edge orbits?

/**
 * Returns a list of orbits computed by nauty. Assumes that the graph G has vertices numbered contiguously from 0.
 *
 * DOES NOT WORK FOR GRAPHS WITH PARALLEL EDGES AND LOOPS
 */
inline std::map<Number, Number> orbits_nauty(const Graph &G)
{
#ifdef BA_GRAPH_DEBUG
	assert(!has_loop(G));
	assert(!has_parallel_edge(G));
#endif
	std::string s = write_sparse6(G, false);
	char *Gs6 = new char[s.length() + 1];
	std::copy(s.begin(), s.end(), Gs6);
	Gs6[s.length()] = '\0';

	sparsegraph* g = (sparsegraph*)malloc(sizeof(sparsegraph));
    SG_INIT((*g));
    int num_loops;
    stringtosparsegraph(Gs6, g, &num_loops);
    delete[] Gs6;

	int n = g->nv; // G.order()

    int m = (n + WORDSIZE - 1) / WORDSIZE;
    nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

    DYNALLSTAT(int, lab, lab_n);
    DYNALLSTAT(int, ptn, ptn_n);
    DYNALLSTAT(int, orbits, orbits_n);
    DYNALLOC1(int, lab, lab_n, n, "malloc");
    DYNALLOC1(int, ptn, ptn_n, n, "malloc");
    DYNALLOC1(int, orbits, orbits_n, n, "malloc");

	static DEFAULTOPTIONS_SPARSEGRAPH( options);
    options.defaultptn = TRUE;
    options.getcanon = TRUE;
	options.digraph = FALSE;

    DYNALLSTAT(setword, workspace, worksize);
    DYNALLOC1(setword, workspace, worksize, 50 * m, "malloc");

    sparsegraph canonical_g;
    SG_INIT(canonical_g);

    statsblk stats;
    nauty((graph*) g, lab, ptn, NULL, orbits, &options, &stats, workspace, 50 * m, m, n, (graph*) &canonical_g);

	std::map<Number, Number> result;
    for (int i = 0; i < n; ++i)
        result[i] = orbits[i];

    DYNFREE(workspace, worksize);
    DYNFREE(lab,lab_n);
    DYNFREE(ptn,ptn_n);
    DYNFREE(orbits,orbits_n);
    SG_FREE(canonical_g);
    SG_FREE((*g));
    free(g);

	return result;
}

inline bool are_isomorphic_nauty(const Graph &G, const Graph &H)
{
	auto Gs6 = canonical_sparse6(G);
	auto Hs6 = canonical_sparse6(H);
	return Gs6 == Hs6;
}

#endif
