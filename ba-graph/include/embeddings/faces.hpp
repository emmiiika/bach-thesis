#ifndef BA_GRAPH_FACES_HPP
#define BA_GRAPH_FACES_HPP

namespace ba_graph {

/*
========== obsolete code

int faces(Graph &G)
{
    //we create EdgeLab with default value 0
    IterLab<int> visited(0);
    int faces=0;

    while(true) {
        //find unused Rotation::EdgeIterator
        Rotation::EdgeIterator it;
        for(auto &rot : G) {
            for(auto it2=rot.begin();it2!=rot.end();it2++)
                if (visited[it2]==0) {it=it2;break;}
            if (!it.isNull()) break; //just because we do not have double break
        }
        if (it.isNull()) break;


        //jump().next() until we return back
        Rotation::EdgeIterator it2=it;
        do {
            visited[it2]=1;
            it2=it2.jump().next();
        } while(it!=it2);

        faces++;
    }


    return faces;
}

*/

}  // namespace end

#endif
