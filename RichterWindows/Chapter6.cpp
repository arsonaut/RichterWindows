#include "stdafx.h"
#include "Chapter6.h"

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

    bool NextPrime(uint64_t& number)
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

	using NextPrimeThreadData = std::shared_ptr<uint64_t>;

    unsigned __stdcall GetNextPrimeThread(void* arg)
    {
        NextPrimeThreadData threadData{*static_cast<NextPrimeThreadData*>(arg)};
        return NextPrime(*threadData) ? ERROR_SUCCESS : ERROR_ARITHMETIC_OVERFLOW;
    }

    unsigned __stdcall PrintNextPrimeThread(void* arg)
    {
        NextPrimeThreadData threadData{*static_cast<NextPrimeThreadData*>(arg)};
        const auto startTime = ::GetTickCount64();
        const bool result = NextPrime(*threadData);
        const auto elapsedTime = ::GetTickCount64() - startTime;
        if (result)
        {
            std::cout << "Next prime = " << *threadData << " (elapsed time = " << elapsedTime << " msec)" << std::endl;
        }
        return result ? ERROR_SUCCESS : ERROR_ARITHMETIC_OVERFLOW;
    }

    bool ThreadLauncher(uint64_t& number, _beginthreadex_proc_type threadFunc)
    {
        NextPrimeThreadData&& threadData = std::make_shared<uint64_t>(number);
        const auto threadHandle = reinterpret_cast<HANDLE>(::_beginthreadex(nullptr, 0, threadFunc, &threadData, 0, nullptr));
        if (threadHandle == 0)
        {
            throw std::system_error{errno, std::generic_category()};
        }
        unsigned long threadExitCode{};
        if (::WaitForSingleObject(threadHandle, INFINITE) != WAIT_OBJECT_0 ||
            ::GetExitCodeThread(threadHandle, &threadExitCode) != TRUE ||
            ::CloseHandle(threadHandle) != TRUE)
        {
            throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
        }
        number = *threadData;
        return threadExitCode == ERROR_SUCCESS;
    }
}

bool chapter6::GetNextPrime(uint64_t& number)
{
    return ThreadLauncher(number, GetNextPrimeThread);
}

bool chapter7::PrintNextPrime(uint64_t& number)
{
    return ThreadLauncher(number, PrintNextPrimeThread);
}
