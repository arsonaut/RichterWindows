#include "stdafx.h"
#include "Chapter1.h"

#ifdef USE_STDLIB

std::string chapter1::GetFormattedMessage(const unsigned long messageId)
{
    return std::system_category().message(messageId);
}

#else

std::wstring chapter1::GetFormattedMessage(const unsigned long messageId)
{
    constexpr unsigned long bufferSize = 64 * 1024 / sizeof(wchar_t);
    thread_local std::vector<wchar_t> buffer(bufferSize);
    const auto charsInBuffer = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                                nullptr, messageId, LANG_USER_DEFAULT,
                                                &buffer.front(), bufferSize, nullptr);
    if (charsInBuffer == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    return std::wstring{buffer.data(), charsInBuffer};
}

#endif