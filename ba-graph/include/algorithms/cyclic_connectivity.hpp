#ifndef BA_GRAPH_INVARIANTS_CYCLIC_CONNECTIVITY_HPP
#define BA_GRAPH_INVARIANTS_CYCLIC_CONNECTIVITY_HPP

#include "../invariants/connectivity.hpp"
#include "../invariants/degree.hpp"
#include "../invariants/distance.hpp"
#include "../invariants/girth.hpp"
#include "matchings.hpp"
#include "../multipoles/multipoles.hpp"
#include "../operations/copies.hpp"
#include "../operations/undo.hpp"

#include <queue>
#include <iostream>
#include <cassert>
#include <ctime>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <queue>
#include <set>
#include <list>
#include <stack>
#include <fstream>
#include <array>
#include <string>
#include <sstream>
#include <cmath>


// ******************************** start of legacy code ********************************
//
// Created by Drahomir Mrozek in 2016, bachelor thesis.

namespace ba_graph::legacy::mrozek {

using namespace std;

typedef struct {
	int number;
	int *neighbors;
	//neighbors is array of .number of neighboring vertices
	int *flow;
	//flow is symmetrical, except for the 2 vertices we add before ford-fulkerson
	//capacity of every edge is 1
	int val; //valency
} Vertex;

typedef struct Part{
	std::vector<int> vertices;
	std::vector<int> cut;
	/* cut je mnozina hran v 1 moznom reze s mohutnostou maxflow.
	 * cut je zlozene z maxflow hran, i-ta hrana je medzi vrcholmi
	 * cut[i*2] a cut[i*2+1]  */
	bool operator < (const Part &other) const { return vertices < other.vertices; }

} Part;

typedef struct {
	int maxflow;
	/* part1 a part1 su na inkluziu minimalne komponenty
	* po rozdeleniach danych struktur (cyklus/strom) podla rezov
	* s mohutnostou maxflow. */
	Part P1,P2;
} Result;

typedef struct {
	int n;
	std::vector<Vertex> vertices;
	int girth;
	int connectivity;
} Graph;

inline Graph Gcopy(const Graph &G);
inline void destroyGraph(Graph *G);
inline void destroyVertex(Vertex &V);
inline bool adjacent(const Vertex &V1, const Vertex &V2);
inline std::vector<int> getAtoms(const std::vector<std::vector<int> > &parts, int n);
inline std::vector<int> getPart(const Graph &G,const Vertex &V,const std::vector<int> &cut);
inline std::vector<int> getCut(const Graph &G,const Vertex &V,  int forb);
inline std::vector<int> getCoAtoms(const Graph &G,const std::vector<int> &atom);
inline void setflow(Graph &G, int parent[], const int &currentn, Vertex &V1);

inline const int K = 3;

inline bool adjacent(const Vertex &V1, const Vertex &V2) {
	//ci su V1 a V2 susedne (ci V1 je susedom V2 a naopak)
	for (int i = 0; i < V1.val; i++) {
		if (V1.neighbors[i] == V2.number)
			return true;
	}
	return false;
}

inline void destroyVertex(Vertex &V) {
	delete[] V.neighbors;
	delete[] V.flow;
}

inline void destroyGraph(Graph *G) {
	for (int i = 0; i < G->n; i++)
		destroyVertex(G->vertices[i]);
	delete G;
}

inline Graph Gcopy(const Graph &G) {
	Graph G2;
	G2.girth = G.girth;
	G2.connectivity = G.connectivity;
	G2.n = G.n;
	for (int i = 0; i < G.n; i++) {
		Vertex V;
		V.number = i;

		V.neighbors = new int[G.vertices[i].val];
		V.flow = new int[G.vertices[i].val];
		for (int ii = 0; ii < G.vertices[i].val; ii++) {
			V.neighbors[ii] = G.vertices[i].neighbors[ii];
		}
		V.val = G.vertices[i].val;
		G2.vertices.push_back(V);
	}
	return G2;

}


inline void setflow(Graph &G, int parent[], const int &currentn, Vertex &V1) {
	//cout << currentn << " ";
	//nastavy toky vrcholov v G a V1 podla parent[]
	if (parent[currentn] == -1)
		return;
	Vertex currentV = G.vertices[currentn];
	Vertex lastV;
	if (parent[currentn] < G.n) {
		lastV = G.vertices[parent[currentn]];
	} else {
		lastV = V1;
	}
	for (int i = 0; i < currentV.val; i++) {
		if (currentV.neighbors[i] == lastV.number)
			currentV.flow[i]--;
	}
	for (int i = 0; i < lastV.val; i++) {
		if (lastV.neighbors[i] == currentV.number)
			lastV.flow[i]++;
	}
	setflow(G, parent, lastV.number, V1);
}


inline std::vector<int> getCut(const Graph &G, const Vertex &V, int forb) {
	/* Postup: zo vsetkych vrcholov komponentu 1 identifikovaneho
	 * imaginarnou hranou V1 sa snazime najst minimalny hranovy rez od V2.
	 * To robime tak ze najprov oznacime vsetky vrcholy v komponente 1 a
	 * z nich prehladavanim oznacime vsetky vrcholy do ktorych sa da dostat
	 * po hranach s nenaplnenou kapacitou (pricom maximalna kapacita je 1).
	 * Rez bude tvoreny vsetkymi hranami z oznacenych vrcholov do neoznacenych.
	 *
	 * Nespustam algoritmus z V1 pretoze potom by mohol oznacit niektore imaginarne hrany.
	 */
	queue<int> q;
	bool *marked = new bool[G.n + 1];
	for (int i = 0; i < G.n; i++)
		marked[i] = false;
	for (int i = 0; i < V.val; i++) {
		q.push(V.neighbors[i]);
		marked[V.neighbors[i]] = true;
	}
	//zaciatok oznacovania
	while (!q.empty()) {
		Vertex currentV;
		int x = q.front();
		q.pop();
		currentV = G.vertices[x];
		for (int i = 0; i < currentV.val; i++) {
			if (currentV.flow[i] != forb
					&& marked[currentV.neighbors[i]] == false) {
				marked[currentV.neighbors[i]] = true;
				q.push(currentV.neighbors[i]);
			}
		}
	}
	//zaciatok hladania rezu
	std::vector<int> cut;
	for (int i = 0; i < G.n; i++) {
		if (!marked[i])
			continue;
		Vertex currentV = G.vertices[i];
		for (int i = 0; i < currentV.val; i++) {

			if (!marked[currentV.neighbors[i]]) {
				cut.push_back(currentV.number);
				cut.push_back(currentV.neighbors[i]);

			}
		}
	}
	delete[] marked;
	//cout<<endl;
	return cut;

}

inline std::vector<int> getPart(const Graph &G, const Vertex &V, const std::vector<int> &cut) {
	/* Zaciname zo susedov V a hladame dalej az kym neoznacime
	 * vsetky vrcholy ku ktorym sa da dostat bez pouzitia cut.
	 * Kedze cut je rez tvoriaci najmensi komponent okolo
	 * susedov V, mal by nam vzniknut najmensi mozny part
	 * okolo tejto struktury(strom/cyklus)
	 */
	stack<int> s;
	bool *marked = new bool[G.n];
	for (int i = 0; i < G.n; i++)
		marked[i] = false;
	marked[V.neighbors[0]] = true;
	s.push(V.neighbors[0]);
	while (!s.empty()) {
		Vertex currentV = G.vertices[s.top()];
		s.pop();
		if (!marked[currentV.number]) {
			//cout<<"GAODGJPIQEJTIOPJADKLVNLJVNIPQEJHRPOQJRKOPQEJGIQEGNIOn8786716"<<endl<<endl<<endl<<endl;
			continue;
		}
		for (int i = 0; i < currentV.val; i++) {
			if (marked[currentV.neighbors[i]])
				continue;
			bool iscut = false;
			for (std::vector<int>::size_type ii = 0; ii < cut.size(); ii += 2) {
				if ((cut[ii] == currentV.number
						&& cut[ii + 1] == currentV.neighbors[i])
						|| (cut[ii] == currentV.neighbors[i]
								&& cut[ii + 1] == currentV.number)) {
					iscut = true;
					break;
				}
			}

			if (iscut)
				continue;
			marked[currentV.neighbors[i]] = true;
			s.push(currentV.neighbors[i]);

		}

	}
	std::vector<int> part;
	for (int i = 0; i < G.n; i++) {
		if (marked[i])
			part.push_back(i);
	}
	delete[] marked;
	return part;

}

inline std::vector<int> getAtoms(const std::vector<std::vector<int> > &parts, int n) {
	//returns vector<int> of which parts are atoms (have no superset in parts). if there are duplicate atoms returns the first one

	//contained[i] is the vector of parts that contain the vertex i;
	std::vector<std::vector<int>> contained;
	//contains[i] is the set of vertices in parts[i]. we use set because vector would take longer searching
	std::vector< std::set<int>> contains;
	for (int i = 0; i < n; i++) {
		std::vector<int> a;
		contained.push_back(a);
	}
	for (std::vector<std::vector<int> >::size_type i = 0; i < parts.size(); i++) {
		std::set<int> tmp;
		contains.push_back(tmp);
		for (std::vector<std::vector<int> >::size_type ii = 0; ii < parts[i].size(); ii++) {
			contained[parts[i][ii]].push_back(i);
			contains[i].insert(parts[i][ii]);

		}
	}

	bool *superset = new bool[parts.size()]; //superset[i]=true if parts[i] is a superset of another part
	for (std::vector<std::vector<int> >::size_type i = 0; i < parts.size(); i++)
		superset[i] = false;
	/*we will now put all candidates that share the first vertex with us into candidates
	 * for our supersets, and then for every other vertex in our part, if they dont have it,
	 * we remove them from candidates. At the end of this process, every remaining candidate will
	 * be our superset. Repeat for every part and we have found all supersets of every candidate,
	 * and the rest are inclusivelly minimal.*/
	for (std::vector<std::vector<int> >::size_type firstpart = 0; firstpart < parts.size(); firstpart++) {
		if (superset[firstpart])
			continue; //dont need to find a superset of superset since it would already be found
		vector<int> part1 = parts[firstpart];
		list<int> candidates; //parts that may be our supersets

		int v = part1[0];
		for (std::vector<std::vector<int> >::size_type container = 0; container < contained[v].size(); container++) {
			if (superset[contained[v][container]] == true)
				continue; //we already know its a superset
			if (contained[v][container] == (long)firstpart)
				continue; //we are allowed to overlap with ourselves
			if (parts[contained[v][container]].size() < part1.size())
				continue; //we cant be subset of smaller part
			candidates.push_back(contained[v][container]);
		}
		//if we dont share a vertex with a superset candidate, its not a superset and we can remove it
		for (std::vector<std::vector<int> >::size_type vertex_num = 1; vertex_num < part1.size(); vertex_num++) {
			for (list<int>::iterator it = candidates.begin();
					it != candidates.end(); it++) {
				if (contains[*it].count(part1[vertex_num]) == 0) {
					//cout<<"erasing "<<*it<<endl;
					list<int>::iterator tmp = it;
					it--;
					candidates.erase(tmp);
				}
			}
		}
		//now candidates are only our supersets
		for (list<int>::iterator it = candidates.begin();
				it != candidates.end(); it++) {
			/* It is possible that some parts are identical. if so, we want to
			 * mark all of them except the first one. fortunately, we start
			 * with the first one and mark all others, so those cant mark the first one*/
			superset[*it] = true;
		}

	}
	delete[] superset;
	//now we have identified all the supersets, lets return all that arent
	std::vector<int> atoms;
	for (std::vector<std::vector<int> >::size_type i = 0; i < parts.size(); i++) {
		if (!superset[i])
			atoms.push_back(i);
	}
	return atoms;

}

inline std::vector<int> getCoAtoms(const Graph &G,
		const std::vector<int> &atom) {
	/* Returns only the pairs of vertices on co-atoms that are not present in the atom,
	 * in a format similar to the format of cuts described in struct Part in common.h,
	 * vector [1,2,5,6] means two co-atoms, each consisting of the atom in arguments, the first
	 * co-atom also has vertices 1,2 ; the second has vertices 5,6.
	 */
	std::vector<int> coAtoms;
	//for every pair of vertices
	for (std::vector<int>::size_type i = 0; i < atom.size() - 1; i++) {
		for (std::vector<int>::size_type ii = i + 1; ii < atom.size(); ii++) {

			Vertex V1 = G.vertices[atom[i]];
			Vertex V2 = G.vertices[atom[ii]];

			/* lets find out if they are in co-atom, that is,
			 * if they have neighbors not in atom that are adjacent */
			//for every pair of neighbors
			for (int n1 = 0; n1 < V1.val; n1++) {
				//make sure they are not in atom, if they are, we can continue to next iteration.
				if (find(atom.begin(), atom.end(), V1.neighbors[n1])
						!= atom.end()) {
					continue;

				}

				for (int n2 = 0; n2 < V2.val; n2++) {
					if (find(atom.begin(), atom.end(), V2.neighbors[n2])
							!= atom.end()) {
						continue;
					}
					if (adjacent(G.vertices[V1.neighbors[n1]],
							G.vertices[V2.neighbors[n2]])) {
						//we have a co-atom
						coAtoms.push_back(V1.neighbors[n1]);
						coAtoms.push_back(V2.neighbors[n2]);
					}
				}
			}

		}
	}
	return coAtoms;
}

typedef struct FTreeNode {
	int number;
	int parent;
} FTreeNode;

inline void setflowT(Graph &G, int* parent, int &currentn) {

	//nastavi toky vrcholov v G podla parent[]
	if (parent[currentn] == -1)
		return;
	Vertex currentV = G.vertices[currentn];
	Vertex lastV = G.vertices[parent[currentn]];

	for (int i = 0; i < currentV.val; i++) {
		if (currentV.neighbors[i] == lastV.number)
			currentV.flow[i]--;
	}
	for (int i = 0; i < lastV.val; i++) {
		if (lastV.neighbors[i] == currentV.number)
			lastV.flow[i]++;
	}
	setflowT(G, parent, lastV.number);
}
inline bool bfsT(Graph &G, const FTreeNode &a, const std::set<int> &targets) {
	//hlada do sirky kym nenajde tokovu cestu z a do vrcholu z targets, a zavola setflowT na upravu prudov

	int *parent = new int[G.n];
	for (int i = 0; i < G.n; i++)
		parent[i] = -1;

	queue<int> q;
	q.push(a.number);
	std::set<int> nodes;
	while (!q.empty()) {
		Vertex currentV;
		int x = q.front();
		q.pop();

		currentV = G.vertices[x];

		for (int i = 0; i < currentV.val; i++) {
			if (targets.count(currentV.neighbors[i]) && currentV.flow[i] < 1) {
				//nasli sme cestu, mozeme bfs ukoncit
				parent[currentV.neighbors[i]] = currentV.number;
				setflowT(G, parent, currentV.neighbors[i]);
				delete[] parent;
				return true;

			}

			if (parent[currentV.neighbors[i]] == -1 && currentV.flow[i] < 1
					&& currentV.neighbors[i] != a.number) {

				parent[currentV.neighbors[i]] = x;
				q.push(currentV.neighbors[i]);

			}
		}
	}
	delete[] parent;
	return false;

}
inline Result ford_Fulk_T(Graph &G, std::vector<FTreeNode> &a, std::vector<FTreeNode> &b) {
	assert(a.size() == b.size());

	//pre kazdy treenode bol funkciou getFullTree pridany
	// prud do rodica , aby sme zrychlili hladanie maxflow

	int maxflow = 0;
	//flows in graph have already been initialized
	std::set<int> targets;
	for (std::vector<FTreeNode>::size_type i = 0; i < b.size(); i++) {
		targets.insert(b.at(i).number);
	}
	for (std::vector<FTreeNode>::size_type i = 0; i < a.size(); i++) {
		while (maxflow <= G.connectivity && bfsT(G, a.at(i), targets)) {
			maxflow++;
		}
	}

	Result R;
	R.maxflow = maxflow;
	if (R.maxflow > G.connectivity) {
		return R;
	}
	if ((a.size() == 1 && maxflow < 3)
			|| (a.size() > 1 && maxflow < (long)a.size() * 2)) {
		G.connectivity = maxflow;
		//vynulujeme prudy nastavene v getFullTree aby ich
		//nezachytilo hladanie rezu
		for (std::vector<FTreeNode>::size_type i = 0; i < a.size(); i++) {
			for (int y = 0; y < G.vertices[a.at(i).number].val; y++) {
				if (G.vertices[a.at(i).number].neighbors[y] == a.at(i).parent) {
					G.vertices[a.at(i).number].flow[y] = 0;
				}
			}
			for (int y = 0; y < G.vertices[b.at(i).number].val; y++) {
				if (G.vertices[b.at(i).number].neighbors[y] == b.at(i).parent) {
					G.vertices[b.at(i).number].flow[y] = 0;
				}
			}
		}
		Vertex V1, V2;
		V1.val = a.size();
		V2.val = b.size();
		V1.number = G.n;
		V1.neighbors = new int[a.size()];
		V1.flow = new int[a.size()];

		V2.number = G.n;
		V2.neighbors = new int[b.size()];
		V2.flow = new int[b.size()];

		for (std::vector<FTreeNode>::size_type i = 0; i < a.size(); i++) {
			V1.neighbors[i] = a.at(i).number;
		}

		for (std::vector<FTreeNode>::size_type i = 0; i < b.size(); i++) {
			V2.neighbors[i] = b.at(i).number;
		}
		R.P1.cut = getCut(G, V1, 1);
		R.P2.cut = getCut(G, V2, -1);
		R.P1.vertices = getPart(G, V1, R.P1.cut);
		R.P2.vertices = getPart(G, V2, R.P2.cut);
		destroyVertex(V1);
		destroyVertex(V2);
	}
	return R;

}

inline std::vector<FTreeNode> getFullTree(Graph &G, int &v, int depth, int prev) {
	//returns vector of ending vertices of tree. we can reconstruct tree, if needed throuh FTreeNode.parent
	//if prev=NULL we are starting a tree, else we are continuing from the node prev

	std::vector<FTreeNode> vector1;
	if (depth == 0) {
		FTreeNode t;
		t.number = v;
		t.parent = prev;
		vector1.push_back(t);
		for (int i = 0; i < G.vertices[v].val; i++) {
			if (G.vertices[v].neighbors[i] == prev) {
				//pripravíme prúd pre ford-fulkerson, kedze hladat cestu cez túto hranu je nezmyselné
				G.vertices[v].flow[i] = 1;
			}
		}
		return vector1;
	}

	for (int i = 0; i < G.vertices[v].val; i++) {
		if (G.vertices[v].neighbors[i] == prev) {
			continue;
		}
		std::vector<FTreeNode> tmp = getFullTree(G, G.vertices[v].neighbors[i],
				depth - 1, v);
		vector1.insert(vector1.end(), tmp.begin(), tmp.end());
	}
	return vector1;
}

inline std::tuple<Graph *, ba_graph::Graph, Mapping<Number>> ba_graph_to_mrozek(const ba_graph::Graph &g)
{
	// first, renumber consecutively and remember the mapping
	Factory f;
	ba_graph::Graph H(copy_other_factory(g, f));
	std::vector<Number> v;
	for (const auto &rot : H)
		v.push_back(rot.n());
	std::sort(v.begin(), v.end());
	Mapping<Number> originalToNew;
	int n = 0;
	for (const auto &x : v)
		originalToNew.set(g[x].n(), n++);
	renumber_vertices(H, originalToNew, f);
	Mapping<Number> newToOriginal;
	for (auto pp : originalToNew)
		newToOriginal.set(pp.second, pp.first);

	// next, create Mrozek graph from ours
	Graph *G = new Graph();
	G->n = H.order();
	for (int i = 0; i < G->n; i++) {
		Rotation &r = H[i];
		Vertex V;
		V.number = i;
		V.neighbors = new int[K];
		V.flow = new int [K];
		int next = 0;
		for (auto &i : r)
			V.neighbors[next++] = i.n2().to_int();
		V.val = 3;
		G->vertices.push_back(V);
	}
	return std::tuple(G, std::move(H), newToOriginal);
}

// returns -1 if there is no cycle separating cut or cyclic connectivity; the second entry is list of found minimal cuts
// the list is not necessarily complete, but contains all cuts separating non-trivial atoms
// (the list definitely does not contain any trivial cyclic parts, that is, circuits)
inline std::pair<int, std::set<std::set<Edge>>> main_mrozek(const ba_graph::Graph &g) {
	auto [G, Gba, newToOriginal] = ba_graph_to_mrozek(g);

	std::set<std::set<Edge>> cuts;
	G->girth = girth(Gba);

	if (G->girth > G->n / 2) {
		destroyGraph(G);
		return std::pair(-1, cuts);
	}
	G->connectivity = G->girth;
	std::set<Part> parts;
	//for every 2 vertices (x and y)
	for (int x = 0; x < G->n - 1; x++) {
		for (int y = x + 1; y < G->n; y++) {
			//std::cout << x << " " << y << std::endl << std::flush;
			int depth = -1;   //depth of full trees we are going to test
			int dist = distance(Gba, x, y); //distance between vertices x and y
			assert(dist > 0);
			do {
				depth++;
				if (depth * 2 + 1 > dist)
					break;//just so they dont overlap

				//reset flow for ford-fulk before getFullTree messes with it
				for (int i = 0; i < G->n; i++) {
					for (int ii = 0; ii < G->vertices[i].val; ii++) {
						G->vertices[i].flow[ii] = 0;
					}
				}
				std::vector<FTreeNode> tree1 = getFullTree(*G, x, depth, -1);
				std::vector<FTreeNode> tree2 = getFullTree(*G, y, depth, -1);
				Result R = ford_Fulk_T(*G, tree1, tree2);
				if (R.maxflow == 3 * pow(2, depth))
					continue;     //rez nieje cyklicky
				if (G->connectivity > R.maxflow) {
					G->connectivity = R.maxflow;
					parts.clear();
				}
				if (R.maxflow < G->girth && R.maxflow <= G->connectivity) {
					parts.insert(R.P1);
					parts.insert(R.P2);
				}
			} while (3 * pow(2, depth) < G->girth);
		}
	}

	for (auto part : parts) {
		std::set<Edge> transformedCut;
		auto cut = part.cut;
		for (unsigned ii = 0; ii < cut.size(); ii += 2) {
			Number u = newToOriginal.get(cut[ii]);
			Number v = newToOriginal.get(cut[ii + 1]);
			transformedCut.insert(g[u].find(v)->e());
		}
		cuts.insert(transformedCut);
	}

	int cc = G->connectivity;
	destroyGraph(G);
	return std::pair(cc, cuts);
}

inline bool main_mrozek_is_cc(const ba_graph::Graph &g, int cc) {
	auto [G, Gba, newToOriginal] = ba_graph_to_mrozek(g);

	G->girth = girth(Gba);
	if (G->girth > G->n / 2) {
		// no cycle-separating cuts for K_4, K_3,3, 
		destroyGraph(G);
		return true;
	}
	if (G->girth < cc) {
		destroyGraph(G);
		return false;
	}
	G->connectivity = G->girth;

	for (int x = 0; x < G->n - 1; x++) {
		for (int y = x + 1; y < G->n; y++) {
			int depth = -1;   //depth of full trees we are going to test
			int dist = distance(Gba, x, y); //distance between vertices x and y
			assert(dist > 0);
			do {
				depth++;
				if (depth * 2 + 1 > dist)
					break;//just so they dont overlap

				//reset flow for ford-fulk before getFullTree messes with it
				for (int i = 0; i < G->n; i++) {
					for (int ii = 0; ii < G->vertices[i].val; ii++) {
						G->vertices[i].flow[ii] = 0;
					}
				}
				std::vector<FTreeNode> tree1 = getFullTree(*G, x, depth, -1);
				std::vector<FTreeNode> tree2 = getFullTree(*G, y, depth, -1);
				Result R = ford_Fulk_T(*G, tree1, tree2);
				if (R.maxflow == 3 * pow(2, depth))
					continue;     //rez nieje cyklicky
				G->connectivity = std::min(G->connectivity, R.maxflow);
				if (G->connectivity < cc) {
					destroyGraph(G);
					return false;
				}	
			} while (3 * pow(2, depth) < G->girth);
		}
	}

	destroyGraph(G);
	return true;
}

} // namespace legacy
// ******************************** end of legacy code ********************************

