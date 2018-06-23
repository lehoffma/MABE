//
// Created by Le on 21.02.2018.
//

#ifndef MABE_COMPAREUTILS_H
#define MABE_COMPAREUTILS_H

#include <cassert>
#include <limits>
#include <type_traits>

namespace CompareUtils {
    //no idea what is happening here honestly, i just copied it from here
    //source: http://www.cygnus-software.com/papers/comparingfloats/Comparing%20floating%20point%20numbers.htm#_Toc135149453
    inline bool AlmostEqual2sComplement(double A, double B, int maxUlps) {
        // Make sure maxUlps is non-negative and small enough that the
        // default NAN won't compare as equal to anything.
        assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);

        int aInt = *(int *) &A;
        // Make aInt lexicographically ordered as a twos-complement int
        if (aInt < 0) {
            aInt = 0x80000000 - aInt;
        }

        // Make bInt lexicographically ordered as a twos-complement int
        int bInt = *(int *) &B;

        if (bInt < 0) {
            bInt = 0x80000000 - bInt;
        }

        int intDiff = abs(aInt - bInt);

        return intDiff <= maxUlps;
    }

    //source: http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
    template<class T>
    typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    inline almostEqual(T x, T y, int ulp) {
        // the machine epsilon has to be scaled to the magnitude of the values used
        // and multiplied by the desired precision in ULPs (units in the last place)
        return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
               // unless the result is subnormal
               || std::abs(x - y) < std::numeric_limits<T>::min();
    }

    struct greater {
        template<class T>
        bool operator()(T const &a, T const &b) const { return a > b; }
    };

    struct lesser {
        template<class T>
        bool operator()(T const &a, T const &b) const { return a < b; }
    };

}

#endif //MABE_COMPAREUTILS_H
