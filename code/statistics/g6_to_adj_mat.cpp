#include <fstream>
#include <string>
#include <iostream>

#include "../grph.cpp"
#include "../convert_graphs.cpp"
#include "../helpers.cpp"
#include "../gen.cpp"

int main() {
//    std::string g = "W???GOB?GI@O@?A?_A??_C?O_AA?GC?O@??P_?C__@GC?D?";
//    std::string g = "H@Q@YiW";

//    std::string g = "IsP@PGXD_";
//    std::string g = "QsP@PGXC?G?O?C?C?@O?Q?@c?D_";
//    std::string g = "SsP@PGWC_G?K?C?A?@??G??o?A_?Ag?@o";
//    std::string g = "SsP@PGWC_H?G?C?A??_?G??g?AO?BG?Ao";
//    std::string g = "SsP@PGWD?C?K?C?A?@??G??o?A_?Ag?@o";
//    std::string g = "SsP@PGWD_A?C?A?A??_?G??g?AO?BG?Ao";
//    std::string g = "SsP@PGXC?G?O?C?C?@O?O?@O?CO?Co?BG";
//    std::string g = "SsP@PGXC?G?O?C?C?@O?O?@_?CO?BG?Ao";
//    std::string g = "SsP@PGXC?G?O?C?C?@O?Q?@_?CO?AO?@K";
//    std::string g = "SsP@PGXC?G_O?C?A??_?G??g?AO?BG?Ao";

//    std::string g = "SsP@PGXC?G_O?C?A??_?G??g?AO?BG?Ao";
//    std::string g = "P???KA?AHAGGCGH@G`?SCCc?";
//    std::string g = "H@Q@YiW";
//
////SsP@PGXC?G_O?C?A??_?G??g?AO?BG?Ao (30): P???KA?AHAGGCGH@G`?SCCc? (23), H@Q@YiW (12),

/* flower snark a jeho tri podgrafy */
//    S?gQ@eOOGC?AP??BO@@?GB????o?E???[: R??G_KW@?O?OC?GDG@S?W_C_`?A@?G, R?CG_CC?GA@?C?GgGg@?SODA?EC?o?, R??GOOPC_g????OOoOQ?g_H?_K@GC?,
//    std::string g = "S?gQ@eOOGC?AP??BO@@?GB????o?E???[";
//    std::string g = "R??G_KW@?O?OC?GDG@S?W_C_`?A@?G";
//    std::string g = "R?CG_CC?GA@?C?GgGg@?SODA?EC?o?";
//    std::string g = "R??GOOPC_g????OOoOQ?g_H?_K@GC?";

//    std::string g = "U???WCC@?O`C?O?OO?`?OOC?O_?_Aa?DA?AGG?P?";
//    std::string g = "W???GOB?GI@O@?A?_A??_C?O_AA?GC?O@??P_?C__@GC?D?";

    std::string g = "W?HI@eO???C?_?OCa_?CP_???CGO?O?g??GC???w@_??@O@";
    std::cout << is_edge_colorable(graph6_to_graph(g)) << std::endl;
//
    grph graph = graph6_to_graph(g);
    print_2d_vector(graph.adj_matrix);

//    grph g;
//    g.num_of_vertices = 1;
//    g.adj_matrix = {{0}};

//    grph g;
//    g.num_of_vertices = 2;
//    g.adj_matrix = {{0, 1}, {1, 0}};

//    grph g;
//    g.num_of_vertices = 4;
//    g.adj_matrix = {{0, 1, 0, 1},
//                    {1, 0, 1, 0},
//                    {0, 1, 0, 1},
//                    {1, 0, 1, 0}};

//    grph g;
//    g.num_of_vertices = 12;
//    g.adj_matrix = {{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0},
//                    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
//                    {0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
//                    {0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0},
//                    {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
//                    {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0},
//                    {0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0},
//                    {0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
//                    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
//                    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
//                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}};
////
//    std::cout << graph_to_graph6(g) << std::endl;

//    std::cout << sparse6_to_graph6(canonical_form(graph6_to_graph("Ss??GOGA_I?c????GOQAACGO_P?_K@?S?"))) << std::endl;
}


