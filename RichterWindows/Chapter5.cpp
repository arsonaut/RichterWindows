#include "stdafx.h"
#include "Chapter5.h"

namespace
{
    struct HandleCloser
    {
        void operator()(::HANDLE handle)
        {
            ::CloseHandle(handle);
        }
    };
}

void chapter5::CreateTwoProcessJob()
{
    using HandleGuard = std::unique_ptr<void, HandleCloser>;
    std::vector<HandleGuard> handleGuards;

    const auto job = ::CreateJobObjectW(nullptr, nullptr);
    if (job == NULL)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    handleGuards.emplace_back(job);

    for (size_t n = 0; n < 2; ++n)
    {
        ::STARTUPINFOW childrenSI{};
        childrenSI.cb = sizeof(childrenSI);
        ::PROCESS_INFORMATION childrenPI{};
        if (::CreateProcessW(L"ChildProcess.exe", nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &childrenSI, &childrenPI) == 0)
        {
            throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
        }
        handleGuards.emplace_back(childrenPI.hThread);
        handleGuards.emplace_back(childrenPI.hProcess);

        if (::AssignProcessToJobObject(job, childrenPI.hProcess) == 0)
        {
            throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
        }
    }
}
