#ifndef BA_GRAPH_SNARKS_COLOURING_BIT_ARRAY_H
#define BA_GRAPH_SNARKS_COLOURING_BIT_ARRAY_H

#include<vector>
#include<tuple>

namespace ba_graph {



class ColouringBitArray {
public:
    //index for our bitarray
    class Index {        
        uint_fast64_t first_;
        uint_fast8_t second_;

    public:
        static const uint_fast8_t SMAX = 27;
        static const uint_fast8_t SMAX_COMPLEMENT = 5; //13=5-27

        inline Index();
        inline Index(uint_fast64_t f, uint_fast8_t s);
        inline Index(std::vector<uint_fast8_t> v);
        Index(const Index&)=default;
        Index(Index&&)=default;
        Index& operator=(const Index&)=default;
        Index& operator=(Index&&)=default;

        inline static Index to_index(uint_fast64_t f);
        inline uint_fast64_t to_int64() const;

        inline uint_fast64_t first() const;
        inline uint_fast8_t second() const;

        inline Index& operator++();
        inline Index operator++(int);
        inline Index& operator--();
        inline Index operator--(int);
        inline Index& add_to_special(const Index &other);
        inline Index& manipulate_special(uint_fast8_t s, int i);
        inline Index& operator+= (const Index &other);
        inline Index& operator-= (const Index &other);
        inline bool operator== (const Index &other) const;
        inline bool operator!= (const Index &other) const {return !(*this==other);}
        inline bool operator< (const Index &other) const;
        inline bool operator> (const Index &other) const {return (other<*this);}
        inline bool operator<= (const Index &other) const;
        inline bool operator>= (const Index &other) const {return (other<=*this);}

        inline bool is_power() const;
        inline uint_fast8_t get_power() const;
        inline bool is_power_multiple(uint_fast8_t s) const;
    };


    //precalculate indices that are power of 3
    static const uint_fast8_t MAX_POWER=40;
    class Power3 {
    private:
        Index pow_[MAX_POWER];
    public:
        Power3();
        inline const Index& operator[] (uint_fast8_t s) const;
    };
    inline static Power3 pow3 = ColouringBitArray::Power3();


private:
    struct Bits27 {
        uint32_t bits;
        inline Bits27(uint32_t value);
        inline bool get(uint_fast8_t s) const;
        inline void set(uint_fast8_t s, bool value);
    private:
        inline Bits27 f0() const;
        inline Bits27 f1() const;
    public:
        static std::tuple<Bits27, Bits27, Bits27> split3(const Bits27 &s1, const Bits27 &s2, const Bits27 &s3, uint_fast8_t power);
    };   


//private members of ColouringBitArray
    Index size_;    
    std::vector<Bits27> array_;

public:    
    //true, false
    static const uint32_t T=0xFFFFFFFF;
    static const uint32_t F=0x00000000;

    //ColouringBitArray constructors
    inline ColouringBitArray(Index sz, uint32_t value);
    ColouringBitArray(): ColouringBitArray::ColouringBitArray(Index(0,0), F) {}
    ColouringBitArray(uint_fast64_t sz, uint32_t value): ColouringBitArray::ColouringBitArray(Index::to_index(sz), value) {}
    ColouringBitArray(Index sz, bool value): ColouringBitArray::ColouringBitArray(sz, value?T:F) {}
    ColouringBitArray(uint_fast64_t sz, bool value): ColouringBitArray::ColouringBitArray(sz, value?T:F) {}

    //copy&move constructor&assignment
    ColouringBitArray(const ColouringBitArray &other)=default;
    ColouringBitArray(ColouringBitArray &&other)=default;
    ColouringBitArray & operator=(const ColouringBitArray &other)=default;
    ColouringBitArray & operator=(ColouringBitArray &&other)=default;

    //simple methods
    inline void reserve(uint_fast64_t sz);    
    inline void reserve(const Index &sz);    
    inline Index size() const;    
    inline bool operator[](const Index &i) const;
    inline void set(const Index &i, bool value);
    inline bool get(const Index &i) const {return (*this)[i];}
    inline bool get(uint_fast64_t a, uint_fast8_t b) const {return get(Index(a,b));}
    inline void set(uint_fast64_t a, uint_fast8_t b, bool value) {set(Index(a,b), value);}

    //==, !=, all_true, all_false
    inline bool operator==(const ColouringBitArray &other) const;
    inline bool operator!=(const ColouringBitArray &other) const {return !((*this)==other);}
    inline bool operator<(const ColouringBitArray &other) const;
    inline bool all_true() const;
    inline bool all_false() const;

    //we require that the length of other is power of 3 and that the length of this is a multiple of other
    inline void concatenate_to_special(const ColouringBitArray &other);

    inline std::tuple<ColouringBitArray, ColouringBitArray, ColouringBitArray> split3(uint_fast8_t power) const;
    inline std::tuple<ColouringBitArray, ColouringBitArray, ColouringBitArray, 
                      ColouringBitArray, ColouringBitArray, ColouringBitArray, 
                      ColouringBitArray, ColouringBitArray, ColouringBitArray> split9(uint_fast8_t power1, uint_fast8_t power2) const;
     
    //&=, |=
    inline ColouringBitArray& operator&=(const ColouringBitArray &other);
    inline ColouringBitArray& operator|=(const ColouringBitArray &other);
    


    //concatenate constructor

};


} //end namespace ba_graph

#endif
