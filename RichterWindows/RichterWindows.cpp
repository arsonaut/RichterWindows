#include "stdafx.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter6.h"

int main()
{
    try
    {
        const auto& message1 = chapter1::GetFormattedMessage(106);
        const auto& message2 = chapter1::GetFormattedMessage(2);
        const auto& bytestr = chapter2::wstring2string(L"You are so na\u00EFve \U0001F609");
        const auto& widestr = chapter2::string2wstring(bytestr);

        uint64_t prime{18446744073709551437};
        if (chapter6::GetNextPrime(prime))
        {
            while (chapter7::PrintNextPrime(prime));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
        return 1;
    }
    return 0;
}