#ifndef BA_GRAPH_STRUCTURES_PERMUTATION_HPP
#define BA_GRAPH_STRUCTURES_PERMUTATION_HPP

#include <array>
#include <initializer_list>
#include <ostream>

#ifdef BA_GRAPH_DEBUG
#include <numeric>
#endif

namespace ba_graph {
    template<unsigned dg>
    class permutation {
    public:
        enum {
            degree = dg
        };

        permutation() {
            for (unsigned i = 0; i < dg; ++i)
                m_data[i] = i;
        }

        permutation(std::initializer_list<unsigned> lst) {
#ifdef BA_GRAPH_DEBUG // pre-condition tests
            assert(dg == lst.size());
            assert(std::accumulate(lst.begin(), lst.end(), 0) == dg * (dg - 1) / 2);
#endif
            // std::copy(lst.begin(), lst.begin() + this->degree, this->m_data);
            unsigned i = 0;
            for (auto item : lst) {
                m_data[i++] = item;
            }
        }

        permutation(permutation const &other) = default;

        permutation<dg> &operator=(permutation<dg> const &rhs) {
            //        static_assert(d1 == rhs.degree, "Non-equal degrees");
            // std::copy(rhs.m_data, rhs.m_data + d1, m_data);
            size_t i = 0;
            for (auto item : rhs.m_data) {
                m_data[i++] = item;
            }
            return *this;
        }

        template<unsigned d>
        friend std::ostream &operator<<(std::ostream &, permutation<d> const &);

        template<unsigned d1>
        friend permutation<d1> operator*(permutation<d1> const &,
                                         permutation<d1> const &);


        unsigned operator()(unsigned i) const {

#ifdef BA_GRAPH_DEBUG // pre-condition test
            assert(i < dg);
#endif
            return m_data[i];
        }

        permutation<dg> inverse() const {
            //        static_assert(this->degree == d, "Degrees does not match");
            permutation<dg> inv;
            for (size_t i = 0; i != dg; ++i)
                inv.m_data[m_data[i]] = static_cast<unsigned>(i);
            return inv;
        }

        permutation<dg> conjugate(permutation<dg> const &other) {
            return other * (*this) * other.inverse();
        }

    private:
        std::array<unsigned, dg> m_data;
    };

    template<unsigned d>
    std::ostream &operator<<(std::ostream &strm, permutation<d> const &perm) {
        strm << "\n[ ";
        for (size_t i = 0; i != d; ++i)
            strm << i << " ";
        strm << "]\n";
        strm << "[ ";
        for (size_t i = 0; i != d; ++i)
            strm << perm.m_data[i] << " ";
        strm << "]\n";

        return strm;
    }

    template<unsigned d1>
    permutation<d1> operator*(permutation<d1> const &lhs,
                              permutation<d1> const &rhs) {
        permutation<d1> result;
        for (size_t i = 0; i != result.degree; ++i) {
            result.m_data[i] = rhs.m_data[lhs.m_data[i]];
        }
        return result;
    }

    /**
     * Compute the power of a the permutation p.
     *
     * @tparam dg the degree of the argument (a permutation)
     * @param p a permutatuion of degree `dg`, acting on the set `{0,1,...,dg-1}`
     * @param n the power (could be negative)
     * @return the permutation `p^n`
     */
    template<unsigned dg>
    permutation<dg> pow(permutation<dg> const &p, int n) {
        permutation<dg> ret;
        permutation<dg> base = n < 0 ? p.inverse() : p;
        int power = n < 0 ? -n : n;
        while (power > 0) {
            if (power & 1) {
                ret = ret * base;
                --power;
            } else {
                ret = ret * (base * base);
                power /= 2;
            }
        }
        return ret;
    }
} // namespace ba_graph

#endif // !BA_GRAPH_STRUCTURES_PERMUTATION_HPP