//#ifndef STATS
//#define STATS
//
//#include "stat_helpers.cpp"
//
///* To run this file time and subgraph files must be generated. */
//void get_stats() {
////    std::string type = "triv";
//    std::string types[] = {"cubic", "snark", "small-cubic", "small-snark"};
//    for (std::string type: types) {
//        int SIZE;
//
//        std::vector<std::string> orders;
//
//        std::string pre_time;
//        std::string post_time;
//        std::string pre_out_time;
//        std::string post_out_time;
//
//        std::string pre_noRedun;
//        std::string post_noRedun;
//        std::string pre_out_noRedun;
//        std::string post_out_noRedun;
//
//        std::string pre_noRedunSorted;
//        std::string post_noRedunSorted;
//        std::string pre_out_noRedunSorted;
//        std::string post_out_noRedunSorted;
//
//        std::string pre_all_in_one;
//        std::string post_all_in_one;
//        std::string out_all_in_one;
//
//        std::string pre_all_orig_in_one;
//        std::string post_all_orig_in_one;
//        std::string out_all_orig_in_one;
//
//        std::string filter_merged_subg;
//        std::string out_filter_merged_subg;
//
//        std::string vertices_origin;
//        std::string out_vertices_origin;
//
//        std::string girth_origin;
//        std::string out_girth_origin;
//
//        std::string edges_origin;
//        std::string out_edges_origin;
//
//        std::string vertices_origin_diff;
//        std::string edges_origin_diff;
//
//        if (type == "cubic") {
//            orders = {"10", "12", "14", "16", "18", "20"};
//            SIZE = 6;
//
//            pre_time = "iofiles/cubic-graphs/times/cub";
//            post_time = "-gir5-time.g6";
//            pre_out_time = "iofiles/cubic-graphs/times/cub";
//            post_out_time = "-gir5-time-summary.txt";
//
//            pre_noRedun = "iofiles/cubic-graphs/subgraphs/cub";
//            post_noRedun = "-gir5-subgraphs.g6";
//            pre_out_noRedun = "iofiles/cubic-graphs/redundancyFree/cub";
//            post_out_noRedun = "-gir5-noRedun.g6";
//
//            pre_noRedunSorted = pre_noRedun;
//            post_noRedunSorted = post_noRedun;
//            pre_out_noRedunSorted = "iofiles/cubic-graphs/redundancyFreeSorted/cub";
//            post_out_noRedunSorted = "-gir5-noRedunSorted.g6";
//
//            pre_all_in_one = pre_out_noRedun;
//            post_all_in_one = post_out_noRedun;
//            out_all_in_one = "iofiles/cubic-graphs/general/cub-gir5-allInOne.g6";
//
//            pre_all_orig_in_one = "iofiles/cubic-graphs/cubic/cub";
//            post_all_orig_in_one = "-gir5.g6";
//            out_all_orig_in_one = "iofiles/cubic-graphs/general/cub-gir5-allGraphsInOne.g6";
//
//            filter_merged_subg = out_all_in_one;
//            out_filter_merged_subg = "iofiles/cubic-graphs/general/cub-gir5-noRedunAllInOne.g6";
//
//            vertices_origin = "iofiles/cubic-graphs/origin.txt";
//            out_vertices_origin = "iofiles/cubic-graphs/origin-vertices.txt";
//
//            girth_origin = vertices_origin;
//            out_girth_origin = "iofiles/cubic-graphs/origin-girth.txt";
//
//            edges_origin = girth_origin;
//            out_edges_origin = "iofiles/cubic-graphs/origin-edges.txt";
//
//            vertices_origin_diff = edges_origin;
//            edges_origin_diff = edges_origin;
//
//        } else if (type == "snark") {
//            orders = {"10", "18", "20", "22", "24", "26", "28"};
//            SIZE = 6;
//
//            pre_time = "iofiles/snarks-graphs/snarks-times/Generated_graphs.";
//            post_time = ".05.sn.cyc4-time.g6";
//            pre_out_time = "iofiles/snarks-graphs/snarks-times/";
//            post_out_time = "-snark-time-summary.txt";
//
//            pre_noRedun = "iofiles/snarks-graphs/snarks-subgraphs/Generated_graphs.";
//            post_noRedun = ".05.sn.cyc4-subgraph.g6";
//            pre_out_noRedun = "iofiles/snarks-graphs/snarks-redundancyFree/";
//            post_out_noRedun = "-snark-subgraphs-noRedun.txt";
//
//            pre_noRedunSorted = pre_noRedun;
//            post_noRedunSorted = post_noRedun;
//            pre_out_noRedunSorted = "iofiles/snarks-graphs/snarks-redundancyFreeSorted/";
//            post_out_noRedunSorted = "-snark-subgraphs-noRedunSorted.txt";
//
//            pre_all_in_one = pre_out_noRedun;
//            post_all_in_one = post_out_noRedun;
//            out_all_in_one = "iofiles/snarks-graphs/snarks-general/snark-allInOne.g6";
//
//            pre_all_orig_in_one = "iofiles/snarks-graphs/snarks/Generated_graphs.";
//            post_all_orig_in_one = ".05.sn.cyc4.g6";
//            out_all_orig_in_one = "iofiles/snarks-graphs/snarks-general/snark-allGraphsInOne.g6";
//
//            filter_merged_subg = out_all_in_one;
//            out_filter_merged_subg = "iofiles/snarks-graphs/snarks-general/snark-noRedunAllInOne.g6";
//
//            vertices_origin = "iofiles/snarks-graphs/origin.txt";
//            out_vertices_origin = "iofiles/snarks-graphs/origin-vertices.txt";
//
//            girth_origin = vertices_origin;
//            out_girth_origin = "iofiles/snarks-graphs/origin-girth.txt";
//
//            edges_origin = girth_origin;
//            out_edges_origin = "iofiles/snarks-graphs/origin-edges.txt";
//
//            vertices_origin_diff = edges_origin;
//            edges_origin_diff = edges_origin;
//
//        } else if (type == "small-cubic") {
//            orders = {"6", "8", "10", "12", "14", "16", "18", "20"};
//            SIZE = 8;
//
//            pre_time = "iofiles/smaller-cubic-graphs/smaller-cubic-times/Generated_graphs.";
//            post_time = ".04-time.g6";
//            pre_out_time = "iofiles/smaller-cubic-graphs/smaller-cubic-times/";
//            post_out_time = "-small-cubic-time-summary.txt";
//
//            pre_noRedun = "iofiles/smaller-cubic-graphs/smaller-cubic-subgraphs/Generated_graphs.";
//            post_noRedun = ".04-subgraph.g6";
//            pre_out_noRedun = "iofiles/smaller-cubic-graphs/smaller-cubic-redundancyFree/";
//            post_out_noRedun = "-small-cubic-subgraphs-noRedun.txt";
//
//            pre_noRedunSorted = pre_noRedun;
//            post_noRedunSorted = post_noRedun;
//            pre_out_noRedunSorted = "iofiles/smaller-cubic-graphs/smaller-cubic-redundancyFreeSorted/";
//            post_out_noRedunSorted = "-small-cubic-subgraphs-noRedunSorted.txt";
//
//            pre_all_in_one = pre_out_noRedun;
//            post_all_in_one = post_out_noRedun;
//            out_all_in_one = "iofiles/smaller-cubic-graphs/smaller-cubic-general/small-cubic-allInOne.g6";
//
//            pre_all_orig_in_one = "iofiles/smaller-cubic-graphs/smaller-cubic/Generated_graphs.";
//            post_all_orig_in_one = ".04.g6";
//            out_all_orig_in_one = "iofiles/smaller-cubic-graphs/smaller-cubic-general/small-cubic-allGraphsInOne.g6";
//
//            filter_merged_subg = out_all_in_one;
//            out_filter_merged_subg = "iofiles/smaller-cubic-graphs/smaller-cubic-general/small-cubic-noRedunAllInOne.g6";
//
//            vertices_origin = "iofiles/smaller-cubic-graphs/origin.txt";
//            out_vertices_origin = "iofiles/smaller-cubic-graphs/origin-vertices.txt";
//
//            girth_origin = vertices_origin;
//            out_girth_origin = "iofiles/smaller-cubic-graphs/origin-girth.txt";
//
//            edges_origin = vertices_origin;
//            out_edges_origin = "iofiles/smaller-cubic-graphs/origin-edges.txt";
//
//            vertices_origin_diff = edges_origin;
//            edges_origin_diff = edges_origin;
//
//        } else if(type == "small-snark"){
//            orders = {"10", "18", "20", "22"};
//            SIZE = 4;
//
//            pre_time = "iofiles/smaller-snarks-graphs/smaller-snarks-times/Generated_graphs.";
//            post_time = ".04.sn.cyc4-time.g6";
//            pre_out_time = "iofiles/smaller-snarks-graphs/smaller-snarks-times/";
//            post_out_time = "-small-snark-time-summary.txt";
//
//            pre_noRedun = "iofiles/smaller-snarks-graphs/smaller-snarks-subgraphs/Generated_graphs.";
//            post_noRedun = ".04.sn.cyc4-subgraph.g6";
//            pre_out_noRedun = "iofiles/smaller-snarks-graphs/smaller-snarks-redundancyFree/";
//            post_out_noRedun = "-small-snark-subgraphs-noRedun.txt";
//
//            pre_noRedunSorted = pre_noRedun;
//            post_noRedunSorted = post_noRedun;
//            pre_out_noRedunSorted = "iofiles/smaller-snarks-graphs/smaller-snarks-redundancyFreeSorted/";
//            post_out_noRedunSorted = "-small-snark-subgraphs-noRedunSorted.txt";
//
//            pre_all_in_one = pre_out_noRedun;
//            post_all_in_one = post_out_noRedun;
//            out_all_in_one = "iofiles/smaller-snarks-graphs/smaller-snarks-general/small-snark-allInOne.g6";
//
//            pre_all_orig_in_one = "iofiles/smaller-snarks-graphs/smaller-snarks/Generated_graphs.";
//            post_all_orig_in_one = ".04.sn.cyc4.g6";
//            out_all_orig_in_one = "iofiles/smaller-snarks-graphs/smaller-snarks-general/small-snark-allGraphsInOne.g6";
//
//            filter_merged_subg = out_all_in_one;
//            out_filter_merged_subg = "iofiles/smaller-snarks-graphs/smaller-snarks-general/small-snark-noRedunAllInOne.g6";
//
//            vertices_origin = "iofiles/smaller-snarks-graphs/origin.txt";
//            out_vertices_origin = "iofiles/smaller-snarks-graphs/origin-vertices.txt";
//
//            girth_origin = vertices_origin;
//            out_girth_origin = "iofiles/smaller-snarks-graphs/origin-girth.txt";
//
//            edges_origin = vertices_origin;
//            out_edges_origin = "iofiles/smaller-snarks-graphs/origin-edges.txt";
//
//            vertices_origin_diff = edges_origin;
//            edges_origin_diff = edges_origin;
//        }
//
//        std::string in_files_filtered[SIZE];
//        std::string in_files_original[SIZE];
//        for (int i = 0; i < SIZE; i++) {
//            std::string order = orders[i];
//            sum_times_per_file(pre_time + order + post_time, pre_out_time + order + post_out_time);
//            filter_redundant_per_file(pre_noRedun + order + post_noRedun, pre_out_noRedun + order + post_out_noRedun);
//            filter_redundant_sorted_per_file(pre_noRedunSorted + order + post_noRedunSorted,
//                                             pre_out_noRedunSorted + order + post_out_noRedunSorted);
//
//            in_files_filtered[i] = pre_all_in_one + order + post_all_in_one;
//            in_files_original[i] = pre_all_orig_in_one + order + post_all_orig_in_one;
//
//        }
//        merge_filtered_files(SIZE, in_files_filtered, out_all_in_one);
//        merge_original_files(SIZE, in_files_original, out_all_orig_in_one);
//
//        for (int i = 0; i < SIZE; i++) {
//            std::string order = orders[i];
//            filter_merged_subgraphs(filter_merged_subg, out_filter_merged_subg);
//        }
//        sort_by_num_of_vertices(out_filter_merged_subg, type);
//        add_num_of_vertices_to_origin_file(vertices_origin, out_vertices_origin);
//
//        sort_by_girth(out_filter_merged_subg, type);
//        add_girth_to_origin_file(girth_origin, out_girth_origin);
//
//        sort_by_num_of_edges(out_filter_merged_subg, type);
//        add_num_of_edges_to_origin_file(edges_origin, out_edges_origin);
//
//        vertices_diff(vertices_origin_diff, type);
//        edges_diff(edges_origin_diff, type);
//    }
//}
//
//#endif