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
    class EventWrapper
    {
    public:
        EventWrapper(bool initialState);
        ~EventWrapper();
        ::HANDLE get();
        void setState(bool state);

    private:
        ::HANDLE m_handle;
    };

    bool PrintConditionallyNextPrime(uint64_t& number, ::HANDLE event);
}

namespace chapter11
{
    bool PrintNextPrimeTreadPool(uint64_t& number);
}