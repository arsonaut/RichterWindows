#include "stdafx.h"
#include "Chapter4.h"

namespace
{
    struct ProcessInformationCloser
    {
        void operator()(::PROCESS_INFORMATION* pi)
        {
            ::CloseHandle(pi->hThread);
            ::CloseHandle(pi->hProcess);
        }
    };
}

::DWORD chapter4::DuplicateHandleCheckExitCode(::HANDLE handle)
{
    ::STARTUPINFOW childrenSI{};
    childrenSI.cb = sizeof(childrenSI);
    ::PROCESS_INFORMATION childrenPI{};
    if (::CreateProcessW(L"ChildProcess.exe", nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &childrenSI, &childrenPI) == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    std::unique_ptr<::PROCESS_INFORMATION, ProcessInformationCloser> childrenPIGuard{&childrenPI};

    ::HANDLE duplicate{};
    if (::DuplicateHandle(::GetCurrentProcess(), handle, childrenPI.hProcess, &duplicate, READ_CONTROL, FALSE, 0) == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }

    if (::WaitForSingleObject(childrenPI.hProcess, INFINITE) != WAIT_OBJECT_0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    ::DWORD exitCode{};
    if (::GetExitCodeProcess(childrenPI.hProcess, &exitCode) == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    return exitCode;
}