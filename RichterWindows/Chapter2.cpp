#include "stdafx.h"
#include "Chapter2.h"

#ifdef USE_STDLIB

#ifdef _WIN32
// Wide strings under Windows is not actually wide, they use UTF-16 encoding
using WcharCodecvt = std::codecvt_utf8_utf16<wchar_t>;
#else
static_assert(sizeof(wchar_t) == 4, "wchar_t can't hold UCS4 codepoint");
using WcharCodecvt = std::codecvt_utf8<wchar_t>;
#endif

std::wstring chapter2::string2wstring(const std::string& str)
{
    return std::wstring_convert<WcharCodecvt>{}.from_bytes(str);
}

std::string chapter2::wstring2string(const std::wstring& str)
{
    return std::wstring_convert<WcharCodecvt>{}.to_bytes(str);
}

#else

std::wstring chapter2::string2wstring(const std::string& str)
{
    if (str.size() > std::numeric_limits<int>::max() - 1)
    {
        throw std::out_of_range{"Input string size can't be represented as int"};
    }
    const auto strSize = static_cast<int>(str.size() + 1);
    const auto requiredBufferSize = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), strSize, nullptr, 0);
    if (requiredBufferSize == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    std::wstring result;
    result.resize(requiredBufferSize);
    const auto charsInBuffer = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), strSize, &result.front(), requiredBufferSize);
    if (charsInBuffer == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    result.resize(charsInBuffer - 1);
    return result;
}

std::string chapter2::wstring2string(const std::wstring& str)
{
    if (str.size() > std::numeric_limits<int>::max() - 1)
    {
        throw std::out_of_range{"Input string size can't be represented as int"};
    }
    const auto strSize = static_cast<int>(str.size() + 1);
    const auto requiredBufferSize = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.c_str(), strSize, nullptr, 0, nullptr, nullptr);
    if (requiredBufferSize == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    std::string result;
    result.resize(requiredBufferSize);
    const auto bytesInBuffer = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.c_str(), strSize, &result.front(), requiredBufferSize, nullptr, nullptr);
    if (bytesInBuffer == 0)
    {
        throw std::system_error{static_cast<int>(::GetLastError()), std::system_category()};
    }
    result.resize(bytesInBuffer - 1);
    return result;
}

#endif