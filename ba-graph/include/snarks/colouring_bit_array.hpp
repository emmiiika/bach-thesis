#ifndef BA_GRAPH_SNARKS_COLOURING_BIT_ARRAY_HPP
#define BA_GRAPH_SNARKS_COLOURING_BIT_ARRAY_HPP

#ifdef BA_GRAPH_DEBUG
#include <iostream>
#endif

#include "colouring_bit_array.h"
#include <vector>

namespace ba_graph {

/****************
ColouringBitArray::Index
*****************/


//constructors
inline ColouringBitArray::Index::Index(): first_(0), second_(0) {}
inline ColouringBitArray::Index::Index(uint_fast64_t f, uint_fast8_t s): first_(f), second_(s) {}
inline ColouringBitArray::Index::Index(std::vector<uint_fast8_t> v): first_(0), second_(0) {
    for(unsigned int i=0;i<v.size() && i<3;i++) {
        if (v[i]>0) second_+=pow3[i].second_;
        if (v[i]>1) second_+=pow3[i].second_;
    }
    for(unsigned int i=3;i<v.size();i++) {
        if (v[i]>0) first_+=pow3[i].first_;
        if (v[i]>1) first_+=pow3[i].first_;
    }
}

//conversions
inline ColouringBitArray::Index ColouringBitArray::Index::to_index(uint_fast64_t f) {return Index(f/SMAX, f%SMAX);}
inline uint_fast64_t ColouringBitArray::Index::to_int64() const {return first_*SMAX+second_;}

//geters
inline uint_fast64_t ColouringBitArray::Index::first() const {return first_;}
inline uint_fast8_t ColouringBitArray::Index::second() const {return second_;}

//operators
inline ColouringBitArray::Index& ColouringBitArray::Index::operator++() {
    second_++; 
    if (second_==SMAX) {second_=0; first_++;};
    return *this;  
}
inline ColouringBitArray::Index ColouringBitArray::Index::operator++(int) {Index a=*this; ++(*this); return a;}
inline ColouringBitArray::Index& ColouringBitArray::Index::operator--() {
    if (second_==0) {second_=SMAX; first_--;}
    second_--; 
    return *this;  
}
inline ColouringBitArray::Index ColouringBitArray::Index::operator--(int) {Index a=*this; --(*this); return a;}

inline ColouringBitArray::Index& ColouringBitArray::Index::add_to_special(const Index &other) {
#ifdef BA_GRAPH_DEBUG
    assert(second_+other.second_<=SMAX);
#endif    
    second_+=other.second_; first_+=other.first_; 
    if (second_==SMAX) {second_=0; first_++;}
    return *this;
}

inline ColouringBitArray::Index& ColouringBitArray::Index::manipulate_special(uint_fast8_t s, int i) {
    switch (s) {
        case 0: second_+=i; return *this;
        case 1: second_+=3*i; return *this;
        case 2: second_+=9*i; return *this;
    }
    uint_fast64_t j=pow3[s].first_; 
    switch (i) {
        case -2: first_-=j; 
            // fall through
        case -1: first_-=j; break;
        case 2: first_+=j;
            // fall through
        case 1: first_+=j; break;
    }    
    return *this;
}

inline ColouringBitArray::Index& ColouringBitArray::Index::operator+= (const Index &other) {
    first_+=other.first_; 
    second_+=other.second_;     
    //note that this works even if second turns to have more than 8 bits
    if (second_<other.second_) {second_+=SMAX_COMPLEMENT; first_++;} 
    if (second_>=SMAX) {second_-=SMAX; first_++;} 
    return *this;
}

inline ColouringBitArray::Index& ColouringBitArray::Index::operator-= (const Index &other) {
    first_-=other.first_; 
    if (second_<other.second_)
        {second_=SMAX-(other.second_-second_);first_--;}
    else
        second_-=other.second_;
    return *this;
}


inline bool ColouringBitArray::Index::operator< (const Index &other) const {
    if (first_<other.first_) return true;
    return (first_==other.first_ && second_<other.second_);       
}
inline bool ColouringBitArray::Index::operator== (const Index &other) const {
    return (first_==other.first_ && second_==other.second_);       
}
inline bool ColouringBitArray::Index::operator<= (const Index &other) const {
    if (first_<other.first_) return true;
    return (first_==other.first_ && second_<=other.second_);       
}

//is power of 3
inline bool ColouringBitArray::Index::is_power() const {
    if (first_==0) return second_==1 || second_==3 || second_==9;
    if (second_!=0) return false;
    for(uint_fast8_t i=3;i<MAX_POWER;i++) 
        if (pow3[i].first_ >= first_)
            return (pow3[i].first_ == first_);
    return false;
}

inline uint_fast8_t ColouringBitArray::Index::get_power() const {
    if (first_==0) {
        if (second_<=1) return 0;
        if (second_<=3) return 1;
        if (second_<=9) return 2;
    }
    for(uint_fast8_t i=3;i<MAX_POWER;i++) 
        if (pow3[i].first_ >= first_)
            return i;
    return MAX_POWER;
}


inline bool ColouringBitArray::Index::is_power_multiple(uint_fast8_t s) const {
    if (s>=3) return (second_==0 && first_%pow3[s].first_==0);
    return second_ % pow3[s].second_==0;
}


/****************
ColouringBitArray::Power3
ColouringBitArray::pow3
*****************/
inline ColouringBitArray::Power3::Power3() {
     pow_[0]=Index(0, 1); pow_[1]=Index(0, 3); pow_[2]=Index(0, 9);
     uint_fast64_t p=1;
     for(uint_fast8_t i=3;i<MAX_POWER;i++, p*=3) pow_[i]=Index(p,0);
}

inline const ColouringBitArray::Index& ColouringBitArray::Power3::operator[] (uint_fast8_t s) const {return pow_[s];}


/****************
ColouringBitArray::Bits27
*****************/

inline ColouringBitArray::Bits27::Bits27(uint32_t value): bits(value) {}
inline bool ColouringBitArray::Bits27::get(uint_fast8_t s) const {return bits & (1ul<<s);}
inline void ColouringBitArray::Bits27::set(uint_fast8_t s, bool value) {
    if (value) bits|=(1ul<<s);
    else bits&=~(1ul<<s);
}


//helper functions
inline ColouringBitArray::Bits27 ColouringBitArray::Bits27::f0() const {
    return Bits27(
                     ((bits & 0421421421)<<0) | ((bits & 0042042042)<<2) | ((bits & 0210210210)>>2) |
                     ((bits & 0004004004)<<4) | ((bits & 0100100100)>>4)
                 ).f1();
}
inline ColouringBitArray::Bits27 ColouringBitArray::Bits27::f1() const {
    return Bits27(
                     ((bits & 0700070007)<<0) | ((bits & 0000700070)<<6) | ((bits & 0070007000)>>6) |
                     ((bits & 0000000700)<<12) | ((bits & 0007000000)>>12)
                 );
}

inline std::tuple<ColouringBitArray::Bits27, ColouringBitArray::Bits27, ColouringBitArray::Bits27> 
        ColouringBitArray::Bits27::split3(const Bits27 &s1, const Bits27 &s2, const Bits27 &s3, uint_fast8_t power) {
#ifdef BA_GRAPH_DEBUG
    assert(power<3);
#endif      
    if (power==0)
        return split3(s1.f0(),s2.f0(),s3.f0(),2);
    if (power==1)
        return split3(s1.f1(),s2.f1(),s3.f1(),2);
    std::tuple<Bits27, Bits27, Bits27> newbits(
        ((s1.bits & 0x000001FFu)<< 0) | ((s2.bits & 0x000001FFu)<<9) | ((s3.bits & 0x000001FFu)<<18),
        ((s1.bits & 0x0003FE00u)>> 9) | ((s2.bits & 0x0003FE00u)<<0) | ((s3.bits & 0x0003FE00u)<< 9),
        ((s1.bits & 0x07FC0000u)>>18) | ((s2.bits & 0x07FC0000u)>>9) | ((s3.bits & 0x07FC0000u)<< 0) );
    return newbits;
}


/****************
ColouringBitArray
*****************/

//ColouringBitArray constructors
inline ColouringBitArray::ColouringBitArray(Index sz, uint32_t value) {
    size_=sz;         
    int vlen=size_.first();
    if (size_.second()!=0) vlen++;
    array_=std::vector<Bits27>(vlen, Bits27(value));
}

//reserve, size, [], get, set
inline void ColouringBitArray::reserve(uint_fast64_t sz) {reserve(Index::to_index(sz));}
inline void ColouringBitArray::reserve(const Index &i) {array_.reserve(i.first());}
inline ColouringBitArray::Index ColouringBitArray::size() const {return size_;}  
inline bool ColouringBitArray::operator[](const Index &i) const {
#ifdef BA_GRAPH_DEBUG
    assert(i<size_);
#endif          
    return array_[i.first()].get(i.second());
}
inline void ColouringBitArray::set(const Index &i, bool value) {
#ifdef BA_GRAPH_DEBUG
    assert(i<size_);
#endif          
    array_[i.first()].set(i.second(), value);
}

//==, !=, all_true, all_false
inline bool ColouringBitArray::operator==(const ColouringBitArray &other) const {
    if (size_!=other.size_) return false;
    uint_fast64_t i=0;
    while (i!=size_.first()) {
        if ( ( array_[i].bits & ((1ul<<28)-1) ) != ( other.array_[i].bits & ((1ul<<28)-1) ) ) return false;
        i++;
    }
    if (size_.second()==0)  return true;
    return ((array_[i].bits & ((1ul<<size_.second())-1)) == (other.array_[i].bits & ((1ul<<size_.second())-1)));
}

inline bool ColouringBitArray::operator<(const ColouringBitArray &other) const {
    if (size_!=other.size_) return size_<other.size_;
    uint_fast64_t i=0;
    while (i!=size_.first()) {
        if ( ( array_[i].bits & ((1ul<<28)-1) ) < ( other.array_[i].bits & ((1ul<<28)-1) ) ) return true;
        if ( ( array_[i].bits & ((1ul<<28)-1) ) > ( other.array_[i].bits & ((1ul<<28)-1) ) ) return false;
        i++;
    }
    if (size_.second()==0)  return false;
    return ((array_[i].bits & ((1ul<<size_.second())-1)) < (other.array_[i].bits & ((1ul<<size_.second())-1)));
}

inline bool ColouringBitArray::all_true() const {
    uint_fast64_t i=0;
    while (i!=size_.first()) {
        if ( (~array_[i].bits) & ((1ul<<28)-1) ) return false;
        i++;
    }
    if (size_.second()==0)  return true;
    return !( (~array_[i].bits) & ((1ul<<size_.second())-1) );                                  
}

inline bool ColouringBitArray::all_false() const {
    uint_fast64_t i=0;
    while (i!=size_.first()) {
        if ( array_[i].bits & ((1ul<<28)-1) ) return false;
        i++;
    }
    if (size_.second()==0)  return true;
    return !( array_[i].bits & ((1ul<<size_.second())-1) );                                  
}




inline void ColouringBitArray::concatenate_to_special(const ColouringBitArray &other) {
#ifdef BA_GRAPH_DEBUG
    if (other.size_.first()==0) {
        assert(other.size_.second()==1 || other.size_.second()==3 || other.size_.second()==9);
        assert(size_.second()%other.size_.second() == 0);
    }
    else assert(size_.second()==0 && other.size_.second()==0);
#endif   
    uint_fast8_t s2=other.size_.second();
    uint_fast64_t last=size_.first();  //this is incorect if size_.second()==0, but it does not matter then
    if (s2==0) array_.insert(array_.end(), other.array_.begin(), other.array_.end());
    else {
        uint_fast8_t s1=size_.second();
        if (s1==0) array_.push_back(other.array_[0]);
        else ( array_[last].bits &= ((1ul<<s1)-1) ) |= (other.array_[0].bits<<s1);
    };

    //we fix the size
    size_.add_to_special(other.size());
    return;
}
     
    

//splits
inline std::tuple<ColouringBitArray, ColouringBitArray, ColouringBitArray> ColouringBitArray::split3(uint_fast8_t power) const {
#ifdef BA_GRAPH_DEBUG
    assert(size_.is_power_multiple(power+1));
#endif  
    std::tuple<ColouringBitArray, ColouringBitArray, ColouringBitArray> res;
    Index new_size=Index(size_.first()/3, size_.second()/3);
    if (size_.first()==1) new_size=Index(0, 9);

    std::get<0>(res).reserve(new_size);
    std::get<1>(res).reserve(new_size);
    std::get<2>(res).reserve(new_size);
    std::get<0>(res).size_=new_size;
    std::get<1>(res).size_=new_size;
    std::get<2>(res).size_=new_size;
    
    if (power>=3) {
        uint_fast64_t i=0;
        for(;i<size_.first();) {
            for(uint_fast64_t j=0;j<pow3[power].first();j++,i++) 
                std::get<0>(res).array_.push_back(array_[i]);
            for(uint_fast64_t j=0;j<pow3[power].first();j++,i++) 
                std::get<1>(res).array_.push_back(array_[i]);
            for(uint_fast64_t j=0;j<pow3[power].first();j++,i++) 
                std::get<2>(res).array_.push_back(array_[i]);
        }
        return res;
    }    
    else {
        uint_fast64_t i=0;
        uint_fast64_t last=size_.first();
        if (size_.second()==0) last--;

        for(;i+2<=last;i+=3) {
            auto [a0, a1, a2]=Bits27::split3(array_[i], array_[i+1], array_[i+2], power);
            std::get<0>(res).array_.push_back(a0);
            std::get<1>(res).array_.push_back(a1);
            std::get<2>(res).array_.push_back(a2);
        }
        //euther i==last-1 or i==last or i+2==last+1 (in the last case we are done)
        if (i<=last) {  
            Bits27 b1(0);
            if (i+1==last) b1=array_[i];            
            auto [a0, a1, a2]=Bits27::split3(array_[i], b1, Bits27(0), power);
            std::get<0>(res).array_.push_back(a0);
            std::get<1>(res).array_.push_back(a1);
            std::get<2>(res).array_.push_back(a2);
        }
    }

    return res;
}



inline std::tuple<ColouringBitArray, ColouringBitArray, ColouringBitArray, 
                  ColouringBitArray, ColouringBitArray, ColouringBitArray, 
                  ColouringBitArray, ColouringBitArray, ColouringBitArray> ColouringBitArray::split9(uint_fast8_t power1, uint_fast8_t power2) const {
    auto [part0, part1, part2] = split3(power1);
    if (power2>power1) power2--;
    auto [part00, part01, part02] = part0.split3(power2);
    auto [part10, part11, part12] = part1.split3(power2);
    auto [part20, part21, part22] = part2.split3(power2);
    return std::tuple(std::move(part00), std::move(part01), std::move(part02), 
                      std::move(part10), std::move(part11), std::move(part12),
                      std::move(part20), std::move(part21), std::move(part22));
}


//|= &=
inline ColouringBitArray& ColouringBitArray::operator&=(const ColouringBitArray &other) {
#ifdef BA_GRAPH_DEBUG
    assert(size_==other.size_);
#endif   
    auto it2=std::cbegin(other.array_);
    for(auto it1=std::begin(array_); it1!=std::end(array_); it1++, it2++)
        (*it1).bits&=(*it2).bits;
    return (*this);
}
inline ColouringBitArray& ColouringBitArray::operator|=(const ColouringBitArray &other) {
#ifdef BA_GRAPH_DEBUG
    assert(size_==other.size_);
#endif   
    auto it2=std::cbegin(other.array_);
    for(auto it1=std::begin(array_); it1!=std::end(array_); it1++, it2++)
        (*it1).bits|=(*it2).bits;
    return (*this);
}


} //end namespace ba_graph

#endif