namespace ba_graph {

inline int cyclic_connectivity(const Graph &G)
{
#ifdef BA_GRAPH_DEBUG
	assert(max_deg(G) == 3);
	assert(min_deg(G) == 3);
#endif
	return legacy::mrozek::main_mrozek(G).first;
}

inline bool is_cyclically_connected(const Graph &G, int k)
{
#ifdef BA_GRAPH_DEBUG
	assert(k >= 1);
	assert(max_deg(G) == 3);
	assert(min_deg(G) == 3);
#endif
	return legacy::mrozek::main_mrozek_is_cc(G, k);
}

inline std::pair<int, std::set<std::set<Edge>>> cyclic_connectivity_with_cuts(const Graph &G)
{
#ifdef BA_GRAPH_DEBUG
	assert(max_deg(G) == 3);
	assert(min_deg(G) == 3);
#endif
	return legacy::mrozek::main_mrozek(G);
}

// !!! FIXME this is incomplete, do not use unless you make sure it does what you want it to...
inline bool contains_cc_obstacle(const Graph &G, const Multipole &mG, int requiredCC)
{
#ifdef BA_GRAPH_DEBUG
	assert(mG.check_in(G));
	assert(mG.size() > 0);
	assert(mG[0].size() > 0);
	assert(max_deg(G) == 3);
	assert(requiredCC > 0);
#endif
	if (requiredCC == 1)
		return !is_connected(G);

	Multipole mGcopy = mG;
	mGcopy.flatten();
	const Connector &connector = mGcopy[0];
	int k = connector.size();

	int g = girth(G);
	if (g != -1) {
		if (k < requiredCC) return true; // small connector means small cut
		if (g < requiredCC) return true; // small girth means small cut FIXME not always, only if there is a cycle on the other side
	}
	// required cc and k are both at least 2

	Factory f;
	Graph H(copy_other_factory(G, f));

	std::vector<int> p(k - 1);
	for (int i = 0; i < k - 1; ++i)
		p[i] = i + 1;
	// connector[0] is fixed
	do {
		// skip half of the permutations since the circuit being attached is symmetric
		if (p[0] > p[k - 2]) continue;

		// next, we attach a circuit to the terminals
		std::set<Edge> addedEdges;
		for (int i = 0; i < k - 2; ++i)
			addedEdges.insert(addE(H, Location(connector[p[i]], connector[p[i + 1]]), f).e());
		addedEdges.insert(addE(H, Location(connector[p[k - 2]], connector[0]), f).e());
		addedEdges.insert(addE(H, Location(connector[p[0]], connector[0]), f).e());

		auto [cc, cuts] = cyclic_connectivity_with_cuts(H);
		if (cc >= requiredCC)
			return false; // could be completed into a graph with required cc, so no obstacles

		// every cut has size smaller than requiredCC
		std::set<Number> terminals(connector.numbers.begin(), connector.numbers.end());
		for (auto cut : cuts) {
			// if there is a cyclic part not containing any vertex of the added cycle (i.e. terminals),
			// it is an obstacle
			auto cleared = clear_edges(H, cut, f);
			auto cs = components(H);
			if (cs.size() < 2)
				continue;
			for (auto c : components(H)) {
				std::set<Number> vertices(c.begin(), c.end());
				std::vector<Number> r;
				std::set_intersection(vertices.begin(), vertices.end(),
						terminals.begin(), terminals.end(), std::back_inserter(r));
				if (r.size() == 0) {
					// the found cyclic part exists independently of the added circuit
					// thus is an obstacle
					return true;
				}
			}
			restore_edges(H, cleared, f);
		}

		for (Edge e : addedEdges)
			deleteE(H, e, f);

	} while (std::next_permutation(p.begin(), p.end()));

	// if we have not decided yet, we try to remove all edge cuts of size at most cc - 1
	for (int k = 0; k < requiredCC; ++k) {
		for (auto &m : all_matchings(H, k)) {
			auto cleared = clear_edges(H, m, f);
			auto cs = components(H);
			if (cs.size() >= 2) {
				// for each component C, if it contains a cycle, then
				// it must be true that (k + the number of dangling edges of C) >= cc
				for (auto c : cs) { // TODO if size > order of the component, it is cyclic, but we do not know size
					if (c.size() == 1) continue; // does not contain a cycle
					int danglingCount = 0;
					bool containsCycle = false;
					for (auto v : c) {
						if (G[v].degree() == 1)
							danglingCount++;
						if (shortest_cycle_length(H, v) != -1)
							containsCycle = true;
					}
					if (containsCycle && (k + danglingCount < requiredCC))
						return true;
				}
			}
			restore_edges(H, cleared, f);
		}
	}
	return false;
}

}  // namespace end

#endif
