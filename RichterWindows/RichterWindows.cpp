#include "stdafx.h"
#include "Chapter1.h"

int main()
{
    try
    {
        const auto& message1 = chapter1::GetFormattedMessage(106);
        const auto& message2 = chapter1::GetFormattedMessage(2);
    }
    catch (const std::exception& e)
    {
        std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
        return 1;
    }
    return 0;
}