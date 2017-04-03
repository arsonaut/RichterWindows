#include "stdafx.h"

int main()
{
    std::this_thread::sleep_for(std::chrono::seconds{2});
    return 0;
}

