#include <impl/basic/include.hpp>
#include <graphs/snarks.hpp>
#include <snarks/colouring_bit_array.hpp>

#include <cassert>
#include <iostream>

using namespace ba_graph;

int main() {

/***********
ColouringBitArray::Index
************/

    ColouringBitArray::Index i1(5, 18);
    i1+=ColouringBitArray::Index(2,10);
    assert(i1==ColouringBitArray::Index(8,1));
    i1-=ColouringBitArray::Index(1,3);
    assert(i1==ColouringBitArray::Index(6,25));
    assert((++i1)==ColouringBitArray::Index(6,26));
    assert((i1++)==ColouringBitArray::Index(6,26));
    assert(i1==ColouringBitArray::Index(7,0));
    assert((i1--)==ColouringBitArray::Index(7,0));
    assert(i1==ColouringBitArray::Index(6,26));
    assert((--i1)==ColouringBitArray::Index(6,25));
    assert(i1==ColouringBitArray::Index(6,25));
    i1.add_to_special(ColouringBitArray::Index(0,2));
    assert(i1==ColouringBitArray::Index(7,0));

    assert(ColouringBitArray::Index(7,6) < ColouringBitArray::Index(8,2));
    assert(!(ColouringBitArray::Index(6,6) < ColouringBitArray::Index(6,5)));
    assert(!(ColouringBitArray::Index(7,2) < ColouringBitArray::Index(6,5)));
    assert(!(ColouringBitArray::Index(7,2) < ColouringBitArray::Index(7,2)));
    assert(!(ColouringBitArray::Index(7,6) == ColouringBitArray::Index(7,2)));
    assert(ColouringBitArray::Index(7,6) != ColouringBitArray::Index(7,2));
    assert(ColouringBitArray::Index(7,6) != ColouringBitArray::Index(2,6));
    assert(ColouringBitArray::Index(7,6) != ColouringBitArray::Index(6,7));
    assert(!(ColouringBitArray::Index(3,6) != ColouringBitArray::Index(3,6)));
    assert(ColouringBitArray::Index(7,6) <= ColouringBitArray::Index(8,2));
    assert(!(ColouringBitArray::Index(6,6) <= ColouringBitArray::Index(6,5)));
    assert(!(ColouringBitArray::Index(7,2) <= ColouringBitArray::Index(6,5)));
    assert(ColouringBitArray::Index(7,2) <= ColouringBitArray::Index(7,2));
   

/***********
ColouringBitArray::pow3
************/

    assert(ColouringBitArray::pow3[2] == ColouringBitArray::Index(0, 9));
    assert(ColouringBitArray::pow3[3] == ColouringBitArray::Index(1, 0));
    ColouringBitArray::Index in=ColouringBitArray::pow3[30];
    ++in;
    assert(in.first()==7625597484987 && in.second()==1); //3^27

    for(int i=0;i<27+1; i++) ++in;
    assert(in.first()==7625597484988 && in.second()==2);

    in.add_to_special(ColouringBitArray::Index(10, 25));
    assert(in.first()==7625597484999 && in.second()==0);


    assert(!ColouringBitArray::Index(7,0).is_power());
    assert(ColouringBitArray::Index(243*3,0).is_power());
    assert(ColouringBitArray::Index(1,0).is_power());
    assert(!ColouringBitArray::Index(1,3).is_power());
    assert(!ColouringBitArray::Index(0,0).is_power());
    assert(ColouringBitArray::Index(0,1).is_power());
    assert(!ColouringBitArray::Index(0,2).is_power());
    assert(ColouringBitArray::Index(0,9).is_power());
    assert(ColouringBitArray::Index(7625597484987,0).is_power());

    assert(ColouringBitArray::Index(0,1).get_power()==0);
    assert(ColouringBitArray::Index(0,9).get_power()==2);
    assert(ColouringBitArray::Index(1,0).get_power()==3);
    assert(ColouringBitArray::Index(81,0).get_power()==7);

    assert(ColouringBitArray::Index(7,43).is_power_multiple(0));
    assert(!ColouringBitArray::Index(7,43).is_power_multiple(1));
    assert(ColouringBitArray::Index(7,45).is_power_multiple(1));
    assert(ColouringBitArray::Index(7,0).is_power_multiple(2));
    assert(ColouringBitArray::Index(7,81).is_power_multiple(2));
    assert(!ColouringBitArray::Index(7,82).is_power_multiple(2));
    assert(ColouringBitArray::Index(33,0).is_power_multiple(4));
    assert(!ColouringBitArray::Index(33,1).is_power_multiple(4));
    assert(!ColouringBitArray::Index(34,0).is_power_multiple(4));


/***********
ColouringBitArray
************/

    ColouringBitArray ar(243*243, ColouringBitArray::F);
    assert(!ar[ColouringBitArray::Index(3,5)]);
    assert(!ar[ColouringBitArray::Index(3,26)]);
    ColouringBitArray ar2(243*243, ColouringBitArray::T);
    assert(ar2[ColouringBitArray::Index(3,5)]);
    assert(ar2[ColouringBitArray::Index(3,26)]);
   
    ar.set(ColouringBitArray::Index(3,5), true);
    assert(ar[ColouringBitArray::Index(3,5)]);
    assert(!ar[ColouringBitArray::Index(3,4)]);
    assert(!ar[ColouringBitArray::Index(3,6)]);

    ar.set(ColouringBitArray::Index(3,26), true);
    assert(ar[ColouringBitArray::Index(3,26)]);
    assert(!ar[ColouringBitArray::Index(3,25)]);
    ar.set(ColouringBitArray::Index(3,26), false);
    assert(!ar[ColouringBitArray::Index(3,26)]);

//all_true, all_false
    ar=ColouringBitArray(ColouringBitArray::Index(3,23), ColouringBitArray::F);
    assert(ar.all_false());
    assert(!ar.all_true());
    ar.set(ColouringBitArray::Index(3,22), true);
    assert(!ar.all_false());
    assert(!ar.all_true());
    ar2=ColouringBitArray(ColouringBitArray::Index(3,23), ColouringBitArray::T);
    assert(!ar2.all_false());
    assert(ar2.all_true());
    ar2.set(ColouringBitArray::Index(3,22), false);
    assert(!ar2.all_false());
    assert(!ar2.all_true());

//==, !=
    ar2=ar;
    assert(ar==ar2);
    ar2.set(ColouringBitArray::Index(3,20), true);
    assert(ar!=ar2);
    ar.set(ColouringBitArray::Index(3,20), true);
    assert(ar==ar2);

//concatedante special
    ar=ColouringBitArray(ColouringBitArray::Index(2,22), false);
    ar2=ColouringBitArray(ColouringBitArray::Index(0,1), true);
    ar.concatenate_to_special(ar2);
    assert(ar.size()==ColouringBitArray::Index(2,23));
    assert(!ar[ColouringBitArray::Index(2,21)]);
    assert(ar[ColouringBitArray::Index(2,22)]);

    ar=ColouringBitArray(ColouringBitArray::Index(2,22), true);
    ar2=ColouringBitArray(ColouringBitArray::Index(0,1), false);
    ar.concatenate_to_special(ar2);
    assert(ar.size()==ColouringBitArray::Index(2,23));
    assert(ar[ColouringBitArray::Index(2,21)]);
    assert(!ar[ColouringBitArray::Index(2,22)]);

    ar=ColouringBitArray(ColouringBitArray::Index(2,12), true);
    ar2=ColouringBitArray(ColouringBitArray::Index(0,3), false);
    ar2.set(ColouringBitArray::Index(0,1), true);
    ar.concatenate_to_special(ar2);
    assert(ar.size()==ColouringBitArray::Index(2,15));
    assert(ar[ColouringBitArray::Index(2,11)]);
    assert(!ar[ColouringBitArray::Index(2,12)]);
    assert(ar[ColouringBitArray::Index(2,13)]);
    assert(!ar[ColouringBitArray::Index(2,14)]);

    ar=ColouringBitArray(ColouringBitArray::Index(2,18), true);
    ar2=ColouringBitArray(ColouringBitArray::Index(0,9), false);
    ar2.set(ColouringBitArray::Index(0,1), true);
    ar2.set(ColouringBitArray::Index(0,6), true);
    ar.concatenate_to_special(ar2);
    assert(ar.size()==ColouringBitArray::Index(3,0));
    assert(ar[ColouringBitArray::Index(2,17)]);
    assert(!ar[ColouringBitArray::Index(2,18)]);
    assert(ar[ColouringBitArray::Index(2,19)]);
    assert(!ar[ColouringBitArray::Index(2,20)]);
    assert(!ar[ColouringBitArray::Index(2,23)]);
    assert(ar[ColouringBitArray::Index(2,24)]);
    assert(!ar[ColouringBitArray::Index(2,25)]);



//split3 powers 3+
    ar=ColouringBitArray(ColouringBitArray::Index(27,0), true);
    ar.set(ColouringBitArray::Index(5,13), false);
    ar.set(ColouringBitArray::Index(7,12), false);
    auto [r0, r1, r2]=ar.split3(4);
    assert(r0.size()==ColouringBitArray::Index(9,0));
    assert(r1.size()==ColouringBitArray::Index(9,0));
    assert(r2.size()==ColouringBitArray::Index(9,0));

    assert(r1[ColouringBitArray::Index(2,12)]);
    assert(!r1[ColouringBitArray::Index(2,13)]);
    assert(r1[ColouringBitArray::Index(2,14)]);
    assert(r2[ColouringBitArray::Index(1,11)]);
    assert(!r2[ColouringBitArray::Index(1,12)]);
    assert(r2[ColouringBitArray::Index(1,13)]);

//split3 powers 2
    std::tie(r0, r1, r2)=ar.split3(2);
    assert(r1[ColouringBitArray::Index(1,21)]);
    assert(!r1[ColouringBitArray::Index(1,22)]);
    assert(r1[ColouringBitArray::Index(1,23)]);
    assert(r1[ColouringBitArray::Index(2,11)]);
    assert(!r1[ColouringBitArray::Index(2,12)]);
    assert(r1[ColouringBitArray::Index(2,13)]);

//split3 powers 1
    std::tie(r0, r1, r2)=ar.split3(1);
    assert(r1[ColouringBitArray::Index(1,21)]);
    assert(!r1[ColouringBitArray::Index(1,22)]);
    assert(r1[ColouringBitArray::Index(1,23)]);
    assert(r1[ColouringBitArray::Index(2,11)]);
    assert(!r1[ColouringBitArray::Index(2,12)]);
    assert(r1[ColouringBitArray::Index(2,13)]);

//split3 powers 0
    std::tie(r0, r1, r2)=ar.split3(0);
    assert(r1[ColouringBitArray::Index(1,21)]);
    assert(!r1[ColouringBitArray::Index(1,22)]);
    assert(r1[ColouringBitArray::Index(1,23)]);
    assert(r0[ColouringBitArray::Index(2,12)]);
    assert(!r0[ColouringBitArray::Index(2,13)]);
    assert(r0[ColouringBitArray::Index(2,14)]);

    r1&=r0;
    assert(r1[ColouringBitArray::Index(2,12)]);
    assert(!r1[ColouringBitArray::Index(2,13)]);
    assert(r1[ColouringBitArray::Index(2,14)]);

    r1|=r0;
    assert(r1[ColouringBitArray::Index(1,21)]);
    assert(r1[ColouringBitArray::Index(1,22)]);
    assert(r1[ColouringBitArray::Index(1,23)]);

    
}
