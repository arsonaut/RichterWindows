#pragma once

namespace chapter6
{
    bool GetNextPrime(uint64_t& number);
}

namespace chapter7
{
    bool PrintNextPrime(uint64_t& number);
}

namespace chapter8
{
    class CriticalSectionWrapper
    {
    public:
        CriticalSectionWrapper();
        ~CriticalSectionWrapper();
        ::CRITICAL_SECTION& get();

    private:
        ::CRITICAL_SECTION m_cs;
    };

    bool PrintSyncedNextPrime(uint64_t& number, ::CRITICAL_SECTION& cs);
}

namespace chapter9
{
    class MutexWrapper
    {
    public:
        MutexWrapper();
        ~MutexWrapper();
        ::HANDLE get();

    private:
        ::HANDLE m_handle;
    };
}
