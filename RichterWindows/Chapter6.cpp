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

    using PrintSyncedThreadData = std::shared_ptr<std::pair<uint64_t, ::CRITICAL_SECTION&>>;

    class CriticalSectionGuard
    {
    public:
        CriticalSectionGuard(::CRITICAL_SECTION& cs)
            : m_cs{cs}
        {
            ::EnterCriticalSection(&m_cs);
        }

        ~CriticalSectionGuard()
        {
            ::LeaveCriticalSection(&m_cs);
        }

    private:
        ::CRITICAL_SECTION& m_cs;
    };

    unsigned __stdcall PrintSyncedNextPrimeThread(void* arg)
    {
        PrintSyncedThreadData threadData{*static_cast<PrintSyncedThreadData*>(arg)};
        const auto startTime = ::GetTickCount64();
        const bool result = NextPrime(threadData->first);
        const auto elapsedTime = ::GetTickCount64() - startTime;
        if (result)
        {
            CriticalSectionGuard csGuard{threadData->second};
            std::cout << "Next prime = " << threadData->first << " (elapsed time = " << elapsedTime << " msec)" << std::endl;
        }
        return result ? ERROR_SUCCESS : ERROR_ARITHMETIC_OVERFLOW;
    }

    unsigned ThreadLauncher(_beginthreadex_proc_type threadFunc, void* threadArg)
    {
        const auto threadHandle = reinterpret_cast<HANDLE>(::_beginthreadex(nullptr, 0, threadFunc, threadArg, 0, nullptr));
        if (threadHandle == 0)
        {
            throw std::system_error{errno, std::generic_category()};
        }
        unsigned long threadExitCode{0};
        if (::WaitForSingleObject(threadHandle, INFINITE) != WAIT_OBJECT_0 ||
            ::GetExitCodeThread(threadHandle, &threadExitCode) != TRUE ||
            ::CloseHandle(threadHandle) != TRUE)
        {
            throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
        }
        return threadExitCode;
    }
}

bool chapter6::GetNextPrime(uint64_t& number)
{
    NextPrimeThreadData&& threadData = std::make_shared<uint64_t>(number);
    const auto result = ThreadLauncher(GetNextPrimeThread, &threadData);
    number = *threadData;
    return result == ERROR_SUCCESS;
}

bool chapter7::PrintNextPrime(uint64_t& number)
{
    NextPrimeThreadData&& threadData = std::make_shared<uint64_t>(number);
    const auto result = ThreadLauncher(PrintNextPrimeThread, &threadData);
    number = *threadData;
    return result == ERROR_SUCCESS;
}

chapter8::CriticalSectionWrapper::CriticalSectionWrapper()
{
    ::InitializeCriticalSection(&m_cs);
}

chapter8::CriticalSectionWrapper::~CriticalSectionWrapper()
{
    ::DeleteCriticalSection(&m_cs);
}

::CRITICAL_SECTION& chapter8::CriticalSectionWrapper::get()
{
    return m_cs;
}

bool chapter8::PrintSyncedNextPrime(uint64_t& number, ::CRITICAL_SECTION& cs)
{
    PrintSyncedThreadData&& threadData = std::make_shared<std::pair<uint64_t, ::CRITICAL_SECTION&>>(number, cs);
    const auto result = ThreadLauncher(PrintSyncedNextPrimeThread, &threadData);
    number = threadData->first;
    return result == ERROR_SUCCESS;
}

chapter9::EventWrapper::EventWrapper(bool initialState)
    : m_handle{::CreateEvent(nullptr, TRUE, initialState ? TRUE : FALSE, nullptr)}
{
    if (m_handle == NULL)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
}

chapter9::EventWrapper::~EventWrapper()
{
    ::CloseHandle(m_handle);
}

::HANDLE chapter9::EventWrapper::get()
{
    return m_handle;
}

void chapter9::EventWrapper::setState(bool state)
{
    const auto result = state ? ::SetEvent(m_handle) : ::ResetEvent(m_handle);
    if (result == FALSE)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
}

bool chapter9::PrintConditionallyNextPrime(uint64_t & number, ::HANDLE & mutex)
{
    return false;
}
