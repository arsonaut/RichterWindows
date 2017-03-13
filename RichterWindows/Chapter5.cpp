#include "stdafx.h"
#include "Chapter5.h"

namespace
{
    bool IsPrime(const uint64_t number)
    {
        if (number <= 1)
        {
            return false;
        }
        for (const auto smallPrime : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29})
        {
            if (number % smallPrime == 0)
            {
                return false;
            }
        }
        for (uint64_t divisorBase{30}; ; divisorBase += 30)
        {
            for (const auto divisorOffset : {1, 7, 11, 13, 17, 19, 23, 29})
            {
                const auto divisor = divisorBase + divisorOffset;
                if (number / divisor < divisor) // divisor > sqrt(number)
                {
                    return true;
                }
                if (number % divisor == 0)
                {
                    return false;
                }
            }
        }
    }
}

bool chapter5::GetNextPrime(uint64_t& number)
{
    while (number < std::numeric_limits<unsigned long long>::max())
    {
        ++number;
        if (IsPrime(number))
        {
            return true;
        }
    }
    return false;
}
